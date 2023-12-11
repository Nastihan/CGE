#include "DX_Factory.h"

#include "DX_PhysicalDevice.h"
#include "DX_Device.h"
#include "DX_SwapChain.h"
#include "DX_FrameGraphExecuter.h"

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

		RHI::Ptr<RHI::SwapChain> DX_Factory::CreateSwapChain()
		{
			return DX_SwapChain::Create();
		}

		RHI::Ptr<RHI::FrameGraphExecuter> DX_Factory::CreateFrameGraphExecuter()
		{
			return DX_FrameGraphExecuter::Create();
		}
	}
}