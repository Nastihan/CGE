#pragma once

// std
#include <string>
#include <memory>
#include <filesystem>

namespace CGE
{
	namespace Scene
	{
		class ModelNode;
		class Mesh;

		class Model
		{
		public:
			Model(const std::string& pathString);
		private:
			std::unique_ptr<ModelNode> m_root;
			std::vector<std::unique_ptr<Mesh>> m_meshPtrs;
		};
	}
}