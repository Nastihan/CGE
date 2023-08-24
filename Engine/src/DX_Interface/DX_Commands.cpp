#include "DX_Commands.h"

namespace CGE
{
	DX_Commands::DX_Commands(D3D12_COMMAND_LIST_TYPE type)
	{
		const auto& device = DX_Device::GetInstance().GetDevice();

		// create command queue
		D3D12_COMMAND_QUEUE_DESC desc{};
		desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		desc.NodeMask = 0;
		desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		desc.Type = type;
		ThrowIfFailed(device->CreateCommandQueue(&desc, IID_PPV_ARGS(&cmdQueue)));
		NAME_D3D12_OBJECT(cmdQueue, type == D3D12_COMMAND_LIST_TYPE_DIRECT ? L"GFX Command Queue" : type == D3D12_COMMAND_LIST_TYPE_COMPUTE ? L"Compute Command Queue" : L"Command Queue");
		LOG_CONSOLE(LogLevel::Info, L"D3D12 Command Queue Created");

		// create command allocators for the number of frames in flight
		for (UINT32 i{ 0 }; i < FRAME_BUFFER_COUNT; i++)
		{
			DX_Command_Frame& frame{ cmdFrames[i] };
			ThrowIfFailed(device->CreateCommandAllocator(type, IID_PPV_ARGS(&frame.cmdAllocator)));
			NAME_D3D12_OBJECT_INDEXED(frame.cmdAllocator, i, type == D3D12_COMMAND_LIST_TYPE_DIRECT ? L"GFX Command List" : type == D3D12_COMMAND_LIST_TYPE_COMPUTE ? L"Compute Command List" : L"Command List");
			LOG_CONSOLE(LogLevel::Info, L"D3D12 Command Allocator " << i << L"Created");
		}

		// create command list
		ThrowIfFailed(device->CreateCommandList(0, type, cmdFrames[0].cmdAllocator, nullptr, IID_PPV_ARGS(&cmdList)));
		// Indicates that recording to the command list has finished
		ThrowIfFailed(cmdList->Close());
		NAME_D3D12_OBJECT(cmdList, type == D3D12_COMMAND_LIST_TYPE_DIRECT ? L"GFX Command List" : type == D3D12_COMMAND_LIST_TYPE_COMPUTE ? L"Compute Command List" : L"Command List");
		LOG_CONSOLE(LogLevel::Info, L"D3D12 Command List Created");
	}

	void DX_Commands::Record()
	{
		// get current frame commander
		DX_Command_Frame& frame{ cmdFrames[currentFrameIdx] };

		// before we start recording wait for the fence value
		frame.Wait();

		// resetting the command allocator will free memory used by previously recorded commands
		ThrowIfFailed(frame.cmdAllocator->Reset());

		// resetting the command list will reopen it for recording new commands
		ThrowIfFailed(cmdList->Reset(frame.cmdAllocator, nullptr));
	}

	void DX_Commands::Submit()
	{
		// close before submiting
		ThrowIfFailed(cmdList->Close());
		//[todo] we are recording single threaded for now
		ID3D12CommandList* const cmdLists[]{ cmdList };
		cmdQueue->ExecuteCommandLists(_countof(cmdLists), &cmdLists[0]);

		currentFrameIdx = (currentFrameIdx + 1) % FRAME_BUFFER_COUNT;
	}
}