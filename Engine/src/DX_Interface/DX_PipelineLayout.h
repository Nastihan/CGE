#pragma once

// RHI
#include "../RHI/PipelineLayout.h"

namespace CGE
{
	namespace DX12
	{
		class DX_PipelineLayout : public RHI::PipelineLayout
		{
		public:
			static RHI::Ptr<DX_PipelineLayout> Create();
		private:
			DX_PipelineLayout() = default;
		};
	}
}