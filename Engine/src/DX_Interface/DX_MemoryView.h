#pragma once

// DX12
#include "DX_CommonHeaders.h"

// RHI
#include "../RHI/MemoryAllocation.h"
#include "../RHI/MemoryEnums.h"

namespace CGE
{
	namespace DX12
	{
		enum class DX_MemoryViewType
		{
			Image = 0,
			Buffer
		};
		
		using DX_Memory = ID3D12Resource;
		using DX_GpuVirtualAddress = D3D12_GPU_VIRTUAL_ADDRESS;
		using DX_MemoryAllocation = RHI::MemoryAllocation<DX_Memory>;
		using DX_CpuVirtualAddress = uint8_t*;

		class DX_MemoryView
		{
			friend class DX_BufferMemoryAllocator;

		public:
			DX_MemoryView() = default;
			DX_MemoryView(RHI::Ptr<DX_Memory> memory, size_t offset, size_t size, size_t alignment, DX_MemoryViewType viewType);
			DX_MemoryView(const DX_MemoryAllocation& memoryAllocation, DX_MemoryViewType viewType);

			DX_MemoryView(const DX_MemoryView& rhs) = default;
			DX_MemoryView(DX_MemoryView && rhs) = default;
			DX_MemoryView& operator=(const DX_MemoryView & rhs) = default;
			DX_MemoryView& operator=(DX_MemoryView && rhs) = default;

			bool IsValid() const;

			// Returns the offset relative to the base memory address in bytes.
			size_t GetOffset() const;

			// Returns the size of the memory view region in bytes.
			size_t GetSize() const;

			// Returns the alignment of the memory view region in bytes.
			size_t GetAlignment() const;

			// Returns a pointer to the memory chunk this view is sub-allocated from.
			DX_Memory* GetMemory() const;

			// A convenience method to map the resource region spanned by the view for CPU access.
			DX_CpuVirtualAddress Map(RHI::HostMemoryAccess hostAccess) const;

			// A convenience method for unmapping the resource region spanned by the view.
			void Unmap(RHI::HostMemoryAccess hostAccess) const;

			// Returns the GPU address, offset to match the view.
			DX_GpuVirtualAddress GetGpuAddress() const;

			// Sets the name of the ID3D12Resource.
			void SetName(const std::string& name);
			void SetName(const std::wstring& name);

		private:
			void Construct();

		private:
			DX_GpuVirtualAddress m_gpuAddress = 0;
			DX_MemoryAllocation m_memoryAllocation;
			DX_MemoryViewType m_viewType;
		};
	}
}