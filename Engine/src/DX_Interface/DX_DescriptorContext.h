#pragma once

// DX12
#include "DX_CommonHeaders.h"
#include "DX_DescriptorPool.h"
#include "DX_PlatformLimitsDescriptor.h"

// RHI
#include "../RHI/BufferViewDescriptor.h"
#include "../RHI/ImageViewDescriptor.h"

// std
#include <unordered_map>
#include <array>
#include <string>
#include <optional>

namespace CGE
{
	namespace DX12
	{
		class DX_Device;
		class DX_Buffer;
		class DX_BufferView;
		class DX_Image;
		class DX_ImageView;

		class DX_DescriptorContext
		{
		public:
			DX_DescriptorContext() = default;
			void Init(ID3D12DeviceX* device, RHI::ConstPtr<DX_PlatformLimitsDescriptor> platformLimitsDescriptor);

			// [todo] change to take in image view
			void CreateRenderTargetView(ID3D12Resource* backBuffer, DX_DescriptorHandle& rtv);

			// Buffer Views
			void CreateShaderResourceView(const DX_Buffer& buffer, const RHI::BufferViewDescriptor& bufferViewDescriptor, DX_DescriptorHandle& shaderResourceView, DX_DescriptorHandle& staticView);
			void CreateUnorderedAccessView(const DX_Buffer& buffer, const RHI::BufferViewDescriptor& bufferViewDescriptor, DX_DescriptorHandle& unorderedAccessView, DX_DescriptorHandle& unorderedAccessViewClear, DX_DescriptorHandle& staticView);
			void CreateConstantBufferView(const DX_Buffer& buffer, const RHI::BufferViewDescriptor& bufferViewDescriptor, DX_DescriptorHandle& constantBufferView, DX_DescriptorHandle& staticView);

			// Image Views
			void CreateShaderResourceView(const DX_Image& image, const RHI::ImageViewDescriptor& imageViewDescriptor, DX_DescriptorHandle& shaderResourceView, DX_DescriptorHandle& staticView);
			void CreateUnorderedAccessView(const DX_Image& image, const RHI::ImageViewDescriptor& imageViewDescriptor, DX_DescriptorHandle& unorderedAccessView, DX_DescriptorHandle& unorderedAccessViewClear, DX_DescriptorHandle& staticView);
			void CreateRenderTargetView(const DX_Image& image, const RHI::ImageViewDescriptor& imageViewDescriptor, DX_DescriptorHandle& renderTargetView);
			void CreateDepthStencilView(const DX_Image& image, const RHI::ImageViewDescriptor& imageViewDescriptor, DX_DescriptorHandle& depthStencilView, DX_DescriptorHandle& depthStencilReadView);

			void ReleaseDescriptor(DX_DescriptorHandle descriptorHandle);
			void ReleaseStaticDescriptor(DX_DescriptorHandle handle);

			// Creates a GPU-visible descriptor table.
			DX_DescriptorTable CreateDescriptorTable(D3D12_DESCRIPTOR_HEAP_TYPE descriptorHeapType, uint32_t descriptorCount);
			// Performs a gather of disjoint CPU-side descriptors and copies to a contiguous GPU-side descriptor table.
			void UpdateDescriptorTableRange(DX_DescriptorTable gpuDestinationTable, const DX_DescriptorHandle* cpuSourceDescriptors, D3D12_DESCRIPTOR_HEAP_TYPE heapType);
			//! Releases a GPU-visible descriptor table.
			void ReleaseDescriptorTable(DX_DescriptorTable descriptorTable);

			DX_DescriptorHandle GetNullHandleSRV(D3D12_SRV_DIMENSION dimension) const;
			DX_DescriptorHandle GetNullHandleUAV(D3D12_UAV_DIMENSION dimension) const;
			DX_DescriptorHandle GetNullHandleCBV() const;
			DX_DescriptorHandle GetNullHandleSampler() const;

			D3D12_CPU_DESCRIPTOR_HANDLE GetCpuPlatformHandle(DX_DescriptorHandle handle) const;
			D3D12_GPU_DESCRIPTOR_HANDLE GetGpuPlatformHandle(DX_DescriptorHandle handle) const;
			D3D12_CPU_DESCRIPTOR_HANDLE GetCpuPlatformHandleForTable(DX_DescriptorTable descTable) const;
			D3D12_GPU_DESCRIPTOR_HANDLE GetGpuPlatformHandleForTable(DX_DescriptorTable descTable) const;

			// Retrieve a descriptor handle to the start of the static region of the shader-visible CBV_SRV_UAV heap
			// Used in bindless (Check DX_CommandList::CommitShaderResources
			D3D12_GPU_DESCRIPTOR_HANDLE GetBindlessGpuPlatformHandle() const;

			void SetDescriptorHeaps(ID3D12GraphicsCommandList* commandList) const;

		private:
			void CopyDescriptor(DX_DescriptorHandle dst, DX_DescriptorHandle src);
			std::optional<D3D12_DESCRIPTOR_HEAP_TYPE> StringToDescriptorHeapType(std::string heapType);
			bool IsShaderVisibleCbvSrvUavHeap(uint32_t type, uint32_t flag) const;
			DX_DescriptorPool& GetPool(uint32_t type, uint32_t flag);
			const DX_DescriptorPool& GetPool(uint32_t type, uint32_t flag) const;

			// Allocates a single descriptor handle
			DX_DescriptorHandle AllocateHandle(D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flags, uint32_t count);

			// Gets a descriptor allocated on the CPU visible heap and creates a copy in the static part of the shader visible heap
			DX_DescriptorHandle AllocateStaticDescriptor(D3D12_CPU_DESCRIPTOR_HANDLE handle);

			void CreateNullDescriptors();
			void CreateNullDescriptorsSRV();
			void CreateNullDescriptorsUAV();
			void CreateNullDescriptorsCBV();
			void CreateNullDescriptorsSampler();

		private:
			static const uint32_t NumHeapFlags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE + 1;
			static const uint32_t m_descriptorCountMax[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES][NumHeapFlags];
			RHI::Ptr<ID3D12DeviceX> m_device;
			DX_DescriptorPool m_pools[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES][NumHeapFlags];

			// Descriptors that persist for the lifetime of the resource view they reference this is the bindless pool
			DX_DescriptorPool m_staticPool;
			// This table binds the entire range of CBV_SRV_UAV descriptor handles in the shader visible heap (used for bindless)
			DX_DescriptorTable m_staticTable;
			// Offset from the shader-visible descriptor heap start to the first static descriptor handle
			uint32_t m_staticDescriptorOffset = 0;
			RHI::ConstPtr<DX_PlatformLimitsDescriptor> m_platformLimitsDescriptor;

			// If some resources are not avalible in the srg's, bind null descriptors to the commandlist.
			// These will get initilized once on the cpu heap and then copied into tables in the gpu visible heap when srg's are being compiled.
			std::unordered_map<D3D12_SRV_DIMENSION, DX_DescriptorHandle> m_nullDescriptorsSRV;
			std::unordered_map<D3D12_UAV_DIMENSION, DX_DescriptorHandle> m_nullDescriptorsUAV;
			DX_DescriptorHandle m_nullDescriptorCBV;
			DX_DescriptorHandle m_nullSamplerDescriptor;
		};
	}
}