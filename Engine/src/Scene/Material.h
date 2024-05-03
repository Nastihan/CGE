#pragma once

// RHI
#include "../RHI/ShaderStageFunction.h"
#include "../RHI/Image.h"

// std
#include <filesystem>
#include <map>

struct aiMaterial;

namespace DirectX
{
	struct TexMetadata;
	class ScratchImage;
}

namespace CGE
{
	namespace Scene
	{
		class Material
		{
        public:
            enum class TextureType
            {
                Ambient = 0,
                Emissive = 1,
                Diffuse = 2,
                Specular = 3,
                SpecularPower = 4,
                Normal = 5,
                Bump = 6,
                Opacity = 7,
            };
		public:
			Material();
            ~Material();

            const glm::vec4& GetDiffuseColor() const;
            void SetDiffuseColor(const glm::vec4& diffuse);
            const glm::vec4& GetGlobalAmbientColor() const;
            void SetGlobalAmbientColor(const glm::vec4& globalAmbient);
            const glm::vec4& GetAmbientColor() const;
            void SetAmbientColor(const glm::vec4& ambient);
            const glm::vec4& GetEmissiveColor() const;
            void SetEmissiveColor(const glm::vec4& emissive);
            const glm::vec4& GetSpecularColor() const;
            void SetSpecularColor(const glm::vec4& phong);
            float GetSpecularPower() const;
            void SetSpecularPower(float phongPower);
            const glm::vec4& GetReflectance() const;
            void SetReflectance(const glm::vec4& reflectance);
            const float GetOpacity() const;
            void SetOpacity(float Opacity);
            float GetIndexOfRefraction() const;
            void SetIndexOfRefraction(float indexOfRefraction);
            float GetBumpIntensity() const;
            void SetBumpIntensity(float bumpIntensity);

            RHI::Ptr<RHI::Image> GetTexture(TextureType ID) const;
            void SetTexture(TextureType type, RHI::Ptr<RHI::Image> texture);
		private:
            __declspec(align(16)) struct MaterialProperties
            {
                MaterialProperties()
                    : m_GlobalAmbient(0.1f, 0.1f, 0.15f, 1)
                    , m_AmbientColor(0, 0, 0, 1)
                    , m_EmissiveColor(0, 0, 0, 1)
                    , m_DiffuseColor(1, 1, 1, 1)
                    , m_SpecularColor(0, 0, 0, 1)
                    , m_Reflectance(0, 0, 0, 0)
                    , m_Opacity(1.0f)
                    , m_SpecularPower(-1.0f)
                    , m_IndexOfRefraction(-1.0f)
                    , m_HasAmbientTexture(false)
                    , m_HasEmissiveTexture(false)
                    , m_HasDiffuseTexture(false)
                    , m_HasSpecularTexture(false)
                    , m_HasSpecularPowerTexture(false)
                    , m_HasNormalTexture(false)
                    , m_HasBumpTexture(false)
                    , m_HasOpacityTexture(false)
                    , m_BumpIntensity(5.0f)
                    , m_SpecularScale(128.0f)
                    , m_AlphaThreshold(0.1f) {}

                glm::vec4   m_GlobalAmbient;
                //-------------------------- ( 16 bytes )
                glm::vec4   m_AmbientColor;
                //-------------------------- ( 16 bytes )
                glm::vec4   m_EmissiveColor;
                //-------------------------- ( 16 bytes )
                glm::vec4   m_DiffuseColor;
                //-------------------------- ( 16 bytes )
                glm::vec4   m_SpecularColor;
                //-------------------------- ( 16 bytes )
                glm::vec4   m_Reflectance;
                //-------------------------- ( 16 bytes )
                // If Opacity < 1, then the material is transparent.
                float       m_Opacity;
                float       m_SpecularPower;
                // For transparent materials, IOR > 0.
                float       m_IndexOfRefraction;
                uint32_t    m_HasAmbientTexture;
                //-------------------------- ( 16 bytes )
                uint32_t    m_HasEmissiveTexture;
                uint32_t    m_HasDiffuseTexture;
                uint32_t    m_HasSpecularTexture;
                uint32_t    m_HasSpecularPowerTexture;
                //-------------------------- ( 16 bytes )
                uint32_t    m_HasNormalTexture;
                uint32_t    m_HasBumpTexture;
                uint32_t    m_HasOpacityTexture;
                float       m_BumpIntensity;    // When using bump textures (heightmaps) we need 
                                                // to scale the height values so the normals are visible.
                //-------------------------- ( 16 bytes )
                float       m_SpecularScale;    // When reading specular power from a texture, 
                                                // we need to scale it into the correct range.
                float       m_AlphaThreshold;   // Pixels with alpha < m_AlphaThreshold will be discarded.
                glm::vec2   m_Padding;          // Pad to 16 byte boundary.
                //-------------------------- ( 16 bytes )
            };  //--------------------------- ( 16 * 10 = 160 bytes )
		private:
            MaterialProperties* m_pProperties;
            RHI::Ptr<RHI::Buffer> m_materialPropertiesCBuff;

            typedef std::map<TextureType, RHI::Ptr<RHI::Image>> TextureMap;
            TextureMap m_Textures;
		};
	}
}