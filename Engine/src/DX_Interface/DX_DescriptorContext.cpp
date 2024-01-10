
// DX12
#include "DX_DescriptorContext.h"

namespace CGE
{
	namespace DX12
	{
		void DX_DescriptorContext::Init(ID3D12DeviceX* device)
		{
			m_device = device;

			for (const auto& itr : m_descriptorHeapLimits)
			{
				for (uint32_t shaderVisibleIdx = 0; shaderVisibleIdx < NumHeapFlags; ++shaderVisibleIdx)
				{
					const uint32_t descriptorCountMax = itr.second[shaderVisibleIdx];
					std::optional<D3D12_DESCRIPTOR_HEAP_TYPE> heapType = StringToDescriptorHeapType(itr.first);
					const D3D12_DESCRIPTOR_HEAP_TYPE type = static_cast<D3D12_DESCRIPTOR_HEAP_TYPE>(heapType.value());
					const D3D12_DESCRIPTOR_HEAP_FLAGS flags = static_cast<D3D12_DESCRIPTOR_HEAP_FLAGS>(shaderVisibleIdx);

					if (descriptorCountMax)
					{
						m_staticDescriptorOffset = static_cast<uint64_t>((1.f - m_staticDescriptorRatio) * descriptorCountMax);
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
	}
}