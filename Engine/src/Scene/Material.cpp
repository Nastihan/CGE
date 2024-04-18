
// Scene
#include "Material.h"

// RHI
#include "../RHI/Image.h"

// assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace CGE
{
	namespace Scene
	{
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
			}

			// if (material.GetTexture(aiTextureType_DIFFUSE_ROUGHNESS, 0, &texFileName) == aiReturn_SUCCESS)
			// {
				// RHI::SetBits(m_textureTypes, TexureType::MATERIAL);
				// Load and creat RHI::Image using dx toolkit
			// }
			// if (material.GetTexture(aiTextureType_SHININESS, 0, &texFileName) == aiReturn_SUCCESS)
			// {
				// RHI::SetBits(m_textureTypes, TexureType::MATERIAL);
				// Load and creat RHI::Image using dx toolkit
			// }

			// if (material.GetTexture(aiTextureType_METALNESS, 0, &texFileName) == aiReturn_SUCCESS)
			// {
				// RHI::SetBits(m_textureTypes, TexureType::MATERIAL);
				// Load and creat RHI::Image using dx toolkit
			// }
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
	}
}