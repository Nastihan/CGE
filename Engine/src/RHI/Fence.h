#pragma once

#include "DeviceObject.h"

#include <functional>
#include <thread>

namespace CGE
{
	namespace RHI
	{
		enum class FenceState : uint32_t
		{
			Reset = 0,
			Signaled
		};

		class Fence : public DeviceObject
		{
		public:
			virtual ~Fence() = 0;
			ResultCode Init(Device& device, FenceState initialState);
			void Shutdown() override final;
			RHI::ResultCode SignalOnCpu();
			RHI::ResultCode WaitOnCpu() const;
			RHI::ResultCode Reset();
			FenceState GetFenceState() const;
			using SignalCallback = std::function<void()>;
			ResultCode WaitOnCpuAsync(SignalCallback callback);
		protected:
			virtual ResultCode InitInternal(Device& device, FenceState initialState) = 0;
			virtual void ShutdownInternal() = 0;
			virtual void SignalOnCpuInternal() = 0;
			virtual void WaitOnCpuInternal() const = 0;
			virtual void ResetInternal() = 0;
			virtual FenceState GetFenceStateInternal() const = 0;

		protected:
			std::thread m_waitThread;
		};
	}
}