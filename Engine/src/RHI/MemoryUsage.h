#pragma once

#include <atomic>

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
	}
}