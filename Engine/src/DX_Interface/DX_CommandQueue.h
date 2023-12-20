#pragma once

// DX12
#include "DX_CommonHeaders.h"
#include "DX_Fence.h"

// RHI
#include "../RHI/CommandQueue.h"

// std
#include <vector>
#include <array>

namespace CGE
{
	namespace DX12
	{
		class DX_Fence;
		class DX_CommandList;

		struct DX_ExecuteWorkRequest : public RHI::ExecuteWorkRequest
		{
			static const uint64_t FenceValueNull = 0;

			// Command lists to queue
			std::vector<DX_CommandList*> m_commandLists;

			// Fence values for the queue to wait on before execution
			std::array<uint64_t, RHI::HardwareQueueClassCount> m_waitFences{ {FenceValueNull} };

			// Fence value to signal after execution
			uint64_t m_signalFence = FenceValueNull;

			// Set of user fences to signal after execution
			std::vector<DX_Fence*> m_userFencesToSignal;
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
			void ExecuteWork(const RHI::ExecuteWorkRequest& rhiRequest) override;
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