#pragma once

// RHI
#include "DeviceObject.h"

namespace CGE
{
	namespace RHI
	{
		// There is no frame graph right now the purpose of this class is
		// just to build the commands for the current frame for now.
		class FrameGraphExecuter : public DeviceObject
		{
		public:
			virtual ~FrameGraphExecuter() = default;
			ResultCode Init(Device& device);
			void RenderFrame();

		protected:
			FrameGraphExecuter() = default;

		private:
			virtual ResultCode InitInternal(Device& device) = 0;
			virtual void RenderFrameInternal() = 0;

		protected:
			uint64_t m_currentFrame = 0;
		};
	}
}