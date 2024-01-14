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

            bool TryReserveMemory(size_t sizeInBytes);
            void Validate();

            // The budget for the heap in bytes. A non-zero budget means the pool will reject reservation requests
            size_t m_budgetInBytes = 0;

            // Number of bytes reserved on the heap for allocation. Cannot exceed budget.
            std::atomic_size_t m_reservedInBytes{ 0 };

            // Number of bytes physically allocated on the heap. Cannot exceed reservation.
            std::atomic_size_t m_residentInBytes{ 0 };
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

            // The memory heap usages of this pool for each level in the hierarchy.
            std::array<HeapMemoryUsage, 2> m_memoryUsagePerLevel;
        };
	}
}