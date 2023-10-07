#include "PhysicalDevice.h"

namespace CGE
{
	namespace RHI
	{
		const PhysicalDeviceDescriptor& PhysicalDevice::GetDescriptor() const
		{
			return m_descriptor;
		}
	}
}