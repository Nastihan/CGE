
// DX12
#include "DX_Device.h"
#include "DX_FrameGraphExecuter.h"
#include "DX_SwapChain.h"
#include "DX_CommandList.h"
#include "DX_ImguiManager.h"
#include "DX_ImageView.h"
#include "DX_BufferPoolResolver.h"
#include "DX_ImagePoolResolver.h"

#include "../RHI/Graphics.h"

#include "../imgui/imgui_impl_dx12.h"
#include "../imgui/imgui_impl_glfw.h"

#include "d3dx12.h"

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

			// imgui frame begin
			RHI::Graphics::GetImguiManager().FrameBegin();

			auto& dxDevice = static_cast<DX_Device&>(GetDevice());
			DX_CommandList* commandList = dxDevice.AcquireCommandList(RHI::HardwareQueueClass::Graphics);
			ID3D12GraphicsCommandList* dxCommandList = commandList->GetCommandList();

			DX_SwapChain& dxSwapChain = static_cast<DX_SwapChain&>(*GetDevice().GetSwapChain());
			ID3D12Resource* swapChainBackBuffer = dxSwapChain.GetBackBuffer();

			D3D12_CPU_DESCRIPTOR_HANDLE renderTargetView = dxDevice.GetDescriptorContext().GetCpuPlatformHandle(dxSwapChain.GetBackBufferDescriptorHandle());
			
			const auto& dsv = static_cast<DX12::DX_ImageView&>(m_forwardPass->GetDSView()).GetDepthStencilDescriptor(RHI::ScopeAttachmentAccess::ReadWrite);
			D3D12_CPU_DESCRIPTOR_HANDLE dsvCpuHandle = dxDevice.GetDescriptorContext().GetCpuPlatformHandle(dsv);

			auto& bufferSystem = RHI::Graphics::GetBufferSystem();
			const auto& inputAssemblyBufferPool = bufferSystem.GetCommonBufferPool(RHI::CommonBufferPoolType::StaticInputAssembly);
			const auto& constantBufferPool = bufferSystem.GetCommonBufferPool(RHI::CommonBufferPoolType::Constant);
			const auto& readOnlyBufferPool = bufferSystem.GetCommonBufferPool(RHI::CommonBufferPoolType::ReadOnly);
			static_cast<DX_BufferPoolResolver*>(inputAssemblyBufferPool->GetResolver())->Compile();
			static_cast<DX_BufferPoolResolver*>(inputAssemblyBufferPool->GetResolver())->Resolve(*commandList);
			static_cast<DX_BufferPoolResolver*>(inputAssemblyBufferPool->GetResolver())->Deactivate();
			static_cast<DX_BufferPoolResolver*>(constantBufferPool->GetResolver())->Compile();
			static_cast<DX_BufferPoolResolver*>(constantBufferPool->GetResolver())->Resolve(*commandList);
			static_cast<DX_BufferPoolResolver*>(constantBufferPool->GetResolver())->Deactivate();
			static_cast<DX_BufferPoolResolver*>(constantBufferPool->GetResolver())->Compile();
			static_cast<DX_BufferPoolResolver*>(readOnlyBufferPool->GetResolver())->Resolve(*commandList);
			static_cast<DX_BufferPoolResolver*>(constantBufferPool->GetResolver())->Deactivate();

			auto& imageSystem = RHI::Graphics::GetImageSystem();
			static_cast<DX_ImagePoolResolver*>(imageSystem.GetSimpleImagePool()->GetResolver())->Resolve(*commandList);
			static_cast<DX_ImagePoolResolver*>(imageSystem.GetSimpleImagePool()->GetResolver())->Deactivate();


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
				dxCommandList->ClearDepthStencilView(dsvCpuHandle, D3D12_CLEAR_FLAG_DEPTH, 1, 0, 0, nullptr);
			}

			// Set FrameBuffers
			{
				CD3DX12_RECT scissorRect;
				CD3DX12_VIEWPORT viewport;
				scissorRect = CD3DX12_RECT{ 0, 0, LONG_MAX, LONG_MAX };
				viewport = CD3DX12_VIEWPORT{ 0.0f, 0.0f, float(RHI::Limits::Device::ClientWidth), float(RHI::Limits::Device::ClientHeight) };
				dxCommandList->RSSetViewports(1, &viewport);
				dxCommandList->RSSetScissorRects(1, &scissorRect);
				dxCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				dxCommandList->OMSetRenderTargets(1, &renderTargetView, TRUE, &dsvCpuHandle);
			}

			// Forward Pass
			{
				m_forwardPass->Render(commandList);
			}

			// Render ImGui
			{
				static_cast<DX_ImguiManager&>(RHI::Graphics::GetImguiManager()).Render(dxCommandList);
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