#pragma once
#include "Limits.h"
#include "DeviceObject.h"

#include <vector>

namespace CGE
{
	namespace RHI
	{
		enum class HardwareQueueClass : uint32_t
		{
			Graphics = 0,
			Compute,
			Copy,
			Count
		};
		const uint32_t HardwareQueueClassCount = static_cast<uint32_t>(HardwareQueueClass::Count);

		class SwapChain;
		struct ExecuteWorkRequest
		{
			// list of swapchains to present to after work is done.
			std::vector<SwapChain*> m_swapChainsToPresent;
		};
		struct CommandQueueDescriptor
		{
			HardwareQueueClass m_hardwareQueueClass = HardwareQueueClass::Graphics;
			int m_maxFrameQueueDepth = Limits::Device::FrameCountMax;
		};

		class CommandQueue : public DeviceObject
		{
		public:
			CommandQueue() = default;
			virtual ~CommandQueue() = default;
			ResultCode Init(Device& device, const CommandQueueDescriptor& descriptor);
			void Shutdown();

			HardwareQueueClass GetHardwareQueueClass() const;
			const CommandQueueDescriptor& GetDescriptor() const;

		protected:
			virtual ResultCode InitInternal(Device& device, const CommandQueueDescriptor& descriptor) = 0;
			virtual void ExecuteWork(const ExecuteWorkRequest& request) = 0;
			virtual void WaitForIdle() = 0;
			virtual void ShutdownInternal() = 0;

		protected:
			CommandQueueDescriptor m_descriptor;
		};
	}
}