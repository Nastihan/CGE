
// RHI
#include "FrameGraphExecuter.h"

namespace CGE
{
	namespace RHI
	{
		ResultCode FrameGraphExecuter::Init(Device& device)
		{
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
		}
	}
}