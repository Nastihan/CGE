#pragma once

// DX12
#include "DX_BufferMemoryView.h"
#include "DX_MemoryPageAllocator.h"
#include "DX_MemorySubAllocator.h"

// RHI
#include "../RHI/FreeListAllocator.h"
#include "../RHI/MemorySubAllocator.h"

namespace CGE
{
	namespace DX12
	{
        class DX_Device;

        // This allocator will try to sub-allocate from existing buffer pages (not existing ID3D12Resources)
        // This will help to not over commit GPU physical memory space (ID3D12Device::CreateCommitedResource)
        // When sub-allocations is done you can use the Map/Unmap methods to copy data to the heap (DX_MemoryView).
        class DX_BufferMemoryAllocator
        {
        public:
            DX_BufferMemoryAllocator() = default;
            DX_BufferMemoryAllocator(const DX_BufferMemoryAllocator&) = delete;

            using Descriptor = DX_MemoryPageAllocator::Descriptor;

            void Init(const Descriptor& descriptor);
            void Shutdown();
            void GarbageCollect();
            DX_BufferMemoryView Allocate(size_t sizeInBytes, size_t overrideAlignment = 0);
            void DeAllocate(const DX_BufferMemoryView& memory);

        private:
            DX_BufferMemoryView AllocateUnique(const RHI::BufferDescriptor& bufferDescriptor);
            void DeAllocateUnique(const DX_BufferMemoryView& memoryView);

            Descriptor m_descriptor;
            DX_MemoryPageAllocator m_pageAllocator;
            bool m_usePageAllocator = true;

            std::mutex m_subAllocatorMutex;
            DX_MemoryFreeListSubAllocator m_subAllocator;
            size_t m_subAllocationAlignment = DX_Alignment::Buffer;
        };
	}
}