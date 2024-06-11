
// DX12
#include "DX_PipelineState.h"
#include "DX_Device.h"
#include "DX_ShaderStageFunction.h"
#include "DX_Conversions.h"

#include <d3dx12.h>
#include <DirectXShaderCompiler/dxcapi.h>
#include <DirectXShaderCompiler/d3d12shader.h>

namespace CGE
{
	namespace DX12
	{
		RHI::Ptr<DX_PipelineState> DX_PipelineState::Create()
		{
			return new DX_PipelineState();
		}

		const DX_PipelineLayout* DX_PipelineState::GetPipelineLayout() const
		{
			return m_pipelineLayout.get();
		}

		ID3D12PipelineState* DX_PipelineState::Get() const
		{
			return m_pipelineState.get();
		}

		const PipelineStateData& DX_PipelineState::GetPipelineStateData() const
		{
			return m_pipelineStateData;
		}

        RHI::ResultCode DX_PipelineState::InitInternal(RHI::Device& deviceBase, const RHI::PipelineStateDescriptorForDraw& descriptor, RHI::PipelineLibrary* pipelineLibraryBase)
        {
            DX_Device& device = static_cast<DX_Device&>(deviceBase);

            D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc = {};
            pipelineStateDesc.NodeMask = 1;
            pipelineStateDesc.SampleMask = 0xFFFFFFFFu;
            pipelineStateDesc.SampleDesc.Count = descriptor.m_renderStates.m_multisampleState.m_samples;
            pipelineStateDesc.SampleDesc.Quality = descriptor.m_renderStates.m_multisampleState.m_quality;

            // Shader state.
            RHI::ConstPtr<DX_PipelineLayout> pipelineLayout = device.AcquirePipelineLayout(*descriptor.m_pipelineLayoutDescriptor);
            pipelineStateDesc.pRootSignature = pipelineLayout->Get();

            if (const DX_ShaderStageFunction* vertexFunction = static_cast<const DX_ShaderStageFunction*>(descriptor.m_vertexFunction.get()))
            {
                pipelineStateDesc.VS = CD3DX12_SHADER_BYTECODE(vertexFunction->GetByteCode()->GetBufferPointer(), vertexFunction->GetByteCode()->GetBufferSize());
            }

            // [todo] Tessellation stage

            if (const DX_ShaderStageFunction* fragmentFunction = static_cast<const DX_ShaderStageFunction*>(descriptor.m_fragmentFunction.get()))
            {
                pipelineStateDesc.PS = CD3DX12_SHADER_BYTECODE(fragmentFunction->GetByteCode()->GetBufferPointer(), fragmentFunction->GetByteCode()->GetBufferSize());
            }

            const RHI::RenderAttachmentConfiguration& renderAttachmentConfiguration = descriptor.m_renderAttachmentConfiguration;
            pipelineStateDesc.DSVFormat = ConvertFormat(renderAttachmentConfiguration.GetDepthStencilFormat());
            pipelineStateDesc.NumRenderTargets = renderAttachmentConfiguration.GetRenderTargetCount();
            for (uint32_t targetIdx = 0; targetIdx < pipelineStateDesc.NumRenderTargets; ++targetIdx)
            {
                pipelineStateDesc.RTVFormats[targetIdx] = ConvertFormat(renderAttachmentConfiguration.GetRenderTargetFormat(targetIdx));
            }

            std::vector<D3D12_INPUT_ELEMENT_DESC> inputElements = ConvertInputElements(descriptor.m_inputStreamLayout);
            pipelineStateDesc.InputLayout.NumElements = uint32_t(inputElements.size());
            pipelineStateDesc.InputLayout.pInputElementDescs = inputElements.data();
            pipelineStateDesc.PrimitiveTopologyType = ConvertToTopologyType(descriptor.m_inputStreamLayout.GetTopology());

            pipelineStateDesc.BlendState = ConvertBlendState(descriptor.m_renderStates.m_blendState);
            pipelineStateDesc.RasterizerState = ConvertRasterState(descriptor.m_renderStates.m_rasterState);
            pipelineStateDesc.DepthStencilState = ConvertDepthStencilState(descriptor.m_renderStates.m_depthStencilState);

            RHI::Ptr<ID3D12PipelineState> pipelineState;
            Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineStateComPtr;
            DXAssertSuccess(device.GetDevice()->CreateGraphicsPipelineState(&pipelineStateDesc, IID_PPV_ARGS(pipelineStateComPtr.GetAddressOf())));
            pipelineState = pipelineStateComPtr.Get();

            if (pipelineState)
            {
                m_pipelineLayout = std::move(pipelineLayout);
                m_pipelineState = std::move(pipelineState);
                m_pipelineStateData.m_type = RHI::PipelineStateType::Draw;
                m_pipelineStateData.m_drawData = PipelineStateDrawData{ descriptor.m_renderStates.m_multisampleState, descriptor.m_inputStreamLayout.GetTopology() };
                return RHI::ResultCode::Success;
            }
            else
            {
                assert(false, "Failed to compile graphics pipeline state. Check the D3D12 debug layer for more info.");
                return RHI::ResultCode::Fail;
            }
        }

		void DX_PipelineState::ShutdownInternal()
		{
			// ray tracing shaders do not have a traditional pipeline state object
			if (m_pipelineStateData.m_type != RHI::PipelineStateType::RayTracing)
			{
				static_cast<DX_Device&>(GetDevice()).QueueForRelease(std::move(m_pipelineState));
			}
			m_pipelineState = nullptr;
			m_pipelineLayout = nullptr;
		}
	}
}