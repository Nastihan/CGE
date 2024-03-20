#pragma once

// RHI
#include "ResourcePoolDescriptor.h"
#include "MemoryEnums.h"
#include "BufferDescriptor.h"

namespace CGE
{
	namespace RHI
	{
        class BufferPoolDescriptor : public ResourcePoolDescriptor
        {
        public:
            virtual ~BufferPoolDescriptor() = default;
            BufferPoolDescriptor() = default;

            HeapMemoryLevel m_heapMemoryLevel = HeapMemoryLevel::Device;
            HostMemoryAccess m_hostMemoryAccess = HostMemoryAccess::Write;
            
            // Must match parent pool.
            BufferBindFlags m_bindFlags = BufferBindFlags::None;
            u64 m_largestPooledAllocationSizeInBytes = 0;
            HardwareQueueClassMask m_sharedQueueMask = HardwareQueueClassMask::All;
        };
	}
}