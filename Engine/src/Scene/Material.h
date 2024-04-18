#pragma once

// RHI
#include "../RHI/ShaderStageFunction.h"
#include "../RHI/Image.h"

// std
#include <filesystem>

struct aiMaterial;

namespace CGE
{
	namespace Scene
	{
		// For now default pbr workflows. Need to set workflow in material (or object) cbuff.
		enum class MaterialType : uint8_t
		{
			METALLIC_ROUGHNESS = 0,
			SPECULAR_GLOSSINESS,
			INVALID
		};

		// [todo] channel mappings
		enum class TexureType : uint8_t
		{
			None = 0,

			// Base color or Diffuse (Albedo)
			BASE_COLOR = (1u << 0u),

			// Normal map
			NORMAL = (1u << 1u),

			// Metallic-Roughness or Specular-Glossiness
			MATERIAL = (1u << 2u),

			AMBIENT_OCCLUSION = (1u << 3u)
		};
		inline TexureType operator|(TexureType a, TexureType b)
		{
			return static_cast<TexureType>(static_cast<int>(a) | static_cast<int>(b));
		}

		class Material
		{
		public:
		public:
			Material() = default;
			Material(const aiMaterial& material, const std::filesystem::path& path);
		private:
			static constexpr size_t DEFAULT_MATERIAL_MODEL_TEXTURE_COUNT = 4;
			RHI::Ptr<RHI::ShaderStageFunction> m_vertexShader;
			RHI::Ptr<RHI::ShaderStageFunction> m_pixelShader;
			std::array<RHI::Ptr<RHI::Image>, DEFAULT_MATERIAL_MODEL_TEXTURE_COUNT> m_textures;
			std::array<RHI::Ptr<RHI::ImageView>, DEFAULT_MATERIAL_MODEL_TEXTURE_COUNT> m_textureViews;
			TexureType m_textureTypes = TexureType::None;
			MaterialType m_materialType = MaterialType::INVALID;
		};
	}
}