
// RHI
#include "../RHI/Image.h"

// DX12
#include "../DX_Interface/DX_CommonHeaders.h"
#include "../DX_Interface/DX_Conversions.h"

// assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// DXToolKit
#include <DirectXTex/DirectXTex.h>

// Scene
#include "Material.h"

namespace CGE
{
	namespace Scene
	{
		using namespace DirectX;

		Material::Material(const aiMaterial& material, const std::filesystem::path& path)
		{
			const auto rootPath = path.parent_path().string() + "\\";

			aiString texFileName;
			m_materialType = (material.GetTexture(aiTextureType_DIFFUSE, 0, &texFileName) == aiReturn_SUCCESS) ? MaterialType::SPECULAR_GLOSSINESS : MaterialType::METALLIC_ROUGHNESS;

			// if (material.GetTexture(aiTextureType_BASE_COLOR, 0, &texFileName) == aiReturn_SUCCESS)
			// {
				// RHI::SetBits(m_textureTypes, TexureType::BASE_COLOR);
				// Load and creat RHI::Image using dx toolkit
			// }
			if (material.GetTexture(aiTextureType_DIFFUSE, 0, &texFileName) == aiReturn_SUCCESS)
			{
				RHI::SetBits(m_textureTypes, TexureType::BASE_COLOR);
				// Load and creat RHI::Image using dx toolkit
				TexMetadata metaData;
				ScratchImage scratchImage;
				std::string texFileString = texFileName.C_Str();
				DX12::DXAssertSuccess(LoadFromWICFile(DX12::s2ws(texFileString).c_str(), WIC_FLAGS_FORCE_RGB, &metaData, scratchImage));
			}

			if (material.GetTexture(aiTextureType_SPECULAR, 0, &texFileName) == aiReturn_SUCCESS)
			{
				RHI::SetBits(m_textureTypes, TexureType::MATERIAL);
				// Load and creat RHI::Image using dx toolkit
			}

			if (material.GetTexture(aiTextureType_NORMALS, 0, &texFileName) == aiReturn_SUCCESS)
			{
				RHI::SetBits(m_textureTypes, TexureType::NORMAL);
				// Load and creat RHI::Image using dx toolkit
			}
		}

		RHI::ResultCode Material::ConstructImageAndView(RHI::Ptr<RHI::Image> image, RHI::Ptr<RHI::ImageView> imageView, const TexMetadata& metaData, const ScratchImage& scratchImage)
		{

			return RHI::ResultCode::Success;
		}
	}
}