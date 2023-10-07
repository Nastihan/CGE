#pragma once

#include "DeviceFeatures.h"
#include "DeviceLimits.h"
#include <memory>


namespace CGE
{
	namespace RHI
	{
		class Device
		{
		public:
			ResultCode Init(PhysicalDevice& physicalDevice);
			void ShutDown();
		protected:
		private:
			virtual ResultCode InitInternal(PhysicalDevice& physicalDevice) = 0;
			virtual void ShutDownInternal() = 0;
		protected:
			DeviceFeatures m_features;
			DeviceLimits m_deviceLimits;
			//[todo] add device descriptor which houses frames in flight
		private:
			std::shared_ptr<PhysicalDevice> m_physicalDevice;
		};
	}
}