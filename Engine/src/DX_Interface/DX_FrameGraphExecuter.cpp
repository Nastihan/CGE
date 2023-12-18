
// DX12
#include "DX_FrameGraphExecuter.h"

namespace CGE
{
	namespace DX12
	{
		RHI::Ptr<DX_FrameGraphExecuter> DX_FrameGraphExecuter::Create()
		{
			return new DX_FrameGraphExecuter();
		}

		RHI::ResultCode DX_FrameGraphExecuter::InitInternal(RHI::Device& device)
		{
			return RHI::ResultCode::Success;
		}

		void DX_FrameGraphExecuter::RenderFrameInternal()
		{
			m_currentFrame++;
		}
	}
}