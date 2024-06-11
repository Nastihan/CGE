#pragma once

// DX12
#include "DX_CommandListBase.h"
#include "DX_PipelineLayout.h"
#include "DX_ShaderResourceGroup.h"
#include "DX_PipelineState.h"

// RHI
#include "../RHI/CommandList.h"
#include "../RHI/ClearValue.h"
#include "../RHI/PipelineStateDescriptor.h"
#include "../RHI/PipelineState.h"
#include "../RHI/Viewport.h"
#include "../RHI/Scissor.h"
#include "../RHI/CommandListStates.h"

namespace CGE
{
	namespace DX12
	{
		class DX_CommandList : public RHI::CommandList, public DX_CommandListBase
		{
		public:
			struct ImageClearRequest
			{
				RHI::ClearValue m_clearValue;
				D3D12_CLEAR_FLAGS m_clearFlags = (D3D12_CLEAR_FLAGS)0;
				// [todo] Replace with imageview
				DX_DescriptorHandle handle;
			};
		public:
			static RHI::Ptr<DX_CommandList> Create();
			bool IsInitilized() const;
			void Init(DX_Device& device, RHI::HardwareQueueClass hardwareQueueClass, ID3D12CommandAllocator* commandAllocator);
			void Shutdown() override;

			void Close() override;
			void ClearRenderTarget(const ImageClearRequest& request);

			// DX12::CommandListBase
			void Reset(ID3D12CommandAllocator* commandAllocator) override;

			// RHI::CommandList
			void SetViewports(const RHI::Viewport* viewports, uint32_t count) override;
			void SetScissors(const RHI::Scissor* scissors, uint32_t count) override;
			void Submit(const RHI::DrawItem& drawItem) override;

		private:
			DX_CommandList() = default;

			template <RHI::PipelineStateType>
			void SetShaderResourceGroup(const DX_ShaderResourceGroup* shaderResourceGroup);

			template <RHI::PipelineStateType, typename Item>
			bool CommitShaderResources(const Item& item);

			void SetStreamBuffers(const RHI::StreamBufferView* descriptors, uint32_t count);
			void SetIndexBuffer(const RHI::IndexBufferView& descriptor);
			void SetTopology(RHI::PrimitiveTopology topology);
			void SetStencilRef(uint8_t stencilRef);
			void CommitViewportState();
			void CommitScissorState();

		private:
			struct ShaderResourceBindings
			{
				const DX_PipelineLayout* m_pipelineLayout = nullptr;
				std::array<const DX_ShaderResourceGroup*, RHI::Limits::Pipeline::ShaderResourceGroupCountMax> m_srgsByIndex;
				std::array<const DX_ShaderResourceGroup*, RHI::Limits::Pipeline::ShaderResourceGroupCountMax> m_srgsBySlot;
				// [todo] check not using it for now.
				bool m_hasRootConstants = false;
			};
			ShaderResourceBindings& GetShaderResourceBindingsByPipelineType(RHI::PipelineStateType pipelineType);

			// The command list must keep track of the states set on it so it knows what changes and needs updates in between GPU commands.
			struct State
			{
				const RHI::PipelineState* m_pipelineState = nullptr;
				std::array<uint64_t, RHI::Limits::Pipeline::StreamCountMax> m_streamBufferHashes = { {} };
				uint64_t m_indexBufferHash = 0;
				uint32_t m_stencilRef = static_cast<uint32_t>(-1);
				RHI::PrimitiveTopology m_topology = RHI::PrimitiveTopology::Undefined;
				RHI::CommandListViewportState m_viewportState;
				RHI::CommandListScissorState m_scissorState;
				std::array<ShaderResourceBindings, static_cast<size_t>(RHI::PipelineStateType::Count)> m_bindingsByPipe;
				DX_CommandQueue* m_parentQueue = nullptr;

				// This will signal that the global bindless heap is bound. We dont need to bind it for the next item.
				bool m_bindBindlessHeap = false;
			} m_state;

		private:
			DX_DescriptorContext* m_descriptorContext;
		};

		template <RHI::PipelineStateType pipelineType>
		void DX_CommandList::SetShaderResourceGroup(const DX_ShaderResourceGroup* shaderResourceGroup)
		{
			if (!shaderResourceGroup)
			{
				assert(false, "ShaderResourceGroup assigned to draw item is null.");
				return;
			}
			const uint32_t bindingSlot = shaderResourceGroup->GetBindingSlot();
			GetShaderResourceBindingsByPipelineType(pipelineType).m_srgsBySlot[bindingSlot] = shaderResourceGroup;
		}

		template <RHI::PipelineStateType pipelineType, typename Item>
		bool DX_CommandList::CommitShaderResources(const Item& item)
		{
			// Get the current bindings on the command list
			ShaderResourceBindings& bindings = GetShaderResourceBindingsByPipelineType(pipelineType);

			// Get the pipeline state of the current item.
			const DX_PipelineState* pipelineState = static_cast<const DX_PipelineState*>(item.m_pipelineState);
			if (!pipelineState)
			{
				assert(false, "No pipeline state for the item.");
				return false;
			}

			// Get the pipeline layout (abstraction over the root signature) of the current item.
			const DX_PipelineLayout* pipelineLayout = pipelineState->GetPipelineLayout();
			if (!pipelineLayout)
			{
				assert(false, "No pipeline layout for the current item.");
				return false;
			}

			// Check to see if we need to change the pipeline state on the command list first.
			bool updatePipelineState = m_state.m_pipelineState != pipelineState;
			if (updatePipelineState)
			{
				if (!pipelineState->IsInitialized())
				{
					assert(false, "New pipeline state is not initilized.");
					return false;
				}
				GetCommandList()->SetPipelineState(pipelineState->Get());

				if constexpr (pipelineType == RHI::PipelineStateType::Draw)
				{
					const auto& pipelineData = pipelineState->GetPipelineStateData();
					// [todo] Check if we need to set custom sample positions
					SetTopology(pipelineData.m_drawData.m_primitiveTopology);
				}

				// When the pipeline state changes usually the pipeline layout does to.
				// We will check if the pipeline layout (root signature) changed, if so all the shader bindings are invalidated and have to be set again.
				if (bindings.m_pipelineLayout != pipelineLayout)
				{
					switch (pipelineType)
					{
					case RHI::PipelineStateType::Draw:
						GetCommandList()->SetGraphicsRootSignature(pipelineLayout->Get());
						break;

					case RHI::PipelineStateType::Dispatch:
						GetCommandList()->SetComputeRootSignature(pipelineLayout->Get());
						break;

					default:
						assert(false, "Invalid PipelineType");
						return false;
					}

					// Set the new pipeline layout.
					bindings.m_pipelineLayout = pipelineLayout;
					// We need to zero these out, since the command list root parameters are invalid.
					for (size_t i = 0; i < bindings.m_srgsByIndex.size(); ++i)
					{
						bindings.m_srgsByIndex[i] = nullptr;
					}
				}
				// Set the new pipeline state pointer.
				m_state.m_pipelineState = pipelineState;
			}

			// Assign shader resource groups from the item to slot bindings.
			for (uint32_t srgIndex = 0; srgIndex < item.m_shaderResourceGroupCount; ++srgIndex)
			{
				SetShaderResourceGroup<pipelineType>(static_cast<const DX_ShaderResourceGroup*>(item.m_shaderResourceGroups[srgIndex]));
			}

			// Now pull the binding for each srg and rebind anything that has changed.
			for (size_t srgIndex = 0; srgIndex < pipelineLayout->GetRootParameterBindingCount(); srgIndex++)
			{
				const size_t srgSlot = pipelineLayout->GetSlotByIndex(srgIndex);
				const DX_ShaderResourceGroup* shaderResourceGroup = bindings.m_srgsBySlot[srgSlot];
				DX_RootParameterBinding binding = pipelineLayout->GetRootParameterBindingByIndex(srgIndex);

				//Check if we are iterating over the bindless srg slot
				const auto& device = static_cast<DX_Device&>(GetDevice());
				if (srgSlot == device.GetBindlessSrgSlot() && shaderResourceGroup == nullptr)
				{
					// Skip in case the global static heap is already bound
					if (m_state.m_bindBindlessHeap)
					{
						continue;
					}
					assert(binding.m_bindlessTable.IsValid(), "BindlessSRG handles is not valid.");

					switch (pipelineType)
					{
					case RHI::PipelineStateType::Draw:
					{
						GetCommandList()->SetGraphicsRootDescriptorTable(binding.m_bindlessTable.GetIndex(), m_descriptorContext->GetBindlessGpuPlatformHandle());
						break;
					}
					case RHI::PipelineStateType::Dispatch:
					{
						GetCommandList()->SetComputeRootDescriptorTable(binding.m_bindlessTable.GetIndex(), m_descriptorContext->GetBindlessGpuPlatformHandle());
						break;
					}
					default:
						assert(false, "Invalid PipelineType");
						break;
					}
					m_state.m_bindBindlessHeap = true;
					continue;
				}

				bool updateSRG = bindings.m_srgsByIndex[srgIndex] != shaderResourceGroup;
				if (updateSRG)
				{
					bindings.m_srgsByIndex[srgIndex] = shaderResourceGroup;
					const DX_ShaderResourceGroupCompiledData& compiledData = shaderResourceGroup->GetCompiledData();
					switch (pipelineType)
					{
					case RHI::PipelineStateType::Draw:
						if (binding.m_rootConstant.IsValid() && compiledData.m_rootConstant)
						{
							assert((compiledData.m_rootConstantSize % 4) == 0, "Invalid inline constant data size. It must be a multiple of 32 bit.");
							switch (pipelineType)
							{
							case RHI::PipelineStateType::Draw:
								GetCommandList()->SetGraphicsRoot32BitConstants(binding.m_rootConstant.GetIndex(), compiledData.m_rootConstantSize / 4, compiledData.m_rootConstant, 0);
								break;

							case RHI::PipelineStateType::Dispatch:
								GetCommandList()->SetComputeRoot32BitConstants(binding.m_rootConstant.GetIndex(), compiledData.m_rootConstantSize / 4, compiledData.m_rootConstant, 0);
								break;

							default:
								assert(false, "Invalid PipelineType");
								return false;
							}
						}
						if (binding.m_resourceTable.IsValid() && compiledData.m_gpuViewsDescriptorHandle.ptr)
						{
							GetCommandList()->SetGraphicsRootDescriptorTable(binding.m_resourceTable.GetIndex(), compiledData.m_gpuViewsDescriptorHandle);
						}
						if (binding.m_constantBuffer.IsValid())
						{
							GetCommandList()->SetGraphicsRootConstantBufferView(binding.m_constantBuffer.GetIndex(), compiledData.m_gpuConstantBufferAddress);
						}
						if (binding.m_samplerTable.IsValid() && compiledData.m_gpuSamplersDescriptorHandle.ptr)
						{
							GetCommandList()->SetGraphicsRootDescriptorTable(binding.m_samplerTable.GetIndex(), compiledData.m_gpuSamplersDescriptorHandle);
						}
						break;

					case RHI::PipelineStateType::Dispatch:
						if (binding.m_resourceTable.IsValid() && compiledData.m_gpuViewsDescriptorHandle.ptr)
						{
							GetCommandList()->SetComputeRootDescriptorTable(binding.m_resourceTable.GetIndex(), compiledData.m_gpuViewsDescriptorHandle);
						}
						if (binding.m_constantBuffer.IsValid())
						{
							GetCommandList()->SetComputeRootConstantBufferView(binding.m_constantBuffer.GetIndex(), compiledData.m_gpuConstantBufferAddress);
						}
						if (binding.m_samplerTable.IsValid() && compiledData.m_gpuSamplersDescriptorHandle.ptr)
						{
							GetCommandList()->SetComputeRootDescriptorTable(binding.m_samplerTable.GetIndex(), compiledData.m_gpuSamplersDescriptorHandle);
						}
						break;

					default:
						assert(false, "Invalid PipelineType");
						return false;
					}
				}
			}
		}
	}
}