#pragma once

// std
#include <string>
#include <memory>
#include <filesystem>
#include <map>

// RHI
#include "../RHI/RHI_Common.h"
#include "../RHI/ShaderResourceGroup.h"

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
			Model();
			~Model();

			bool LoadFromFile(const std::string& fileName, Pass::ForwardPass* pForwardPass);
			void Render(Pass::ForwardPass* pForwardPass, Camera& camera, RHI::CommandList* commandList) const;

		private:
			void ImportMaterial(const aiMaterial& material, const std::string& parentPath);
			void ImportMesh(const aiMesh& mesh, Pass::ForwardPass* pForwardPass);
			std::shared_ptr<ModelNode> ImportSceneNode(std::shared_ptr<ModelNode> parent, aiNode* aiNode);
			RHI::ResultCode ConstructTexture(RHI::Ptr<RHI::Image> image, const DirectX::TexMetadata& metaData, const DirectX::ScratchImage& scratchImage);
			RHI::ResultCode ConstructInputAssemblyBuffer(RHI::Ptr<RHI::Buffer> buffer, const void* data, unsigned int count, unsigned int stride);

		private:
			typedef std::vector<std::shared_ptr<Material>> MaterialList;
			typedef std::vector<std::shared_ptr<Mesh>> MeshList;

			std::string m_modelFile;
			std::shared_ptr<ModelNode> m_root;
			MaterialList m_materials;
			MeshList m_meshes;
		};
	}
}