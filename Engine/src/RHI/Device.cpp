#include "Device.h"

namespace CGE
{
	namespace RHI
	{
		ResultCode Device::Init(PhysicalDevice& physicalDevice)
		{
			return InitInternal(physicalDevice);
		}
	}
}