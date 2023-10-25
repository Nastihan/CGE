#include "CommandQueue.h"

namespace CGE
{
	namespace RHI
	{
		ResultCode CommandQueue::Init(Device& device, const CommandQueueDescriptor& descriptor)
		{
			const ResultCode resultCode = InitInternal(device, descriptor);
			if (resultCode == ResultCode::Success)
			{
				DeviceObject::Init(device);
				m_descriptor = descriptor;
			}
			return resultCode;
		}
		void CommandQueue::Shutdown()
		{
			if (IsInitilized())
			{
				ShutdownInternal();
				DeviceObject::Shutdown();
			}
		}
		HardwareQueueClass CommandQueue::GetHardwareQueueClass() const
		{
			return m_descriptor.m_hardwareQueueClass;
		}
		const CommandQueueDescriptor& CommandQueue::GetDescriptor() const
		{
			return m_descriptor;
		}
	}
}