
// RHI
#include "MemoryUsage.h"
#include "RHI_Common.h"

namespace CGE
{
	namespace RHI
	{
		bool HeapMemoryUsage::TryReserveMemory(size_t sizeInBytes)
		{
			const size_t reservationInBytes = (m_reservedInBytes + sizeInBytes);

			if (m_budgetInBytes && reservationInBytes > m_budgetInBytes)
			{
				return false;
			}

			m_reservedInBytes += sizeInBytes;
			return true;
		}

		void HeapMemoryUsage::Validate()
		{
			assert(m_budgetInBytes >= m_reservedInBytes);
			assert(m_reservedInBytes >= m_reservedInBytes);
		}
	}
}