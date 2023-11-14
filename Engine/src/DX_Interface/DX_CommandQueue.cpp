#include "DX_CommandQueue.h"
#include "DX_Device.h"
#include "DX_Conversions.h"

namespace CGE
{
	namespace DX12
	{
		RHI::Ptr<RHI::CommandQueue> CommandQueue::Create()
		{
			return new CommandQueue();
		}

		RHI::ResultCode CommandQueue::InitInternal(RHI::Device& device, const RHI::CommandQueueDescriptor& descriptor)
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

        void CommandQueue::ShutdownInternal()
        {
            if (m_queue)
            {
                m_queue.reset();
            }
        }

		HRESULT CommandQueue::CreateCommandQueue(ID3D12DeviceX* device, RHI::HardwareQueueClass hardwareQueueClass, HardwareQueueSubclass hardwareQueueSubclass, ID3D12CommandQueueX** commandQueue)
		{
			D3D12_COMMAND_QUEUE_DESC queueDesc = {};
			queueDesc.Type = ConvertHardwareQueueClass(hardwareQueueClass);
			queueDesc.NodeMask = 0;
			return device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(commandQueue));
		}

		const char* CommandQueue::GetQueueName(RHI::HardwareQueueClass hardwareQueueClass, HardwareQueueSubclass hardwareQueueSubclass)
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

        void CommandQueue::ExecuteWork(const RHI::ExecuteWorkRequest& request) {}
        void CommandQueue::WaitForIdle() {}
	}
}