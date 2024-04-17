#pragma once

// RHI
#include "../RHI/ShaderStageFunction.h"
#include "../RHI/Image.h"

// std
#include <fstream>

struct aiMaterial;

namespace CGE
{
	namespace Scene
	{
		class Material
		{
		public:
			// For now default pbr workflows. Need to set workflow in material cbuff.
			enum class MaterialType : uint8_t
			{
				METALLIC_ROUGHNESS = 0,
				SPECULAR_GLOSSINESS
			};

			// [todo] channel mappings
			enum class TexureIndex : uint8_t
			{
				// Base color or Diffuse (Albedo)
				BASE_COLOR = 0,
				
				// Normal map
				NORMAL,

				// Metallic-Roughness or Specular-Glossiness
				MATERIAL,

				AMBIENT_OCCLUSION,

				TEXTURE_COUNT
			};
		public:
			Material() = default;
			Material(const aiMaterial& material, const std::filesystem::path& path);
		private:
			static constexpr size_t TextureCount = static_cast<size_t>(TexureIndex::TEXTURE_COUNT);
			RHI::Ptr<RHI::ShaderStageFunction> m_vertexShader;
			RHI::Ptr<RHI::ShaderStageFunction> m_pixelShader;
			std::array<RHI::Ptr<RHI::Image>, TextureCount> m_textures;
			std::array<RHI::Ptr<RHI::ImageView>, TextureCount> m_textureViews;
		};
	}
}