#pragma once
#include "RHI_Common.h"

#include "PhysicalDevice.h"
#include "Device.h"
#include "SwapChain.h"
#include "FrameGraphExecuter.h"

namespace CGE
{
	namespace RHI
	{
		class Device;

		class Factory : public Object
		{
		public:
			Factory() = default;
			virtual ~Factory() = default;
			REMOVE_COPY_AND_MOVE(Factory);

			static Factory& Get();

		public:
			virtual Ptr<PhysicalDevice> CreatePhysicalDevice() = 0;
			virtual Ptr<Device> CreateDevice() = 0;
			virtual Ptr<SwapChain> CreateSwapChain() = 0;
			virtual Ptr<FrameGraphExecuter> CreateFrameGraphExecuter() = 0;
		};
	}
}