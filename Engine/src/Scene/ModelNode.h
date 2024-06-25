#pragma once

// std
#include <vector>
#include <memory>
#include <string>
#include <map>

#include <glm/glm.hpp>

#include "../RHI/ShaderResourceGroup.h"
#include "../RHI/DrawItem.h"

namespace CGE
{
	namespace RHI
	{
		class CommandList;
	}

	namespace Pass
	{
		class ForwardPass;
	}

	namespace Scene
	{
		class Mesh;
		class Camera;

		// The scene will consist of models each model is a collection on meshesh.
		// Each mesh (sub-mesh) position can be offset relative to the base of the model (root) or any other sub-mesh in the model.
		// The final transform of the mesh will consider the concatenation of all its parentes offsets.
		class ModelNode : public std::enable_shared_from_this<ModelNode>
		{
		public:
			explicit ModelNode(const glm::mat4& localTransform = glm::mat4(1.0f));
			~ModelNode();

			const std::string& GetName() const;
			void SetName(const std::string& name);

			// Relative to it's parent world transform
			glm::mat4 GetLocalTransform() const;
			glm::mat4 GetInverseLocalTransform() const;
			void SetLocalTransform(const glm::mat4& localTransform);

			// Concatenated with parents world transform
			glm::mat4 GetWorldTransfom() const;
			glm::mat4 GetInverseWorldTransform() const;
			void SetWorldTransform(const glm::mat4& worldTransform);

			void AddChild(std::shared_ptr<ModelNode> pNode);
			void RemoveChild(std::shared_ptr<ModelNode> pNode);
			void SetParent(std::weak_ptr<ModelNode> pNode);

			void AddMesh(std::shared_ptr<Mesh> mesh);
			void RemoveMesh(std::shared_ptr<Mesh> mesh);

			void BuildDrawList(std::vector<RHI::DrawItem>& drawList, std::array<RHI::ShaderResourceGroup*, RHI::Limits::Pipeline::ShaderResourceGroupCountMax>& srgsToBind);
			RHI::ResultCode BuildModelMatrix();

		private:
			glm::mat4 GetParentWorldTransform() const;

		private:
			__declspec(align(16)) struct PerObjectData
			{
				glm::mat4 m_modelTransform;
			};

		private:
			typedef std::vector<std::shared_ptr<ModelNode>> NodeList;
			typedef std::vector<std::shared_ptr<Mesh>> MeshList;
			typedef std::multimap<std::string, std::shared_ptr<ModelNode>> NodeNameMap;

			std::string m_name;

			// Transforms node from parent's space to world space for rendering.
			glm::mat4 m_localTransform;
			glm::mat4 m_inverseTransform;

			std::weak_ptr<ModelNode> m_parent;
			NodeList m_children;
			MeshList m_meshes;
			NodeNameMap m_childrenByName;

			PerObjectData* m_perObjectData;
			RHI::Ptr<RHI::Buffer> m_modelTransformCbuff;
			RHI::Ptr<RHI::BufferView> m_modelTransformBufferView;
			RHI::Ptr<RHI::ShaderResourceGroup> m_objectSrg;
		};
	}
}