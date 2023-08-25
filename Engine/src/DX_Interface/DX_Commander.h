#pragma once
#include "DX_CommonHeaders.h"
#include "DX_Device.h"

namespace CGE
{
	class DX_Commander
	{
	private:
		struct DX_Command_Frame
		{
			ID3D12CommandAllocator* cmdAllocator{ nullptr };
			UINT64 fenceValue{ 0 };

			void Wait(HANDLE fenceEvent, ID3D12Fence1* fence)
			{
				assert(fence && fenceEvent);
				if (fence->GetCompletedValue() < fenceValue)
				{
					ThrowIfFailed(fence->SetEventOnCompletion(fenceValue, fenceEvent));
					WaitForSingleObject(fenceEvent, INFINITE);
				}
			}
			void release(UINT32 logIdx)
			{
				CGE::Release(cmdAllocator);
				LOG_CONSOLE(LogLevel::Info, "Released Command Allocator " << logIdx);
			}
		};
	public:
		// [todo] we need to pass it a device
		DX_Commander(D3D12_COMMAND_LIST_TYPE type);
		~DX_Commander();
		DX_Commander(const DX_Commander&) = delete;
		DX_Commander(DX_Commander&&) = delete;
		DX_Commander& operator=(const DX_Commander&) = delete;
		DX_Commander& operator=(DX_Commander&&) = delete;
	public:
		void Record();
		void Submit();
		UINT64 GetCurrentFrameIdx();
	private:
		void FlushGPU();
	private:
		ID3D12CommandQueue* cmdQueue{ nullptr };
		ID3D12GraphicsCommandList6* cmdList{ nullptr };
		ID3D12Fence1* fence{ nullptr };
		UINT64 fenceValue{ 0 };
		HANDLE fenceEvent{ nullptr };
		static constexpr UINT32 FRAME_BUFFER_COUNT = 3;
		UINT32 currentFrameIdx{ 0 };
		DX_Command_Frame cmdFrames[FRAME_BUFFER_COUNT];
	};
}