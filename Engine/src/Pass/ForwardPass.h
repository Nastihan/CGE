#pragma once

// RHI
#include "../RHI/PipelineLayoutDescriptor.h"
#include "../RHI/PipelineStateDescriptor.h"
#include "../RHI/DrawItem.h"
#include "../RHI/CommandList.h"
#include "../RHI/Image.h"
#include "../RHI/PipelineState.h"

#include "../Scene/Scene.h"

#include <glm/glm.hpp>

namespace CGE
{
	namespace Pass
	{
		class ForwardPass
		{
		private:
			// PerObject constant buffer data.
			__declspec(align(16)) struct PerObject
			{
				glm::mat4 m_modelViewProjection;
				glm::mat4 m_modelView;
			};
		public:
			ForwardPass() = default;
			void Init(RHI::Device& device);
			void SetScenePtr(std::shared_ptr<Scene::Scene> scene);
			void Render(RHI::CommandList* commandList);
			void PushSrg(RHI::ShaderResourceGroup* srg);
			void PopSrg();
			void UpdatePerObjectData(Scene::Camera& camera, glm::mat4& nodeWorldTransform);
			void AddDrawItem(RHI::DrawItem* drawItem);
			RHI::ImageView& GetDSView();
			PerObject* GetPerObjectDataPtr();
			std::vector<RHI::ShaderResourceGroup*>& GetSrgsToBind();

			// [todo] remove
			const RHI::PipelineStateDescriptorForDraw& GetPipelineStateDescriptorForDraw();
			const RHI::PipelineState& GetPipelineState();


		private:
			// [todo] For now the pipeline layout will come from the pass class. Need to change later.
			// Each DrawItem needs to have or share its own version. For now let the pass own this layout.
			RHI::Ptr<RHI::PipelineState> m_pipelineState;
			RHI::PipelineStateDescriptorForDraw m_drawPipelineStateDescriptor;
			std::vector<RHI::DrawItem*> m_drawItems;
			std::vector<RHI::ShaderResourceGroup*> m_srgsToBind;

			PerObject* m_perObjectData;

			// Scene to render
			std::shared_ptr<Scene::Scene> m_scene;

			// FrameBuffers
			struct
			{
				RHI::Ptr<RHI::Image> m_depthBuffer;
				RHI::Ptr<RHI::ImageView> m_depthBufferView;
				std::array<RHI::Ptr<RHI::Image>, RHI::Limits::Device::FrameCountMax> m_colorTaget;
				std::array<RHI::Ptr<RHI::ImageView>, RHI::Limits::Device::FrameCountMax> m_colorTagetView;
			}m_frameBuffers;
		};
	}
}