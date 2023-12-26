#pragma once

#include "PhysicalDevice.h"
#include "DeviceFeatures.h"
#include "DeviceLimits.h"
#include "Object.h"
#include <memory>

namespace CGE
{
	namespace RHI
	{
		class SwapChain;

		class Device : public Object
		{
		public:
			ResultCode Init(PhysicalDevice& physicalDevice);

			void SetSwapChain(SwapChain* swapChain);
			SwapChain* GetSwapChain();
			void EndFrame();
		protected:
		private:
			virtual ResultCode InitInternal(PhysicalDevice& physicalDevice) = 0;
			virtual void ShutdownInternal() = 0;
			virtual ResultCode BeginFrameInternal() = 0;
			virtual void EndFrameInternal() = 0;
			virtual ResultCode InitializeLimits() = 0;
		protected:
			DeviceFeatures m_features;
			DeviceLimits m_deviceLimits;
			//[todo] add device descriptor which houses frames in flight
		private:
			RHI::Ptr<PhysicalDevice> m_physicalDevice;
			RHI::Ptr<SwapChain> m_swapChain;
		};
	}
}