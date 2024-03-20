
// DX12
#include "DX_DescriptorContext.h"
#include "DX_Buffer.h"
#include "DX_Conversions.h"

namespace CGE
{
	namespace DX12
	{
		void DX_DescriptorContext::Init(ID3D12DeviceX* device, RHI::ConstPtr<DX_PlatformLimitsDescriptor> platformLimitsDescriptor)
		{
			m_device = device;
			assert(platformLimitsDescriptor.get());
			m_platformLimitsDescriptor = platformLimitsDescriptor;

			for (const auto& itr : m_platformLimitsDescriptor->m_descriptorHeapLimits)
			{
				for (uint32_t shaderVisibleIdx = 0; shaderVisibleIdx < NumHeapFlags; ++shaderVisibleIdx)
				{
					const uint32_t descriptorCountMax = itr.second[shaderVisibleIdx];
					std::optional<D3D12_DESCRIPTOR_HEAP_TYPE> heapType = StringToDescriptorHeapType(itr.first);
					const D3D12_DESCRIPTOR_HEAP_TYPE type = static_cast<D3D12_DESCRIPTOR_HEAP_TYPE>(heapType.value());
					const D3D12_DESCRIPTOR_HEAP_FLAGS flags = static_cast<D3D12_DESCRIPTOR_HEAP_FLAGS>(shaderVisibleIdx);

					if (descriptorCountMax)
					{
						m_staticDescriptorOffset = static_cast<uint64_t>((1.f - m_platformLimitsDescriptor->m_staticDescriptorRatio) * descriptorCountMax);
						assert(m_staticDescriptorOffset < descriptorCountMax);

						if (IsShaderVisibleCbvSrvUavHeap(type, flags))
						{
							// The shader-visible CBV_SRV_UAV heap is divided into a dynamic region and a static region. The static region
							// stores pinned copies of all unique resource views and is bound via m_staticTable for bindless shader access.
							DX_DescriptorPool& dynamicPool = GetPool(static_cast<uint32_t>(heapType.value()), shaderVisibleIdx);
							dynamicPool.Init(m_device.get(), type, flags, descriptorCountMax, m_staticDescriptorOffset);

							// The remaining elements are assigned to a second pool sharing the same descriptor heap as the first
							// We assume that unbounded array is always supported on dx12 and can ignore the case when it is not 
							uint32_t staticDescriptorCount = descriptorCountMax - m_staticDescriptorOffset;
							m_staticPool.InitPooledRange(dynamicPool, m_staticDescriptorOffset, staticDescriptorCount);
							m_staticTable = DX_DescriptorTable{ DX_DescriptorHandle{ type, flags, 0 }, staticDescriptorCount };
						}
						else
						{
							GetPool(static_cast<uint32_t>(heapType.value()), shaderVisibleIdx).Init(m_device.get(), type, flags, descriptorCountMax, descriptorCountMax);
						}
					}
				}
			}
		}

		void DX_DescriptorContext::CopyDescriptor(DX_DescriptorHandle dst, DX_DescriptorHandle src)
		{
			assert(dst.m_type == src.m_type, "Cannot copy descriptors from different heaps");
			assert(!src.IsShaderVisible(), "The source descriptor cannot be shader visible.");
			m_device->CopyDescriptorsSimple(1, GetCpuPlatformHandle(dst), GetCpuPlatformHandle(src), dst.m_type);
		}

		std::optional<D3D12_DESCRIPTOR_HEAP_TYPE> DX_DescriptorContext::StringToDescriptorHeapType(std::string heapType)
		{
			if (heapType == "DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV")
			{
				return D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			}
			if (heapType == "DESCRIPTOR_HEAP_TYPE_SAMPLER")
			{
				return D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
			}
			if (heapType == "DESCRIPTOR_HEAP_TYPE_RTV")
			{
				return D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			}
			if (heapType == "DESCRIPTOR_HEAP_TYPE_DSV")
			{
				return D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
			}
			return {};
		}

		bool DX_DescriptorContext::IsShaderVisibleCbvSrvUavHeap(uint32_t type, uint32_t flag) const
		{
			return type == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV && flag == D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		}

		DX_DescriptorPool& DX_DescriptorContext::GetPool(uint32_t type, uint32_t flag)
		{
			assert(type < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES, "Trying to get pool with invalid type: [%d]", type);
			assert(flag < NumHeapFlags, "Trying to get pool with invalid flag: [%d]", flag);
			return m_pools[type][flag];
		}

		const DX_DescriptorPool& DX_DescriptorContext::GetPool(uint32_t type, uint32_t flag) const
		{
			assert(type < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES, "Trying to get pool with invalid type: [%d]", type);
			assert(flag < NumHeapFlags, "Trying to get pool with invalid flag: [%d]", flag);
			return m_pools[type][flag];
		}

		DX_DescriptorHandle DX_DescriptorContext::AllocateHandle(D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flags, uint32_t count)
		{
			return GetPool(type, flags).AllocateHandle(count);
		}

		D3D12_CPU_DESCRIPTOR_HANDLE DX_DescriptorContext::GetCpuPlatformHandle(DX_DescriptorHandle handle) const
		{
			return GetPool(handle.m_type, handle.m_flags).GetCpuPlatformHandle(handle);
		}

		void DX_DescriptorContext::CreateRenderTargetView(ID3D12Resource* backBuffer, DX_DescriptorHandle& rtv)
		{
			if (rtv.IsNull())
			{
				rtv = AllocateHandle(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 1);
				assert(!rtv.IsNull());
			}
			
			D3D12_CPU_DESCRIPTOR_HANDLE rtvDescriptorHandle = GetCpuPlatformHandle(rtv);
			m_device->CreateRenderTargetView(backBuffer, nullptr, rtvDescriptorHandle);
		}

		DX_DescriptorHandle DX_DescriptorContext::AllocateStaticDescriptor(D3D12_CPU_DESCRIPTOR_HANDLE handle)
		{
			DX_DescriptorHandle staticHandle = m_staticPool.AllocateHandle(1);
			assert(!staticHandle.IsNull());

			m_device->CopyDescriptorsSimple(1, m_staticPool.GetCpuPlatformHandle(staticHandle), handle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			return staticHandle;
		}

		void DX_DescriptorContext::CreateShaderResourceView(const DX_Buffer& buffer, const RHI::BufferViewDescriptor& bufferViewDescriptor, DX_DescriptorHandle& shaderResourceView, DX_DescriptorHandle& staticView)
		{
			if (shaderResourceView.IsNull())
			{
				shaderResourceView = AllocateHandle(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 1);
				if (shaderResourceView.IsNull())
				{
					assert(false);
					return;
				}
			}
			D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle = GetCpuPlatformHandle(shaderResourceView);

			D3D12_SHADER_RESOURCE_VIEW_DESC viewDesc;
			ConvertBufferView(buffer, bufferViewDescriptor, viewDesc);

			ID3D12Resource* resource = buffer.GetMemoryView().GetMemory();
			m_device->CreateShaderResourceView(resource, &viewDesc, descriptorHandle);
			staticView = AllocateStaticDescriptor(descriptorHandle);
		}

		void DX_DescriptorContext::CreateUnorderedAccessView(const DX_Buffer& buffer, const RHI::BufferViewDescriptor& bufferViewDescriptor, DX_DescriptorHandle& unorderedAccessView, DX_DescriptorHandle& unorderedAccessViewClear, DX_DescriptorHandle& staticView)
		{
			if (unorderedAccessView.IsNull())
			{
				unorderedAccessView = AllocateHandle(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 1);
				if (unorderedAccessView.IsNull())
				{
					assert(false);
					return;
				}
			}
			D3D12_CPU_DESCRIPTOR_HANDLE unorderedAccessDescriptor = GetCpuPlatformHandle(unorderedAccessView);

			D3D12_UNORDERED_ACCESS_VIEW_DESC viewDesc;
			ConvertBufferView(buffer, bufferViewDescriptor, viewDesc);
			m_device->CreateUnorderedAccessView(buffer.GetMemoryView().GetMemory(), nullptr, &viewDesc, unorderedAccessDescriptor);

			// Copy the UAV descriptor into the GPU-visible version for clearing.
			if (unorderedAccessViewClear.IsNull())
			{
				unorderedAccessViewClear = AllocateHandle(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, 1);
				if (unorderedAccessViewClear.IsNull())
				{
					assert(false);
					return;
				}
			}
			CopyDescriptor(unorderedAccessViewClear, unorderedAccessView);
			staticView = AllocateStaticDescriptor(unorderedAccessDescriptor);
		}

		void DX_DescriptorContext::CreateConstantBufferView(const DX_Buffer& buffer, const RHI::BufferViewDescriptor& bufferViewDescriptor, DX_DescriptorHandle& constantBufferView, DX_DescriptorHandle& staticView)
		{
			if (constantBufferView.IsNull())
			{
				constantBufferView = AllocateHandle(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 1);
			}
			D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle = GetCpuPlatformHandle(constantBufferView);

			D3D12_CONSTANT_BUFFER_VIEW_DESC viewDesc;
			ConvertBufferView(buffer, bufferViewDescriptor, viewDesc);
			m_device->CreateConstantBufferView(&viewDesc, descriptorHandle);
			staticView = AllocateStaticDescriptor(descriptorHandle);
		}

		void DX_DescriptorContext::ReleaseDescriptor(DX_DescriptorHandle descriptorHandle)
		{
			if (!descriptorHandle.IsNull())
			{
				GetPool(descriptorHandle.m_type, descriptorHandle.m_flags).ReleaseHandle(descriptorHandle);
			}
		}

		void DX_DescriptorContext::ReleaseStaticDescriptor(DX_DescriptorHandle handle)
		{
			if (!handle.IsNull())
			{
				m_staticPool.ReleaseHandle(handle);
			}
		}
	}
}