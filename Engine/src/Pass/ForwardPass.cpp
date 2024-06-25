
// Pass
#include "ForwardPass.h"

// RHI
#include "../RHI/Graphics.h"
#include "../RHI/ShaderStages.h"

#include "../Scene/Light.h"
#include "../Scene/Material.h"

#include <iostream>

namespace CGE
{
	namespace Pass
	{
		void ForwardPass::Init(RHI::Device& device)
		{
			auto& rhiFactory = RHI::Graphics::GetFactory();

			// Init the frame buffers the pass uses.
			m_frameBuffers.m_depthBuffer = rhiFactory.CreateImage();
			RHI::ImageInitRequest initRequest;
			RHI::ClearValue clearValue = RHI::ClearValue::CreateDepthStencil(1.0, 0);
			initRequest.m_image = m_frameBuffers.m_depthBuffer.get();
			initRequest.m_descriptor = RHI::ImageDescriptor::Create2D(RHI::ImageBindFlags::DepthStencil, RHI::Limits::Device::ClientWidth, RHI::Limits::Device::ClientHeight, RHI::Format::D32_FLOAT);
			initRequest.m_optimizedClearValue = &clearValue;
			RHI::Graphics::GetImageSystem().GetSimpleImagePool()->InitImage(initRequest);

			m_frameBuffers.m_depthBufferView = rhiFactory.CreateImageView();
			RHI::ImageDescriptor imageDesc = m_frameBuffers.m_depthBuffer->GetDescriptor();
			RHI::ImageViewDescriptor imageViewDesc = RHI::ImageViewDescriptor::Create(imageDesc.m_format, 0, 0);
			m_frameBuffers.m_depthBufferView->Init(*m_frameBuffers.m_depthBuffer, imageViewDesc);

			for (size_t i = 0; i < RHI::Limits::Device::FrameCountMax; i++)
			{
				// [todo] Dont forget resize. Resize will also invalidate the views.
				m_frameBuffers.m_colorTaget[i] = rhiFactory.CreateImage();
				initRequest.m_image = m_frameBuffers.m_colorTaget[i].get();
				initRequest.m_descriptor = RHI::ImageDescriptor::Create2D(RHI::ImageBindFlags::Color, RHI::Limits::Device::ClientWidth, RHI::Limits::Device::ClientHeight, RHI::Format::R8G8B8A8_UNORM);
				RHI::Graphics::GetImageSystem().GetSimpleImagePool()->InitImage(initRequest);

				m_frameBuffers.m_colorTagetView[i] = rhiFactory.CreateImageView();
				m_frameBuffers.m_colorTagetView[i]->Init(*m_frameBuffers.m_colorTaget[i], {});
			}

			/*
			RHI::InputStreamLayout inputStreamLayoutPacked = {};
			uint32_t streamBufferIdx = 0;
			inputStreamLayoutPacked.SetTopology(RHI::PrimitiveTopology::TriangleList);
			inputStreamLayoutPacked.AddStreamChannel(RHI::StreamChannelDescriptor{ RHI::ShaderSemantic{"POSITION", 0}, RHI::Format::R32G32B32_FLOAT, 0, streamBufferIdx++ });
			inputStreamLayoutPacked.AddStreamBuffer(RHI::StreamBufferDescriptor{ RHI::StreamStepFunction::PerVertex, 1, RHI::GetFormatSize(RHI::Format::R32G32B32_FLOAT) });
			inputStreamLayoutPacked.AddStreamChannel(RHI::StreamChannelDescriptor{ RHI::ShaderSemantic{"NORMAL", 0}, RHI::Format::R32G32B32_FLOAT, 0, streamBufferIdx++ });
			inputStreamLayoutPacked.AddStreamBuffer(RHI::StreamBufferDescriptor{ RHI::StreamStepFunction::PerVertex, 1,  RHI::GetFormatSize(RHI::Format::R32G32B32_FLOAT) });
			inputStreamLayoutPacked.AddStreamChannel(RHI::StreamChannelDescriptor{ RHI::ShaderSemantic{"TANGENT", 0}, RHI::Format::R32G32B32_FLOAT, 0, streamBufferIdx++ });
			inputStreamLayoutPacked.AddStreamBuffer(RHI::StreamBufferDescriptor{ RHI::StreamStepFunction::PerVertex, 1, RHI::GetFormatSize(RHI::Format::R32G32B32_FLOAT) });
			inputStreamLayoutPacked.AddStreamChannel(RHI::StreamChannelDescriptor{ RHI::ShaderSemantic{"BINORMAL", 0}, RHI::Format::R32G32B32_FLOAT, 0, streamBufferIdx++ });
			inputStreamLayoutPacked.AddStreamBuffer(RHI::StreamBufferDescriptor{ RHI::StreamStepFunction::PerVertex, 1, RHI::GetFormatSize(RHI::Format::R32G32B32_FLOAT) });
			inputStreamLayoutPacked.AddStreamChannel(RHI::StreamChannelDescriptor{ RHI::ShaderSemantic{"TEXCOORD", 0}, RHI::Format::R32G32_FLOAT, 0, streamBufferIdx++ });
			inputStreamLayoutPacked.AddStreamBuffer(RHI::StreamBufferDescriptor{ RHI::StreamStepFunction::PerVertex, 1, RHI::GetFormatSize(RHI::Format::R32G32_FLOAT) });
			inputStreamLayoutPacked.Finalize();

			// [todo] This layout is being built from ForwardVS.hlsl ForwardPS.hlsl for now. Change later.
			RHI::Ptr<RHI::PipelineLayoutDescriptor> pipelineLayoutDescriptor = RHI::Graphics::GetFactory().CreatePipelineLayoutDescriptor();
			RHI::Ptr<RHI::ShaderResourceGroupLayout> sceneSrgLayout = RHI::ShaderResourceGroupLayout::Create();
			sceneSrgLayout->SetBindingSlot(static_cast<uint32_t>(RHI::ShaderResourceGroupType::Scene));
			RHI::ShaderInputBufferDescriptor lightBufferBindingDesc(
				"LightBuffer", RHI::ShaderInputBufferAccess::Read, RHI::ShaderInputBufferType::Structured, 1, sizeof(Scene::Light), 8, 0);
			//RHI::ShaderInputBufferDescriptor cameraBufferBindingDesc(
				//"CameraBuffer", RHI::ShaderInputBufferAccess::Constant, RHI::ShaderInputBufferType::Constant, 1, 0, 3, 0);
			sceneSrgLayout->AddShaderInput(lightBufferBindingDesc);
			//sceneSrgLayout->AddShaderInput(cameraBufferBindingDesc);
			sceneSrgLayout->SetName("FarwardPassSceneSrgLayout");
			sceneSrgLayout->Finalize();
			pipelineLayoutDescriptor->AddShaderResourceGroupLayoutInfo(*sceneSrgLayout, {});

			RHI::Ptr<RHI::ShaderResourceGroupLayout> objectSrgLayout = RHI::ShaderResourceGroupLayout::Create();
			objectSrgLayout->SetBindingSlot(static_cast<uint32_t>(RHI::ShaderResourceGroupType::Object));
			RHI::ShaderInputConstantDescriptor objectTransformCbuffDesc("PreObjectTransform", 0, sizeof(PerObject), 0, 0);
			RHI::ShaderInputImageDescriptor materialTexturesBindingDesc(
				"MaterialTextures", RHI::ShaderInputImageAccess::Read, RHI::ShaderInputImageType::Image2D, 8, 0, 0);
			RHI::ShaderInputBufferDescriptor materialBufferBindingDesc(
				"MaterialBuffer", RHI::ShaderInputBufferAccess::Constant, RHI::ShaderInputBufferType::Constant, 1, sizeof(Scene::Material::MaterialProperties), 2, 0);
			RHI::ShaderInputStaticSamplerDescriptor linearRepeatStaticSamplerDesc(
				"WrapSampler", RHI::SamplerState::Create(RHI::FilterMode::Linear, RHI::FilterMode::Linear, RHI::AddressMode::Wrap), 0, 0);
			RHI::ShaderInputStaticSamplerDescriptor linearClampStaticSamplerDesc(
				"ClampSampler", RHI::SamplerState::Create(RHI::FilterMode::Linear, RHI::FilterMode::Linear, RHI::AddressMode::Clamp), 1, 0);
			objectSrgLayout->AddShaderInput(objectTransformCbuffDesc);
			objectSrgLayout->AddShaderInput(materialTexturesBindingDesc);
			objectSrgLayout->AddShaderInput(materialBufferBindingDesc);
			objectSrgLayout->AddStaticSampler(linearRepeatStaticSamplerDesc);
			objectSrgLayout->AddStaticSampler(linearClampStaticSamplerDesc);
			objectSrgLayout->SetName("FarwardPassObjectSrgLayout");
			objectSrgLayout->Finalize();
			pipelineLayoutDescriptor->AddShaderResourceGroupLayoutInfo(*objectSrgLayout, {});
			pipelineLayoutDescriptor->Finalize("ForwardPassPipelineLayout");

			RHI::Ptr<RHI::ShaderStageFunction> vertexShader{ nullptr };
			vertexShader = rhiFactory.CreateShaderStageFunction();
			RHI::ShaderFileInfo fileInfo;
			fileInfo.m_fileName = "VertexShader.hlsl";
			fileInfo.m_function = "main";
			fileInfo.m_stage = RHI::ShaderStage::Vertex;
			vertexShader->Init(fileInfo);
			vertexShader->Finalize();

			RHI::Ptr<RHI::ShaderStageFunction> pixelShader{ nullptr };
			pixelShader = rhiFactory.CreateShaderStageFunction();
			fileInfo.m_fileName = "PixelShader.hlsl";
			fileInfo.m_function = "main";
			fileInfo.m_stage = RHI::ShaderStage::Fragment;
			pixelShader->Init(fileInfo);
			pixelShader->Finalize();

			// Default initilize RenderStates
			RHI::RenderStates renderState = {};
			*/

			RHI::RenderAttachmentConfiguration renderAttachmentConfiguration;
			renderAttachmentConfiguration.m_subpassIndex = 0;
			auto& renderAttachmentLayout = renderAttachmentConfiguration.m_renderAttachmentLayout;
			renderAttachmentLayout.m_attachmentCount = 2;
			renderAttachmentLayout.m_attachmentFormats[0] = m_frameBuffers.m_colorTaget[0]->GetDescriptor().m_format;
			renderAttachmentLayout.m_attachmentFormats[1] = m_frameBuffers.m_depthBuffer->GetDescriptor().m_format;
			renderAttachmentLayout.m_subpassLayouts[0].m_rendertargetCount = 1;
			renderAttachmentLayout.m_subpassLayouts[0].m_rendertargetDescriptors[0] = RHI::RenderAttachmentDescriptor{ 0, RHI::InvalidRenderAttachmentIndex, RHI::AttachmentLoadStoreAction() };
			renderAttachmentLayout.m_subpassLayouts[0].m_depthStencilDescriptor = RHI::RenderAttachmentDescriptor{ 1, RHI::InvalidRenderAttachmentIndex, RHI::AttachmentLoadStoreAction() };

			const std::shared_ptr<const RHI::ShaderPermutation> defaultPBRForward_MaterialShader = RHI::Graphics::GetAssetProcessor().GetShaderPermutation("DefaultPBRForward_MaterialShader");

			m_drawPipelineStateDescriptor.m_vertexFunction = defaultPBRForward_MaterialShader->m_vertexShader;
			m_drawPipelineStateDescriptor.m_fragmentFunction = defaultPBRForward_MaterialShader->m_pixelShader;
			m_drawPipelineStateDescriptor.m_pipelineLayoutDescriptor = defaultPBRForward_MaterialShader->m_pipelineLayoutDescriptor;
			m_drawPipelineStateDescriptor.m_inputStreamLayout = defaultPBRForward_MaterialShader->m_inputStreamLayout;
			m_drawPipelineStateDescriptor.m_renderStates = defaultPBRForward_MaterialShader->m_renderState;
			// The pass should configure this. ([todo] later on json passes)
			m_drawPipelineStateDescriptor.m_renderAttachmentConfiguration = renderAttachmentConfiguration;

			m_pipelineState = rhiFactory.CreatePipelineState();
			m_pipelineState->Init(device, m_drawPipelineStateDescriptor);
		}

		void ForwardPass::Render(RHI::CommandList* commandList)
		{
			m_drawItems.clear();
			m_drawItems = m_scene->BuildDrawList();

			for (size_t i = 0; i < m_drawItems.size(); i++)
			{
				m_drawItems[i].m_pipelineState = m_pipelineState.get();
				commandList->Submit(m_drawItems[i]);
			}
		}

		const RHI::PipelineStateDescriptorForDraw& ForwardPass::GetPipelineStateDescriptorForDraw()
		{
			return m_drawPipelineStateDescriptor;
		}

		const RHI::PipelineState& ForwardPass::GetPipelineState()
		{
			return *m_pipelineState;
		}

		void ForwardPass::SetScenePtr(std::shared_ptr<Scene::Scene> scene)
		{
			m_scene = scene;
		}

		RHI::ImageView& ForwardPass::GetDSView()
		{
			return *m_frameBuffers.m_depthBufferView;
		}

		std::vector<RHI::ShaderResourceGroup*>& ForwardPass::GetSrgsToBind()
		{
			return m_srgsToBind;
		}
	}
}