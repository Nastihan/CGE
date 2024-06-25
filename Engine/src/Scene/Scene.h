#pragma once

// Scene
#include "Camera.h"
#include "Light.h"
#include "Model.h"

// RHI
#include "../RHI/Buffer.h"
#include "../RHI/ShaderResourceGroup.h"

// std
#include <vector>
#include <string>

namespace CGE
{
	namespace Pass
	{
		class ForwardPass;
	}
	namespace RHI
	{
		class CommandList;
	}

	namespace Scene
	{
		class Model;
		class Mesh;

		class Scene final
		{
		public:
			Scene() = default;
			void Init(Pass::ForwardPass* pForwardPass);
			void LoadModel(const std::string& pathString, Pass::ForwardPass* pForwardPass);
			void AddLight(const Light& light);
			Camera& GetCamera();
			void SpawnLightImGuiWindow();
			void Update();
			std::vector<RHI::DrawItem> BuildDrawList();

		private:
			RHI::ResultCode UpdateLightBuffer();

		private:
			Camera m_camera;
			std::vector<Model> m_models;

			// [todo] need to hookup to ImGui
			// The scene will contain a fixed number of lights for now. (Same as NUM_LIGHTS in Forward shader)
			std::vector<Light> m_lights;
			int m_currentSelectedLight = 0;
			bool m_needLightUpdate = false;
			RHI::Ptr<RHI::Buffer> m_lightBuffer;
			RHI::Ptr<RHI::BufferView> m_lightBufferView;

			RHI::Ptr<RHI::ShaderResourceGroup> m_sceneSrg;
		};
	}
}