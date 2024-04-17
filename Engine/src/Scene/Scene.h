#pragma once

// std
#include <vector>
#include <string>

namespace CGE
{
	class Model;
	class Mesh;

	class Scene final
	{
	public:
		Scene() = default;
		void LoadModel(const std::string& pathString);
	private:
		std::vector<Model> m_sceneModels;
	};
}