#include "DX_Commander.h"

namespace CGE
{
	namespace DX12
	{
		DX_Commander::DX_Commander(D3D12_COMMAND_LIST_TYPE type)
		{
			LOCAL_HR;
			const auto& device = DX_Device::GetInstance().GetDevice();

			// create command queue
			D3D12_COMMAND_QUEUE_DESC desc{};
			desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
			desc.NodeMask = 0;
			desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
			desc.Type = type;
			DX_THROW_FAILED(device->CreateCommandQueue(&desc, IID_PPV_ARGS(&cmdQueue)));
			NAME_D3D12_OBJECT(cmdQueue, type == D3D12_COMMAND_LIST_TYPE_DIRECT ? L"GFX Command Queue" : type == D3D12_COMMAND_LIST_TYPE_COMPUTE ? L"Compute Command Queue" : L"Command Queue");
			LOG_CONSOLE(LogLevel::Info, L"D3D12 Command Queue Created");

			// create command allocators for the number of frames in flight
			for (UINT32 i{ 0 }; i < FRAME_BUFFER_COUNT; i++)
			{
				DX_Command_Frame& frame{ cmdFrames[i] };
				DX_THROW_FAILED(device->CreateCommandAllocator(type, IID_PPV_ARGS(&frame.cmdAllocator)));
				NAME_D3D12_OBJECT_INDEXED(frame.cmdAllocator, i, type == D3D12_COMMAND_LIST_TYPE_DIRECT ? L"GFX Command List" : type == D3D12_COMMAND_LIST_TYPE_COMPUTE ? L"Compute Command List" : L"Command List");
				LOG_CONSOLE(LogLevel::Info, L"D3D12 Command Allocator " << i << L" Created");
			}

			// create command list
			DX_THROW_FAILED(device->CreateCommandList(0, type, cmdFrames[0].cmdAllocator.Get(), nullptr, IID_PPV_ARGS(&cmdList)));
			// Indicates that recording to the command list has finished
			DX_THROW_FAILED(cmdList->Close());
			NAME_D3D12_OBJECT(cmdList, type == D3D12_COMMAND_LIST_TYPE_DIRECT ? L"GFX Command List" : type == D3D12_COMMAND_LIST_TYPE_COMPUTE ? L"Compute Command List" : L"Command List");
			LOG_CONSOLE(LogLevel::Info, L"D3D12 Command List Created");

			// create the fence
			DX_THROW_FAILED(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));
			NAME_D3D12_OBJECT(fence, L"D3D12 fence");
			LOG_CONSOLE(LogLevel::Info, L"D2D12 fence created");

			// create windows event
			fenceEvent = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS);
			assert(fenceEvent);
		}

		DX_Commander::~DX_Commander()
		{
			FlushGPU();
			fenceValue = 0;
			if (fenceEvent)
			{
				CloseHandle(fenceEvent);
				fenceEvent = nullptr;
			}
		}

		void DX_Commander::Record()
		{
			LOCAL_HR;
			// get current frame
			DX_Command_Frame& frame{ cmdFrames[currentFrameIdx] };

			// before we start recording wait for the current frame to be signaled
			frame.Wait(fenceEvent, fence.Get());

			// resetting the command allocator will free memory used by previously recorded commands
			DX_THROW_FAILED(frame.cmdAllocator->Reset());

			// resetting the command list will reopen it for recording new commands
			DX_THROW_FAILED(cmdList->Reset(frame.cmdAllocator.Get(), nullptr));
		}

		void DX_Commander::Submit()
		{
			LOCAL_HR;
			// close before submiting
			DX_THROW_FAILED(cmdList->Close());
			//[todo] we are recording single threaded for now
			ID3D12CommandList* const cmdLists[]{ cmdList.Get() };
			cmdQueue->ExecuteCommandLists(_countof(cmdLists), &cmdLists[0]);

			// signal the fence with the new fence value
			fenceValue++;
			cmdFrames[currentFrameIdx].fenceValue = fenceValue;
			cmdQueue->Signal(fence.Get(), fenceValue);
			currentFrameIdx = (currentFrameIdx + 1) % FRAME_BUFFER_COUNT;
		}

		void DX_Commander::FlushGPU()
		{
			for (UINT32 i{ 0 }; i < FRAME_BUFFER_COUNT; i++)
			{
				cmdFrames[i].Wait(fenceEvent, fence.Get());
			}
			currentFrameIdx = 0;
		}

		UINT64 DX_Commander::GetCurrentFrameIdx()
		{
			return currentFrameIdx;
		}
	}
}