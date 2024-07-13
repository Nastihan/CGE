#pragma once

// std
#include <string>
#include <memory>
#include <filesystem>
#include <map>

// RHI
#include "../RHI/RHI_Common.h"
#include "../RHI/ShaderResourceGroup.h"
#include "../RHI/DrawItem.h"

// glm
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

struct aiMaterial;
struct aiMesh;
struct aiNode;

namespace DirectX
{
	struct TexMetadata;
	class ScratchImage;
}

namespace CGE
{
	namespace RHI
	{
		class Image;
		class Buffer;
		class CommandList;
	}

	namespace Pass
	{
		class ForwardPass;
	}

	namespace Scene
	{
		class ModelNode;
		class Mesh;
		class Material;
		class Camera;

		class Model
		{
		public:
			Model(const std::string& name, glm::vec3 rootTranslation, glm::vec3 rootScale, glm::quat rootRotation);
			~Model();

			bool LoadFromFile(const std::string& pathString, const std::string modelName);
			void BuildDrawList(std::vector<RHI::DrawItem>& drawItems, std::array<RHI::ShaderResourceGroup*, RHI::Limits::Pipeline::ShaderResourceGroupCountMax>& srgsToBind) const;
			const std::string& GetName();

			void SpawnImGuiWindow();
			void Update();

		private:
			void ImportMaterial(const aiMaterial& material, const std::string& parentPath);
			void ImportMesh(const aiMesh& mesh);
			std::shared_ptr<ModelNode> ImportSceneNode(std::shared_ptr<ModelNode> parent, aiNode* aiNode);
			RHI::ResultCode ConstructTexture(RHI::Ptr<RHI::Image> image, const DirectX::TexMetadata& metaData, const DirectX::ScratchImage& scratchImage);
			RHI::ResultCode ConstructInputAssemblyBuffer(RHI::Ptr<RHI::Buffer> buffer, const void* data, unsigned int count, unsigned int stride);

		private:
			typedef std::vector<std::shared_ptr<Material>> MaterialList;
			typedef std::vector<std::shared_ptr<Mesh>> MeshList;

			glm::vec3 m_rootTranslation;
			glm::vec3 m_rootScale;
			glm::quat m_rootRotation;

			// Using this to compute the delta in ImGui (In euler angles degrees)
			glm::vec3 m_previousRotation{};
			glm::vec3 m_currentRotation{};

			bool m_dirty = false;

			std::string m_modelName;
			std::shared_ptr<ModelNode> m_root;
			MaterialList m_materials;
			MeshList m_meshes;
		};
	}
}