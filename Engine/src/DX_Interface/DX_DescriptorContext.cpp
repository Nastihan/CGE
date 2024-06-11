
// DX12
#include "DX_DescriptorContext.h"
#include "DX_Buffer.h"
#include "DX_Conversions.h"
#include "DX_Image.h"

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
			CreateNullDescriptors();
		}

		DX_DescriptorTable DX_DescriptorContext::CreateDescriptorTable(D3D12_DESCRIPTOR_HEAP_TYPE descriptorHeapType, uint32_t descriptorCount)
		{
			return GetPool(descriptorHeapType, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE).AllocateTable(descriptorCount);
		}

		void DX_DescriptorContext::UpdateDescriptorTableRange(DX_DescriptorTable gpuDestinationTable, const DX_DescriptorHandle* cpuSourceDescriptors, D3D12_DESCRIPTOR_HEAP_TYPE heapType)
		{
			const uint32_t descriptorCount = gpuDestinationTable.GetSize();
			std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> cpuSourceHandles;
			cpuSourceHandles.reserve(descriptorCount);
			for (uint32_t i = 0; i < descriptorCount; ++i)
			{
				cpuSourceHandles.push_back(GetCpuPlatformHandle(cpuSourceDescriptors[i]));
			}

			D3D12_CPU_DESCRIPTOR_HANDLE gpuDestinationHandle = GetCpuPlatformHandleForTable(gpuDestinationTable);

			// An array of descriptor sizes for each range. We just want N ranges with 1 descriptor each. (check ID3D12Device::CopyDescriptors)
			std::vector<uint32_t> rangeCounts(descriptorCount, 1);
			m_device->CopyDescriptors(1, &gpuDestinationHandle, &descriptorCount, descriptorCount, cpuSourceHandles.data(), rangeCounts.data(), heapType);
		}

		void DX_DescriptorContext::ReleaseDescriptorTable(DX_DescriptorTable table)
		{
			GetPool(table.GetType(), table.GetFlags()).ReleaseTable(table);
		}

		DX_DescriptorHandle DX_DescriptorContext::GetNullHandleSRV(D3D12_SRV_DIMENSION dimension) const
		{
			auto iter = m_nullDescriptorsSRV.find(dimension);
			if (iter != m_nullDescriptorsSRV.end())
			{
				return iter->second;
			}
			else
			{
				return DX_DescriptorHandle();
			}
		}

		DX_DescriptorHandle DX_DescriptorContext::GetNullHandleUAV(D3D12_UAV_DIMENSION dimension) const
		{
			auto iter = m_nullDescriptorsUAV.find(dimension);
			if (iter != m_nullDescriptorsUAV.end())
			{
				return iter->second;
			}
			else
			{
				return DX_DescriptorHandle();
			}
		}

		DX_DescriptorHandle DX_DescriptorContext::GetNullHandleCBV() const
		{
			return m_nullDescriptorCBV;
		}

		DX_DescriptorHandle DX_DescriptorContext::GetNullHandleSampler() const
		{
			return m_nullSamplerDescriptor;
		}

		D3D12_CPU_DESCRIPTOR_HANDLE DX_DescriptorContext::GetCpuPlatformHandle(DX_DescriptorHandle handle) const
		{
			return GetPool(handle.m_type, handle.m_flags).GetCpuPlatformHandle(handle);
		}

		D3D12_GPU_DESCRIPTOR_HANDLE DX_DescriptorContext::GetGpuPlatformHandle(DX_DescriptorHandle handle) const
		{
			return GetPool(handle.m_type, handle.m_flags).GetGpuPlatformHandle(handle);
		}

		D3D12_CPU_DESCRIPTOR_HANDLE DX_DescriptorContext::GetCpuPlatformHandleForTable(DX_DescriptorTable descTable) const
		{
			return GetPool(descTable.GetOffset().m_type, descTable.GetOffset().m_flags).GetCpuPlatformHandleForTable(descTable);
		}

		D3D12_GPU_DESCRIPTOR_HANDLE DX_DescriptorContext::GetGpuPlatformHandleForTable(DX_DescriptorTable descTable) const
		{
			return GetPool(descTable.GetOffset().m_type, descTable.GetOffset().m_flags).GetGpuPlatformHandleForTable(descTable);
		}

		D3D12_GPU_DESCRIPTOR_HANDLE DX_DescriptorContext::GetBindlessGpuPlatformHandle() const
		{
			return m_staticPool.GetGpuPlatformHandleForTable(m_staticTable);
		}

		void DX_DescriptorContext::SetDescriptorHeaps(ID3D12GraphicsCommandList* commandList) const
		{
			ID3D12DescriptorHeap* heaps[2];
			heaps[0] = GetPool(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE).GetPlatformHeap();
			heaps[1] = GetPool(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE).GetPlatformHeap();
			commandList->SetDescriptorHeaps(2, heaps);
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

		void DX_DescriptorContext::CreateShaderResourceView(const DX_Image& image, const RHI::ImageViewDescriptor& imageViewDescriptor, DX_DescriptorHandle& shaderResourceView, DX_DescriptorHandle& staticView)
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
			ConvertImageView(image, imageViewDescriptor, viewDesc);
			m_device->CreateShaderResourceView(image.GetMemoryView().GetMemory(), &viewDesc, descriptorHandle);

			// Only allocate if the index is already null, otherwise just copy the descriptor onto the old index.
			if (staticView.m_index == DX_DescriptorHandle::NullIndex)
			{
				staticView = AllocateStaticDescriptor(descriptorHandle);
			}
			else
			{
				m_device->CopyDescriptorsSimple(1, m_staticPool.GetCpuPlatformHandle(staticView), descriptorHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			}
		}

		void DX_DescriptorContext::CreateUnorderedAccessView(const DX_Image& image, const RHI::ImageViewDescriptor& imageViewDescriptor, DX_DescriptorHandle& unorderedAccessView, DX_DescriptorHandle& unorderedAccessViewClear, DX_DescriptorHandle& staticView)
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
			ConvertImageView(image, imageViewDescriptor, viewDesc);
			m_device->CreateUnorderedAccessView(image.GetMemoryView().GetMemory(), nullptr, &viewDesc, unorderedAccessDescriptor);

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

			if (staticView.m_index == DX_DescriptorHandle::NullIndex)
			{
				staticView = AllocateStaticDescriptor(unorderedAccessDescriptor);
			}
			else
			{
				m_device->CopyDescriptorsSimple(1, m_staticPool.GetCpuPlatformHandle(staticView), unorderedAccessDescriptor, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			}
		}

		void DX_DescriptorContext::CreateRenderTargetView(const DX_Image& image, const RHI::ImageViewDescriptor& imageViewDescriptor, DX_DescriptorHandle& renderTargetView)
		{
			if (renderTargetView.IsNull())
			{
				renderTargetView = AllocateHandle(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 1);
				if (renderTargetView.IsNull())
				{
					assert(false);
					return;
				}
			}
			D3D12_CPU_DESCRIPTOR_HANDLE renderTargetDescriptor = GetCpuPlatformHandle(renderTargetView);

			D3D12_RENDER_TARGET_VIEW_DESC viewDesc;
			ConvertImageView(image, imageViewDescriptor, viewDesc);
			m_device->CreateRenderTargetView(image.GetMemoryView().GetMemory(), &viewDesc, renderTargetDescriptor);
		}

		void DX_DescriptorContext::CreateDepthStencilView(const DX_Image& image, const RHI::ImageViewDescriptor& imageViewDescriptor, DX_DescriptorHandle& depthStencilView, DX_DescriptorHandle& depthStencilReadView)
		{
			if (depthStencilView.IsNull())
			{
				depthStencilView = AllocateHandle(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 1);
				if (depthStencilView.IsNull())
				{
					assert(false);
					return;
				}
			}
			D3D12_CPU_DESCRIPTOR_HANDLE depthStencilDescriptor = GetCpuPlatformHandle(depthStencilView);

			if (depthStencilReadView.IsNull())
			{
				depthStencilReadView = AllocateHandle(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 1);
				if (depthStencilReadView.IsNull())
				{
					assert(false);
					return;
				}
			}
			D3D12_CPU_DESCRIPTOR_HANDLE depthStencilReadDescriptor = GetCpuPlatformHandle(depthStencilReadView);

			D3D12_DEPTH_STENCIL_VIEW_DESC viewDesc;
			ConvertImageView(image, imageViewDescriptor, viewDesc);
			m_device->CreateDepthStencilView(image.GetMemoryView().GetMemory(), &viewDesc, depthStencilDescriptor);
			viewDesc.Flags = D3D12_DSV_FLAG_READ_ONLY_DEPTH;

			const bool isStencilFormat = GetStencilFormat(viewDesc.Format) != DXGI_FORMAT_UNKNOWN;
			if (isStencilFormat)
			{
				viewDesc.Flags |= D3D12_DSV_FLAG_READ_ONLY_STENCIL;
			}
			m_device->CreateDepthStencilView(image.GetMemoryView().GetMemory(), &viewDesc, depthStencilReadDescriptor);
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

		void DX_DescriptorContext::CreateNullDescriptors()
		{
			CreateNullDescriptorsSRV();
			CreateNullDescriptorsUAV();
			CreateNullDescriptorsCBV();
			CreateNullDescriptorsSampler();
		}

		void DX_DescriptorContext::CreateNullDescriptorsSRV()
		{
			const std::array<D3D12_SRV_DIMENSION, 10> validSRVDimensions = { D3D12_SRV_DIMENSION_BUFFER,
				D3D12_SRV_DIMENSION_TEXTURE1D,
				D3D12_SRV_DIMENSION_TEXTURE1DARRAY,
				D3D12_SRV_DIMENSION_TEXTURE2D,
				D3D12_SRV_DIMENSION_TEXTURE2DARRAY,
				D3D12_SRV_DIMENSION_TEXTURE2DMS,
				D3D12_SRV_DIMENSION_TEXTURE2DMSARRAY,
				D3D12_SRV_DIMENSION_TEXTURE3D,
				D3D12_SRV_DIMENSION_TEXTURECUBE,
				D3D12_SRV_DIMENSION_TEXTURECUBEARRAY };

			for (D3D12_SRV_DIMENSION dimension : validSRVDimensions)
			{
				DX_DescriptorHandle srvDescriptorHandle = AllocateHandle(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 1);
				D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
				desc.Format = DXGI_FORMAT_R32_UINT;
				desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
				desc.ViewDimension = dimension;
				m_device->CreateShaderResourceView(nullptr, &desc, GetCpuPlatformHandle(srvDescriptorHandle));
				m_nullDescriptorsSRV[dimension] = srvDescriptorHandle;
			}
		}

		void DX_DescriptorContext::CreateNullDescriptorsUAV()
		{
			const std::array<D3D12_UAV_DIMENSION, 6> UAVDimensions = { D3D12_UAV_DIMENSION_BUFFER,
				D3D12_UAV_DIMENSION_TEXTURE1D,
				D3D12_UAV_DIMENSION_TEXTURE1DARRAY,
				D3D12_UAV_DIMENSION_TEXTURE2D,
				D3D12_UAV_DIMENSION_TEXTURE2DARRAY,
				D3D12_UAV_DIMENSION_TEXTURE3D };

			for (D3D12_UAV_DIMENSION dimension : UAVDimensions)
			{
				DX_DescriptorHandle uavDescriptorHandle = AllocateHandle(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 1);
				D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {};
				desc.Format = DXGI_FORMAT_R32_UINT;
				desc.ViewDimension = dimension;
				m_device->CreateUnorderedAccessView(nullptr, nullptr, &desc, GetCpuPlatformHandle(uavDescriptorHandle));
				m_nullDescriptorsUAV[dimension] = uavDescriptorHandle;
			}
		}

		void DX_DescriptorContext::CreateNullDescriptorsCBV()
		{
			D3D12_CONSTANT_BUFFER_VIEW_DESC constantBufferDesc = {};
			DX_DescriptorHandle cbvDescriptorHandle = AllocateHandle(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 1);
			m_device->CreateConstantBufferView(&constantBufferDesc, GetCpuPlatformHandle(cbvDescriptorHandle));
			m_nullDescriptorCBV = cbvDescriptorHandle;
		}

		void DX_DescriptorContext::CreateNullDescriptorsSampler()
		{
			m_nullSamplerDescriptor = AllocateHandle(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 1);
			D3D12_SAMPLER_DESC samplerDesc = {};
			samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
			samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
			samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
			samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
			samplerDesc.MinLOD = 0;
			samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
			samplerDesc.MipLODBias = 0.0f;
			samplerDesc.MaxAnisotropy = 1;
			samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
			m_device->CreateSampler(&samplerDesc, GetCpuPlatformHandle(m_nullSamplerDescriptor));
		}
	}
}