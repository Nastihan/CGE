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
		class Shape;

		class Scene final
		{
		public:
			Scene() = default;
			void Init();
			void LoadModel(const std::string& pathString, const std::string& modelName, glm::vec3 pos, glm::vec3 scale, glm::quat rot);
			void AddLight(const Light& light);
			void AddShape(std::shared_ptr<Shape> shape);
			Camera& GetCamera();
			void SpawnImGuiWindow();
			void Update();
			std::vector<RHI::DrawItem> BuildDrawList();

		private:
			void SpawnLightImGuiWindow();
			void SpawnModelsImGuiWindow();
			RHI::ResultCode UpdateLightBuffer();

		private:
			Camera m_camera;

			std::vector<Model> m_models;
			int m_currentSelectedModel = 0;

			std::vector<std::shared_ptr<Shape>> m_shapes;
			int m_currentSelectedShape = 0;

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