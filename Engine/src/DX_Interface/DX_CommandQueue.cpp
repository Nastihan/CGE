
// DX12
#include "DX_CommandQueue.h"
#include "DX_Device.h"
#include "DX_Conversions.h"
#include "DX_CommandList.h"

// RHI
#include "../RHI/SwapChain.h"

namespace CGE
{
	namespace DX12
	{
		RHI::Ptr<DX_CommandQueue> DX_CommandQueue::Create()
		{
			return new DX_CommandQueue();
		}

		RHI::ResultCode DX_CommandQueue::InitInternal(RHI::Device& device, const RHI::CommandQueueDescriptor& descriptor)
		{
			DeviceObject::Init(device);
			wrl::ComPtr<ID3D12CommandQueueX> queue = nullptr;

			ID3D12DeviceX* dxDevice = static_cast<DX_Device&>(device).GetDevice();
			const CommandQueueDescriptor& commandQueueDesc = static_cast<const CommandQueueDescriptor&>(descriptor);
			DXAssertSuccess(CreateCommandQueue(dxDevice, commandQueueDesc.m_hardwareQueueClass, commandQueueDesc.m_hardwareQueueSubclass, queue.GetAddressOf()));

            const char* queueName = GetQueueName(commandQueueDesc.m_hardwareQueueClass, commandQueueDesc.m_hardwareQueueSubclass);
            assert(queueName);
            if (queueName)
            {
                std::wstring queueNameW = s2ws(queueName);
                queue->SetName(queueNameW.c_str());
                SetName(queueName);
            }
            m_queue = queue.Get();
            m_device = dxDevice;
            m_hardwareQueueSubclass = commandQueueDesc.m_hardwareQueueSubclass;

            return RHI::ResultCode::Success;
		}

        void DX_CommandQueue::ShutdownInternal()
        {
            if (m_queue)
            {
                m_queue.reset();
            }
        }

		HRESULT DX_CommandQueue::CreateCommandQueue(ID3D12DeviceX* device, RHI::HardwareQueueClass hardwareQueueClass, HardwareQueueSubclass hardwareQueueSubclass, ID3D12CommandQueueX** commandQueue)
		{
			D3D12_COMMAND_QUEUE_DESC queueDesc = {};
			queueDesc.Type = ConvertHardwareQueueClass(hardwareQueueClass);
			queueDesc.NodeMask = 0;
			return device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(commandQueue));
		}

		const char* DX_CommandQueue::GetQueueName(RHI::HardwareQueueClass hardwareQueueClass, HardwareQueueSubclass hardwareQueueSubclass)
		{
            switch (hardwareQueueClass)
            {
            case RHI::HardwareQueueClass::Copy:
                switch (hardwareQueueSubclass)
                {
                case HardwareQueueSubclass::Primary:
                    return "Copy Queue (Primary)";
                case HardwareQueueSubclass::Secondary:
                    return "Copy Queue (Secondary)";
                default:
                    return "Copy Queue";
                }

            case RHI::HardwareQueueClass::Compute:
                switch (hardwareQueueSubclass)
                {
                case HardwareQueueSubclass::Primary:
                    return "Compute Queue (Primary)";
                case HardwareQueueSubclass::Secondary:
                    return "Compute Queue (Secondary)";
                default:
                    return "Compute Queue";
                }

            case RHI::HardwareQueueClass::Graphics:
                return "Graphics Queue";

            default:
                return "Unknown Queue";
            }
		}

        void DX_CommandQueue::ExecuteWork(const RHI::ExecuteWorkRequest& rhiRequest) 
        {
            auto& dxDevice = static_cast<DX_Device&>(GetDevice());
            const DX_ExecuteWorkRequest& request = static_cast<const DX_ExecuteWorkRequest&>(rhiRequest);

            QueueCommand([=](void* commandQueue)
                {
                    ID3D12CommandQueue* dxCommandQueue = static_cast<ID3D12CommandQueue*>(commandQueue);
                    static const uint32_t CommandListCountMax = 128;

                    if (request.m_commandLists.size())
                    {
                        uint32_t executeCount = 0;
                        ID3D12CommandList* executeLists[CommandListCountMax];
                        for (uint32_t commandListIdx = 0; commandListIdx < (uint32_t)request.m_commandLists.size(); commandListIdx++)
                        {
                            DX_CommandList& commandList = *request.m_commandLists[commandListIdx];
                            executeLists[executeCount++] = commandList.GetCommandList();
                        }
                        assert(executeCount <= CommandListCountMax);
                        dxCommandQueue->ExecuteCommandLists(executeCount, executeLists);
                    }
                    for (RHI::SwapChain* swapChain : request.m_swapChainsToPresent)
                    {
                        swapChain->Present();
                    }
                });
        }

        void DX_CommandQueue::WaitForIdle()
        {
            DX_Fence fence;

            // fence value = 0 | fence m_pendingValue = 1
            fence.Init(m_device.get(), RHI::FenceState::Reset);
            QueueGpuSignal(fence);

            // [todo] doing nothing
            FlushCommands();

            DX_FenceEvent event("WaitForIdel");
            fence.Wait(event);
        }

        void DX_CommandQueue::QueueGpuSignal(DX_Fence& fence)
        {
            QueueCommand([&fence](void* commandQueue)
            {
                ID3D12CommandQueue* dx12CommandQueue = static_cast<ID3D12CommandQueue*>(commandQueue);
                dx12CommandQueue->Signal(fence.Get(), fence.GetPendingValue());
            });
        }

        ID3D12CommandQueue* DX_CommandQueue::GetPlatformQueue() const
        {
            return m_queue.get();
        }

        void* DX_CommandQueue::GetNativeQueue()
        {
            return m_queue.get();
        }
	}
}