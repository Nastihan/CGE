#pragma once

// DX12
#include "DX_MemoryPageAllocator.h"
#include "DX_MemorySubAllocator.h"

namespace CGE
{
	namespace DX12
	{
        class DX_Device;

        class DX_StagingMemoryAllocator
        {
        public:
            DX_StagingMemoryAllocator();
            DX_StagingMemoryAllocator(const DX_StagingMemoryAllocator&) = delete;

            struct Descriptor
            {
                DX_Device* m_device = nullptr;
                uint32_t m_mediumPageSizeInBytes = 0;
                uint32_t m_largePageSizeInBytes = 0;
                uint32_t m_collectLatency = 0;
            };

            void Init(const Descriptor& descriptor);
            void Shutdown();
            void GarbageCollect();
            DX_MemoryView Allocate(size_t sizeInBytes, size_t alignmentInBytes);
            DX_MemoryPageAllocator& GetMediumPageAllocator();

        private:
            DX_MemoryView AllocateUnique(size_t sizeInBytes);

            DX_Device* m_device = nullptr;
            RHI::HeapMemoryUsage m_memoryUsage;

            /**
             * Small allocations are done from a thread-local linear allocator that pulls
             * pages from a central page allocator. This allows small allocations to be very
             * low contention.
             */
            // No thread local for now.
            DX_MemoryPageAllocator m_mediumPageAllocator;
            DX_MemoryLinearSubAllocator m_mediumBlockAllocators;

            /**
             * Large allocations are done through a separate page pool (with large pages) and
             * uses a lock. We should have few large allocations per frame for things like streaming
             * image or geometry uploads.
             */
            DX_MemoryPageAllocator m_largePageAllocator;
            DX_MemoryLinearSubAllocator m_largeBlockAllocator;
        };
	}
}