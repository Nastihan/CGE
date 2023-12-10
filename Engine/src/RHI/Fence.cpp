#include "Fence.h"

namespace CGE
{
	namespace RHI
	{
		Fence::~Fence() {}

		ResultCode Fence::Init(Device& device, FenceState initialState)
		{
			const ResultCode resultCode = InitInternal(device, initialState);
			if (resultCode == ResultCode::Success)
			{
				DeviceObject::Init(device);
			}
			else
			{
				assert(false, "Failed to create a fence");
			}
			return resultCode;
		}

		void Fence::Shutdown()
		{
			ShutdownInternal();
			DeviceObject::Shutdown();
		}

		ResultCode Fence::SignalOnCpu()
		{
			SignalOnCpuInternal();
			return ResultCode::Success;
		}

		ResultCode Fence::WaitOnCpu() const
		{
			WaitOnCpuInternal();
			return ResultCode::Success;
		}

		ResultCode Fence::Reset()
		{
			ResetInternal();
			return ResultCode::Success;
		}

		FenceState Fence::GetFenceState() const
		{
			return GetFenceStateInternal();
		}
	}
}