#pragma once

// DX12
#include "DX_CommonHeaders.h"
#include "DX_Fence.h"

// RHI
#include "../RHI/CommandQueue.h"

namespace CGE
{
	namespace DX12
	{
		class DX_Fence;

		struct ExecuteWorkRequest : public RHI::ExecuteWorkRequest
		{
		};

		enum class HardwareQueueSubclass
		{
			Primary = 0,
			Secondary
		};

		struct CommandQueueDescriptor : public RHI::CommandQueueDescriptor
		{
			HardwareQueueSubclass m_hardwareQueueSubclass;
		};

		class DX_CommandQueue final : public RHI::CommandQueue
		{
		public:
			static RHI::Ptr<DX_CommandQueue> Create();

			// RHI overrides
			void ExecuteWork(const RHI::ExecuteWorkRequest& request) override;
			// Flush work on command queue (after this the commandQueue will be empty)
			void WaitForIdle() override;

			void QueueGpuSignal(DX_Fence& fence);

			ID3D12CommandQueue* GetPlatformQueue() const;

		private:
			DX_CommandQueue() = default;

			RHI::ResultCode InitInternal(RHI::Device& device, const RHI::CommandQueueDescriptor& descriptor) override;
			void ShutdownInternal() override;

			HRESULT CreateCommandQueue(ID3D12DeviceX* device, RHI::HardwareQueueClass hardwareQueueClass, HardwareQueueSubclass hardwareQueueSubclass, ID3D12CommandQueueX** commandQueue);
			const char* GetQueueName(RHI::HardwareQueueClass hardwareQueueClass, HardwareQueueSubclass hardwareQueueSubclass);
		private:
			RHI::Ptr<ID3D12CommandQueueX> m_queue;
			RHI::Ptr<ID3D12DeviceX> m_device;

			HardwareQueueSubclass m_hardwareQueueSubclass = HardwareQueueSubclass::Primary;
		};
	}
}