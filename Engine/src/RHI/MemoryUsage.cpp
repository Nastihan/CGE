
// RHI
#include "MemoryUsage.h"
#include "RHI_Common.h"

namespace CGE
{
	namespace RHI
	{
		bool HeapMemoryUsage::CanAllocate(size_t sizeInBytes) const
		{
			if (m_budgetInBytes && (m_usedResidentInBytes + sizeInBytes) > m_budgetInBytes)
			{
				return false;
			}
			return true;
		}

		void HeapMemoryUsage::Validate()
		{
			assert(m_budgetInBytes == 0 || m_budgetInBytes >= m_usedResidentInBytes);
		}
	}
}