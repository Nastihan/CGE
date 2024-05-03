#pragma once

// Scene
#include "Camera.h"
#include "Light.h"

// std
#include <vector>
#include <string>

namespace CGE
{
	namespace Scene
	{
		class Model;
		class Mesh;

		class Scene final
		{
		public:
			Scene() = default;
			void LoadModel(const std::string& pathString);
		private:
			Camera m_camera;
			std::vector<Model> m_models;
			std::vector<Light> m_lights
		};
	}
}