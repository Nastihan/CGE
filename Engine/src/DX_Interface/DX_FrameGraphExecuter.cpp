
// DX12
#include "DX_Device.h"
#include "DX_CommandList.h"
#include "DX_FrameGraphExecuter.h"
#include "DX_SwapChain.h"

namespace CGE
{
	namespace DX12
	{
		RHI::Ptr<DX_FrameGraphExecuter> DX_FrameGraphExecuter::Create()
		{
			return new DX_FrameGraphExecuter();
		}

		RHI::ResultCode DX_FrameGraphExecuter::InitInternal(RHI::Device& device)
		{
			return RHI::ResultCode::Success;
		}

		void DX_FrameGraphExecuter::RenderFrameInternal()
		{
			/*
			*  1. Get the current frames Command Allocator and command list and reset them
			*  2. Get the current backbuffer descriptor
			*  3. record the commands
			*  4. push command list for execution
			*  5. Get next frame index and wait for resources to be avalible
			*/

			auto& dxDevice = static_cast<DX_Device&>(GetDevice());
			DX_CommandList* commandList = dxDevice.AcquireCommandList(RHI::HardwareQueueClass::Graphics);
			ID3D12GraphicsCommandList* dxCommandList = commandList->GetCommandList();

			DX_SwapChain& dxSwapChain = static_cast<DX_SwapChain&>(*GetDevice().GetSwapChain());
			ID3D12Resource* swapChainBackBuffer = dxSwapChain.GetBackBuffer();

			D3D12_CPU_DESCRIPTOR_HANDLE renderTargetView = dxDevice.GetDescriptorContext().GetCpuPlatformHandle(dxSwapChain.GetBackBufferDescriptorHandle());

			// Clear the render target
			{
				D3D12_RESOURCE_BARRIER barrier = {};
				barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
				barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
				barrier.Transition.pResource = swapChainBackBuffer;
				barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
				barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
				barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
				dxCommandList->ResourceBarrier(1, &barrier);

				FLOAT clearColor[] = { 0.4f, 0.6f, 0.9f, 1.0f };
				dxCommandList->ClearRenderTargetView(renderTargetView, clearColor, 0, nullptr);
			}

			// Present
			{
				D3D12_RESOURCE_BARRIER barrier = {};
				barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
				barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
				barrier.Transition.pResource = swapChainBackBuffer;
				barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
				barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
				barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
				dxCommandList->ResourceBarrier(1, &barrier);
			}

			commandList->Close();

			DX_ExecuteWorkRequest workRequest;
			workRequest.m_commandLists.push_back(commandList);
			workRequest.m_swapChainsToPresent.push_back(GetDevice().GetSwapChain());

			DX_CommandQueueContext& commandQueueContext = dxDevice.GetCommandQueueContext();
			commandQueueContext.ExecuteWork(RHI::HardwareQueueClass::Graphics, workRequest);
		}
	}
}