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

		const PlatformLimitsDescriptor& Device::GetPlatformLimitsDescriptor() const
		{
			return *m_platformLimitsDescriptor;
		}

		const DeviceFeatures& Device::GetFeatures() const
		{
			return m_features;
		}

		ResultCode Device::InitBindlessSrg(RHI::Ptr<RHI::ShaderResourceGroupLayout> bindlessSrgLayout)
		{
			BindlessSrgDescriptor bindlessSrgDesc;
			bindlessSrgDesc.m_bindlesSrgBindingSlot = bindlessSrgLayout->GetBindingSlot();
			bool isUnboundedArraySupported = GetFeatures().m_unboundedArrays;

			if (isUnboundedArraySupported)
			{
				return InitInternalBindlessSrg(bindlessSrgDesc);
			}
		}
	}
}