#pragma once

// std
#include <vector>
#include <memory>
#include <string>

#include <DirectXMath.h>

namespace CGE
{
	namespace Scene
	{
		class Mesh;

		// The scene will consist of models each model is a collection on meshesh.
		// Each mesh (sub-mesh) position can be offset relative to the base of the model (root) or any other sub-mesh in the model.
		// The final transform of the mesh will consider the concatenation of all its parentes offsets.
		class ModelNode
		{
			friend class Model;
		public:
			ModelNode(int id, const std::string& name, std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& transform);

			void SetAppliedTransform(DirectX::FXMMATRIX transform);
			const DirectX::XMFLOAT4X4& GetAppliedTransform() const;
			bool HasChildren() const;

		private:
			void AddChild(std::unique_ptr<ModelNode> pChild);
		private:
			std::string m_name;
			std::vector<std::unique_ptr<ModelNode>> m_childPtrs;
			std::vector<Mesh*> m_meshPtrs;
			DirectX::XMFLOAT4X4 m_transform;
			DirectX::XMFLOAT4X4 appliedTransform;
		};
	}
}