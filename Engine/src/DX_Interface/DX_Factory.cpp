#include "DX_Factory.h"

#include "DX_PhysicalDevice.h"
#include "DX_Device.h"

namespace CGE
{
	namespace DX12
	{
		RHI::Ptr<RHI::PhysicalDevice> DX_Factory::CreatePhysicalDevice()
		{
			return DX_PhysicalDevice::Create();
		}
		RHI::Ptr<RHI::Device> DX_Factory::CreateDevice()
		{
			return DX_Device::Create();
		}
	}
}