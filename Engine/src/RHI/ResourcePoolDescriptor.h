#pragma once

#include "RHI_Common.h"

namespace CGE
{
	namespace RHI
	{
		class ResourcePoolDescriptor
		{
		public:
			ResourcePoolDescriptor() = default;
			virtual ~ResourcePoolDescriptor() = default;

			u64 m_budgetInBytes = 0;
		};
	}
}