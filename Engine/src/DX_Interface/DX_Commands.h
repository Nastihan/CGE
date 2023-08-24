#pragma once
#include "DX_CommonHeaders.h"
#include "DX_Device.h"

namespace CGE
{
	class DX_Commands
	{
	private:
		struct DX_Command_Frame
		{
			ID3D12CommandAllocator* cmdAllocator{ nullptr };
			void Wait() {}
			void release() { CGE::Release(cmdAllocator); }
		};
	public:
		DX_Commands(D3D12_COMMAND_LIST_TYPE type);
		~DX_Commands();
		DX_Commands(const DX_Commands&) = delete;
		DX_Commands& operator=(const DX_Commands&) = delete;
		void Record();
		void Submit();
	private:
		ID3D12CommandQueue* cmdQueue{ nullptr };
		ID3D12GraphicsCommandList6* cmdList{ nullptr };
		static constexpr UINT32 FRAME_BUFFER_COUNT = 3;
		UINT32 currentFrameIdx{ 0 };
		DX_Command_Frame cmdFrames[FRAME_BUFFER_COUNT];
	};
}