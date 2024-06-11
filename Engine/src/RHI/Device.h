#pragma once

// RHI
#include "PhysicalDevice.h"
#include "DeviceFeatures.h"
#include "DeviceLimits.h"
#include "Object.h"
#include "PlatformLimitsDescriptor.h"
#include "BindlessSrgDescriptor.h"
#include "ShaderResourceGroupLayout.h"

// std
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
			ResultCode WaitForIdle();
			const PlatformLimitsDescriptor& GetPlatformLimitsDescriptor() const;
			const DeviceFeatures& GetFeatures() const;
			ResultCode InitBindlessSrg(RHI::Ptr<RHI::ShaderResourceGroupLayout> bindlessSrgLayout);

		protected:
		private:
			virtual ResultCode InitInternal(PhysicalDevice& physicalDevice) = 0;
			virtual void ShutdownInternal() = 0;
			virtual ResultCode BeginFrameInternal() = 0;
			virtual void EndFrameInternal() = 0;
			virtual ResultCode InitializeLimits() = 0;
			virtual void WaitForIdleInternal() = 0;
			virtual ResultCode InitInternalBindlessSrg(const BindlessSrgDescriptor& bindlessSrgDesc) = 0;
		protected:
			DeviceFeatures m_features;
			DeviceLimits m_deviceLimits;
			RHI::Ptr<PlatformLimitsDescriptor> m_platformLimitsDescriptor = nullptr;
		private:
			RHI::Ptr<PhysicalDevice> m_physicalDevice;
			RHI::Ptr<SwapChain> m_swapChain;
		};
	}
}