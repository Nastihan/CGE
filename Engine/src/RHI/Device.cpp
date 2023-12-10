#include "Device.h"

namespace CGE
{
	namespace RHI
	{
		ResultCode Device::Init(PhysicalDevice& physicalDevice)
		{
			ResultCode resultCode = InitInternal(physicalDevice);
			if (resultCode == ResultCode::Success)
			{
				resultCode = InitializeLimits();
			}
			return resultCode;
		}
	}
}