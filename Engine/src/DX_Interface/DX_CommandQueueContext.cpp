
// DX12
#include "DX_CommandQueueContext.h"
#include "DX_Device.h"

namespace CGE
{
	namespace DX12
	{
        namespace EventTrace
        {
            const char* GpuQueueNames[] =
            {
                "Graphics Queue",
                "Compute Queue",
                "Copy Queue"
            };
        }
        // Initilizes primary Graphics, Cpmpute and Copy queue
        void DX_CommandQueueContext::Init(RHI::Device& deviceBase)
        {
            m_device = static_cast<DX_Device*>(&deviceBase);
            m_currentFrameIndex = 0;
            m_frameFences.resize(RHI::Limits::Device::FrameCountMax);
            for (DX_FenceSet& fences : m_frameFences)
            {
                fences.Init(m_device->GetDevice(), RHI::FenceState::Signaled);
            }

            m_compiledFences.Init(m_device->GetDevice(), RHI::FenceState::Reset);

            for (uint32_t hardwareQueueIdx = 0; hardwareQueueIdx < RHI::HardwareQueueClassCount; ++hardwareQueueIdx)
            {
                m_commandQueues[hardwareQueueIdx] = DX_CommandQueue::Create();

                CommandQueueDescriptor commandQueueDesc;
                commandQueueDesc.m_hardwareQueueClass = static_cast<RHI::HardwareQueueClass>(hardwareQueueIdx);
                commandQueueDesc.m_hardwareQueueSubclass = HardwareQueueSubclass::Primary;
                m_commandQueues[hardwareQueueIdx]->SetName(EventTrace::GpuQueueNames[hardwareQueueIdx]);
                m_commandQueues[hardwareQueueIdx]->Init(*m_device, commandQueueDesc);
            }
        }

        void DX_CommandQueueContext::Shutdown()
        {
            WaitForIdle();

            m_compiledFences.Shutdown();

            for (DX_FenceSet& fenceSet : m_frameFences)
            {
                fenceSet.Shutdown();
            }
            m_frameFences.clear();

            for (uint32_t hardwareQueueIdx = 0; hardwareQueueIdx < RHI::HardwareQueueClassCount; ++hardwareQueueIdx)
            {
                m_commandQueues[hardwareQueueIdx] = nullptr;
            }
        }

        DX_CommandQueue& DX_CommandQueueContext::GetCommandQueue(RHI::HardwareQueueClass hardwareQueueClass)
        {
            return *m_commandQueues[static_cast<uint32_t>(hardwareQueueClass)];
        }

        const DX_CommandQueue& DX_CommandQueueContext::GetCommandQueue(RHI::HardwareQueueClass hardwareQueueClass) const
        {
            return *m_commandQueues[static_cast<uint32_t>(hardwareQueueClass)];
        }

        void DX_CommandQueueContext::Begin() {}

        uint64_t DX_CommandQueueContext::IncrementFence(RHI::HardwareQueueClass hardwareQueueClass)
        {
            return m_compiledFences.GetFence(hardwareQueueClass).Increment();
        }

        void DX_CommandQueueContext::QueueGpuSignals(DX_FenceSet& fenceSet)
        {
            for (uint32_t hardwareQueueIdx = 0; hardwareQueueIdx < RHI::HardwareQueueClassCount; ++hardwareQueueIdx)
            {
                const RHI::HardwareQueueClass hardwareQueueClass = static_cast<RHI::HardwareQueueClass>(hardwareQueueIdx);
                DX_Fence& fence = fenceSet.GetFence(hardwareQueueClass);
                m_commandQueues[hardwareQueueIdx]->QueueGpuSignal(fence);
            }
        }

        void DX_CommandQueueContext::WaitForIdle()
        {
            for (uint32_t hardwareQueueIdx = 0; hardwareQueueIdx < RHI::HardwareQueueClassCount; ++hardwareQueueIdx)
            {
                if (m_commandQueues[hardwareQueueIdx])
                {
                    m_commandQueues[hardwareQueueIdx]->WaitForIdle();
                }
            }
        }

        // Called after recording of frame is done. Just before command queue execution.
        void DX_CommandQueueContext::End()
        {
            QueueGpuSignals(m_frameFences[m_currentFrameIndex]);

            for (uint32_t hardwareQueueIdx = 0; hardwareQueueIdx < RHI::HardwareQueueClassCount; ++hardwareQueueIdx)
            {
                m_commandQueues[hardwareQueueIdx]->FlushCommands();
            }

            // Advance to the next frame and wait for its resources to be available before continuing.
            m_currentFrameIndex = (m_currentFrameIndex + 1) % static_cast<uint32_t>(m_frameFences.size());

            {
                DX_FenceEvent event("FrameFence");
                m_frameFences[m_currentFrameIndex].Wait(event);
                m_frameFences[m_currentFrameIndex].Reset();
            }
        }
	}
}