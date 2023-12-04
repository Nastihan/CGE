#pragma once

// DX12
#include "DX_CommonHeaders.h"
#include "DX_Descriptor.h"

// RHI
#include "../RHI/FreeListAllocator.h"

// std
#include <mutex>

namespace CGE
{
	namespace DX12
	{
		class DX_DescriptorPool
		{
		public:
			DX_DescriptorPool() = default;
			virtual ~DX_DescriptorPool() = default;

			// Initialize the native heap as well as init the allocators tracking the memory for descriptor handles
			virtual void Init(ID3D12DeviceX* device, D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flags, uint32_t descriptorCountForHeap, uint32_t descriptorCountForAllocator);
			// Initialize a descriptor pool mapping a range of descriptors from a parent heap. Descriptors are allocated using the RHI::PoolAllocator
			void InitPooledRange(DX_DescriptorPool& parent, uint32_t offset, uint32_t count);
			ID3D12DescriptorHeap* GetPlatformHeap() const;
			DX_DescriptorHandle AllocateHandle(uint32_t count = 1);
			void ReleaseHandle(DX_DescriptorHandle table);
			virtual DX_DescriptorTable AllocateTable(uint32_t count = 1);
			virtual void ReleaseTable(DX_DescriptorTable table);
			virtual void GarbageCollect();

			virtual D3D12_CPU_DESCRIPTOR_HANDLE GetCpuPlatformHandleForTable(DX_DescriptorTable handle) const;
			virtual D3D12_GPU_DESCRIPTOR_HANDLE GetGpuPlatformHandleForTable(DX_DescriptorTable handle) const;
			D3D12_CPU_DESCRIPTOR_HANDLE GetCpuPlatformHandle(DX_DescriptorHandle handle) const;
			D3D12_GPU_DESCRIPTOR_HANDLE GetGpuPlatformHandle(DX_DescriptorHandle handle) const;

		protected:
			D3D12_DESCRIPTOR_HEAP_DESC m_desc;
			D3D12_CPU_DESCRIPTOR_HANDLE m_cpuStart = {};
			D3D12_GPU_DESCRIPTOR_HANDLE m_gpuStart = {};
			uint32_t m_stride = 0;
			std::mutex m_mutex;
		private:
			wrl::ComPtr<ID3D12DescriptorHeap> m_descriptorHeap;
			std::unique_ptr<RHI::Allocator> m_allocator;
		};
	}
}