#pragma once

// RHI
#include "ResourcePoolDescriptor.h"
#include "MemoryEnums.h"
#include "BufferDescriptor.h"

namespace CGE
{
	namespace RHI
	{
        // ResourcePoolDescriptor contains the budget.
        class BufferPoolDescriptor : public ResourcePoolDescriptor
        {
        public:
            virtual ~BufferPoolDescriptor() = default;
            BufferPoolDescriptor() = default;

            // Host visible / Device Local.
            HeapMemoryLevel m_heapMemoryLevel = HeapMemoryLevel::Device;
            // For Host visible Write will be fast for CPU wirte GPU read. Read will be fast for GPU wirte CPU read.
            // For Device local this must be Write as only write operations are allowed on Map.
            HostMemoryAccess m_hostMemoryAccess = HostMemoryAccess::Write;
            
            // Must match parent pool. Specifies how the buffers will be used by the pipeline.
            BufferBindFlags m_bindFlags = BufferBindFlags::None;
            u64 m_largestPooledAllocationSizeInBytes = 0;
            HardwareQueueClassMask m_sharedQueueMask = HardwareQueueClassMask::All;
        };
	}
}