
// RHI
#include "FrameGraphExecuter.h"

namespace CGE
{
	namespace RHI
	{
		ResultCode FrameGraphExecuter::Init(Device& device)
		{
			m_forwardPass = new Pass::ForwardPass();
			m_forwardPass->Init(device);

			const ResultCode resultCode = InitInternal(device);
			if (resultCode == RHI::ResultCode::Success)
			{
				DeviceObject::Init(device);
			}
			return resultCode;
		}

		void FrameGraphExecuter::RenderFrame()
		{
			RenderFrameInternal();
			GetDevice().EndFrame();
		}

		Pass::ForwardPass* FrameGraphExecuter::GetForwardPass()
		{
			return m_forwardPass;
		}
	}
}