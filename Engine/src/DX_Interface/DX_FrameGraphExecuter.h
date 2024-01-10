#pragma once

// RHI
#include "../RHI/FrameGraphExecuter.h"

namespace CGE
{
	namespace DX12
	{
		// There is no frame graph right now the purpose of this class is
		// just to build the commands for the current frame for now.
		class DX_FrameGraphExecuter final : public RHI::FrameGraphExecuter
		{
		public:
			static RHI::Ptr<DX_FrameGraphExecuter> Create();
		private:
			DX_FrameGraphExecuter() = default;

			// RHI::FrameGraphExecuter
			RHI::ResultCode InitInternal(RHI::Device& device) override;
			void RenderFrameInternal() override;
		};
	}
}