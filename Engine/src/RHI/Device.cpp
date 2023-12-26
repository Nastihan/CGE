#include "Device.h"
#include "SwapChain.h"

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

		void Device::EndFrame()
		{
			EndFrameInternal();
		}

		void Device::SetSwapChain(SwapChain* swapChain)
		{
			m_swapChain = swapChain;
		}

		SwapChain* Device::GetSwapChain()
		{
			return m_swapChain.get();
		}

		ResultCode Device::WaitForIdle()
		{
			WaitForIdleInternal();
			return ResultCode::Success;
		}
	}
}