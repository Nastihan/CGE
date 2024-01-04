#pragma once

// RHI
#include "Limits.h"
#include "DeviceObject.h"
#include "AttachmentEnums.h"

// std
#include <vector>
#include <queue>
#include <mutex>

namespace CGE
{
	namespace RHI
	{
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

			using Command = std::function<void(void* commandQueue)>;
			void QueueCommand(Command command);
			void FlushCommands();

		protected:
			virtual ResultCode InitInternal(Device& device, const CommandQueueDescriptor& descriptor) = 0;
			virtual void ExecuteWork(const ExecuteWorkRequest& request) = 0;
			virtual void WaitForIdle() = 0;
			virtual void ShutdownInternal() = 0;
			// This function will return the native queue. You have to reinterpret_cast it after.
			virtual void* GetNativeQueue() = 0;

		protected:
			CommandQueueDescriptor m_descriptor;

			// Bunch of std::function that the queue will execute
			std::queue<Command> m_workQueue;
			std::mutex m_workQueueMutex;
		};
	}
}