#pragma once

// RHI
#include "DeviceObject.h"
#include "../Pass/ForwardPass.h"

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

			// [todo] remove
			Pass::ForwardPass* GetForwardPass();

		protected:
			FrameGraphExecuter() = default;
			// [todo] This will change when I implement the frame graph.
			Pass::ForwardPass* m_forwardPass;

		private:
			virtual ResultCode InitInternal(Device& device) = 0;
			virtual void RenderFrameInternal() = 0;
		};
	}
}