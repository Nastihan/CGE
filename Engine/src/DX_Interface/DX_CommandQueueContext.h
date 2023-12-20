#pragma once

// DX12
#include "DX_CommandQueue.h"
#include "DX_Fence.h"

// RHI
#include "../RHI/Device.h"

namespace CGE
{
	namespace DX12
	{
        class DX_Device;

		// The device will own an instance
		class DX_CommandQueueContext
		{
		public:
			DX_CommandQueueContext() = default;

            void Init(RHI::Device& deviceBase);
            void Shutdown();

            DX_CommandQueue& GetCommandQueue(RHI::HardwareQueueClass hardwareQueueClass);
            const DX_CommandQueue& GetCommandQueue(RHI::HardwareQueueClass hardwareQueueClass) const;

            void Begin();
            uint64_t IncrementFence(RHI::HardwareQueueClass hardwareQueueClass);
            void QueueGpuSignals(DX_FenceSet& fenceSet);
            void WaitForIdle();
            void End();
            void ExecuteWork(RHI::HardwareQueueClass hardwareQueueClass, const DX_ExecuteWorkRequest& request);
            // Fences across all queues that are compiled by the frame graph compilation phase
            const DX_FenceSet& GetCompiledFences();
            // Get frame fences for the specified frame
            const DX_FenceSet& GetFrameFences(size_t frameIndex) const;
            // Get the frame index of the last executed frame
            size_t GetLastFrameIndex() const;

        private:
            std::array<RHI::Ptr<DX_CommandQueue>, RHI::HardwareQueueClassCount> m_commandQueues;
            DX_FenceSet m_compiledFences;
            std::vector<DX_FenceSet> m_frameFences;
            uint32_t m_currentFrameIndex = 0;
            DX_Device* m_device = nullptr;
		};
	}
}