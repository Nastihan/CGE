#pragma once

// RHI
#include "MemoryEnums.h"

// std
#include <atomic>
#include <array>
#include <vector>

namespace CGE
{
	namespace RHI
	{
		struct HeapMemoryUsage
		{
			HeapMemoryUsage() = default;

            bool CanAllocate(size_t sizeInBytes) const;
            void Validate();

            // The budget for the heap in bytes. A non-zero budget means the pool will reject reservation requests if the budget is exceeded.
            // This will be the total budget the pool can go up to.
            size_t m_budgetInBytes = 0;

            // This is the total physical memory allocated. It cannot exceed the budget.
            std::atomic_size_t m_totalResidentInBytes{ 0 };

            // This is the current used memory for resources and objects. (sub-allocations in the pool pages)
            std::atomic_size_t m_usedResidentInBytes{ 0 };

            // Number of bytes used by Unique Allocations. (This is for objects that are bigger than the page size for the pool)
            std::atomic_size_t m_uniqueAllocationBytes{ 0 };
		};

        struct PoolMemoryUsage
        {
            PoolMemoryUsage() = default;
            PoolMemoryUsage(const PoolMemoryUsage&) = default;
            PoolMemoryUsage& operator=(const PoolMemoryUsage&) = default;

            HeapMemoryUsage& GetHeapMemoryUsage(HeapMemoryLevel memoryType)
            {
                return m_memoryUsagePerLevel[static_cast<size_t>(memoryType)];
            }

            const HeapMemoryUsage& GetHeapMemoryUsage(HeapMemoryLevel memoryType) const
            {
                return m_memoryUsagePerLevel[static_cast<size_t>(memoryType)];
            }

            // The memory heap usages of this pool for each level in the hierarchy. (Host and device levels)
            std::array<HeapMemoryUsage, 2> m_memoryUsagePerLevel;
        };
	}
}