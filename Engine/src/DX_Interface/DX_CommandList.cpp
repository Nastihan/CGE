
// DX12
#include "DX_CommandList.h"
#include "DX_Buffer.h"
#include "DX_Conversions.h"

namespace CGE
{
	namespace DX12
	{
		RHI::Ptr<DX_CommandList> DX_CommandList::Create()
		{
			return new DX_CommandList();
		}

		bool DX_CommandList::IsInitilized() const
		{
			return DX_CommandListBase::IsInitialized();
		}

		void DX_CommandList::Init(DX_Device& device, RHI::HardwareQueueClass hardwareQueueClass, ID3D12CommandAllocator* commandAllocator)
		{
			DX_CommandListBase::Init(device, hardwareQueueClass, commandAllocator);
			m_descriptorContext = &device.GetDescriptorContext();
			if (GetHardwareQueueClass() != RHI::HardwareQueueClass::Copy)
			{
				m_descriptorContext->SetDescriptorHeaps(GetCommandList());
			}
		}

		void DX_CommandList::Shutdown() {}

		void DX_CommandList::Close()
		{
			DX_CommandListBase::Close();
		}

		void DX_CommandList::ClearRenderTarget(const ImageClearRequest& request)
		{
			D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle = m_descriptorContext->GetCpuPlatformHandle(request.handle);
			GetCommandList()->ClearRenderTargetView(descriptorHandle, request.m_clearValue.m_vector4Float.data(), 0, nullptr);
		}

		void DX_CommandList::Reset(ID3D12CommandAllocator* commandAllocator)
		{
			DX_CommandListBase::Reset(commandAllocator);

			if (GetHardwareQueueClass() != RHI::HardwareQueueClass::Copy)
			{
				m_descriptorContext->SetDescriptorHeaps(GetCommandList());
			}

			// Clear state back to empty.
			m_state = State();
		}

		void DX_CommandList::SetStreamBuffers(const RHI::StreamBufferView* streams, uint32_t count)
		{
			bool needsBinding = false;
			for (uint32_t i = 0; i < count; ++i)
			{
				if (m_state.m_streamBufferHashes[i] != static_cast<uint64_t>(streams[i].GetHash()))
				{
					m_state.m_streamBufferHashes[i] = static_cast<uint64_t>(streams[i].GetHash());
					needsBinding = true;
				}
			}

			if (needsBinding)
			{
				D3D12_VERTEX_BUFFER_VIEW views[RHI::Limits::Pipeline::StreamCountMax];
				for (uint32_t i = 0; i < count; ++i)
				{
					if (streams[i].GetBuffer())
					{
						const DX_Buffer* buffer = static_cast<const DX_Buffer*>(streams[i].GetBuffer());
						views[i].BufferLocation = buffer->GetMemoryView().GetGpuAddress() + streams[i].GetByteOffset();
						views[i].SizeInBytes = streams[i].GetByteCount();
						views[i].StrideInBytes = streams[i].GetByteStride();
					}
					else
					{
						views[i] = {};
					}
				}
				GetCommandList()->IASetVertexBuffers(0, count, views);
			}
		}

		void DX_CommandList::SetIndexBuffer(const RHI::IndexBufferView& indexBufferView)
		{
			uint64_t indexBufferHash = static_cast<uint64_t>(indexBufferView.GetHash());
			if (indexBufferHash != m_state.m_indexBufferHash)
			{
				m_state.m_indexBufferHash = indexBufferHash;
				if (const DX_Buffer* indexBuffer = static_cast<const DX_Buffer*>(indexBufferView.GetBuffer()))
				{
					D3D12_INDEX_BUFFER_VIEW view;
					view.BufferLocation = indexBuffer->GetMemoryView().GetGpuAddress() + indexBufferView.GetByteOffset();
					view.Format = (indexBufferView.GetIndexFormat() == RHI::IndexFormat::Uint16) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
					view.SizeInBytes = indexBufferView.GetByteCount();
					GetCommandList()->IASetIndexBuffer(&view);
				}
			}
		}

		void DX_CommandList::SetTopology(RHI::PrimitiveTopology topology)
		{
			if (m_state.m_topology != topology)
			{
				GetCommandList()->IASetPrimitiveTopology(ConvertTopology(topology));
				m_state.m_topology = topology;
			}
		}

		void DX_CommandList::SetStencilRef(uint8_t stencilRef)
		{
			if (m_state.m_stencilRef != stencilRef)
			{
				GetCommandList()->OMSetStencilRef(stencilRef);
				m_state.m_stencilRef = stencilRef;
			}
		}

		void DX_CommandList::CommitViewportState()
		{
			if (!m_state.m_viewportState.m_isDirty)
			{
				return;
			}
			D3D12_VIEWPORT dx12Viewports[D3D12_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
			const auto& viewports = m_state.m_viewportState.m_states;
			for (uint32_t i = 0; i < viewports.size(); ++i)
			{
				dx12Viewports[i].TopLeftX = viewports[i].m_minX;
				dx12Viewports[i].TopLeftY = viewports[i].m_minY;
				dx12Viewports[i].Width = viewports[i].m_maxX - viewports[i].m_minX;
				dx12Viewports[i].Height = viewports[i].m_maxY - viewports[i].m_minY;
				dx12Viewports[i].MinDepth = viewports[i].m_minZ;
				dx12Viewports[i].MaxDepth = viewports[i].m_maxZ;
			}
			GetCommandList()->RSSetViewports(viewports.size(), dx12Viewports);
			m_state.m_viewportState.m_isDirty = false;
		}

		void DX_CommandList::CommitScissorState()
		{
			if (!m_state.m_scissorState.m_isDirty)
			{
				return;
			}
			D3D12_RECT dx12Scissors[D3D12_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
			const auto& scissors = m_state.m_scissorState.m_states;
			for (uint32_t i = 0; i < scissors.size(); ++i)
			{
				dx12Scissors[i].left = scissors[i].m_minX;
				dx12Scissors[i].top = scissors[i].m_minY;
				dx12Scissors[i].right = scissors[i].m_maxX;
				dx12Scissors[i].bottom = scissors[i].m_maxY;
			}
			GetCommandList()->RSSetScissorRects(scissors.size(), dx12Scissors);
			m_state.m_scissorState.m_isDirty = false;
		}

		void DX_CommandList::SetViewports(const RHI::Viewport* viewports, uint32_t count)
		{
			m_state.m_viewportState.Set(std::span<const RHI::Viewport>(viewports, count));
		}

		void DX_CommandList::SetScissors(const RHI::Scissor* scissors, uint32_t count)
		{
			m_state.m_scissorState.Set(std::span<const RHI::Scissor>(scissors, count));
		}

		void DX_CommandList::Submit(const RHI::DrawItem& drawItem)
		{
			if (!CommitShaderResources<RHI::PipelineStateType::Draw>(drawItem))
			{
				assert(false, "Failed to bind shader resources for draw item.");
				return;
			}
			SetStreamBuffers(drawItem.m_streamBufferViews, drawItem.m_streamBufferViewCount);
			SetStencilRef(drawItem.m_stencilRef);

			RHI::CommandListScissorState scissorState;
			if (drawItem.m_scissorsCount)
			{
				scissorState = m_state.m_scissorState;
				SetScissors(drawItem.m_scissors, drawItem.m_scissorsCount);
			}
			RHI::CommandListViewportState viewportState;
			if (drawItem.m_viewportsCount)
			{
				viewportState = m_state.m_viewportState;
				SetViewports(drawItem.m_viewports, drawItem.m_viewportsCount);
			}
			CommitScissorState();
			CommitViewportState();
			// [todo] ShadingRateState

			switch (drawItem.m_arguments.m_type)
			{
			case RHI::DrawType::Indexed:
			{
				assert(drawItem.m_indexBufferView, "Index buffer view is null!");
				const RHI::DrawIndexed& indexed = drawItem.m_arguments.m_indexed;
				SetIndexBuffer(*drawItem.m_indexBufferView);
				GetCommandList()->DrawIndexedInstanced(indexed.m_indexCount, indexed.m_instanceCount, indexed.m_indexOffset, indexed.m_vertexOffset, indexed.m_instanceOffset);
				break;
			}

			case RHI::DrawType::Linear:
			{
				const RHI::DrawLinear& linear = drawItem.m_arguments.m_linear;
				GetCommandList()->DrawInstanced(linear.m_vertexCount, linear.m_instanceCount, linear.m_vertexOffset, linear.m_instanceOffset);
				break;
			}

			// [todo] case RHI::DrawType::Indirect
			}

			// Restore the scissors and viewports if needed.
			if (scissorState.IsValid())
			{
				SetScissors(scissorState.m_states.data(), scissorState.m_states.size());
			}
			if (viewportState.IsValid())
			{
				SetViewports(viewportState.m_states.data(), viewportState.m_states.size());
			}
		}

		DX_CommandList::ShaderResourceBindings& DX_CommandList::GetShaderResourceBindingsByPipelineType(RHI::PipelineStateType pipelineType)
		{
			return m_state.m_bindingsByPipe[static_cast<size_t>(pipelineType)];
		}
	}
}