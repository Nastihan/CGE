#pragma once

#include "intrusive_base.h"

namespace CGE
{
	namespace RHI
	{
		class PipelineLayout : public intrusive_base
		{
		public:
			virtual ~PipelineLayout() = default;
		};
	}
}