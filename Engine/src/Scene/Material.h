#pragma once

// RHI
#include "../RHI/ShaderStageFunction.h"
#include "../RHI/Image.h"
#include "../RHI/ShaderResourceGroup.h"

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

            std::pair<RHI::Ptr<RHI::Image>, RHI::Ptr<RHI::ImageView>> GetTextureAndView(TextureType ID) const;
            void SetTexture(TextureType type, RHI::Ptr<RHI::Image> texture, RHI::Ptr<RHI::ImageView> textureView);
            RHI::Ptr<RHI::BufferView> GetMaterialCbuffView();
            void InitMaterialCbuff();
            void InitMaterialSrg();
            RHI::ShaderResourceGroup* GetMaterialSrg() const;
            void SpawnImGuiWindow();
            RHI::ResultCode UpdateMaterialBuffer();

		public:
            __declspec(align(16)) struct MaterialProperties
            {
                MaterialProperties()
                    : m_globalAmbient(0.1f, 0.1f, 0.15f, 1)
                    , m_ambientColor(0, 0, 0, 1)
                    , m_emissiveColor(0, 0, 0, 1)
                    , m_diffuseColor(1, 1, 1, 1)
                    , m_specularColor(0, 0, 0, 1)
                    , m_reflectance(0, 0, 0, 0)
                    , m_opacity(1.0f)
                    , m_specularPower(-1.0f)
                    , m_indexOfRefraction(-1.0f)
                    , m_hasAmbientTexture(false)
                    , m_hasEmissiveTexture(false)
                    , m_hasDiffuseTexture(false)
                    , m_hasSpecularTexture(false)
                    , m_hasSpecularPowerTexture(false)
                    , m_hasNormalTexture(false)
                    , m_hasBumpTexture(false)
                    , m_hasOpacityTexture(false)
                    , m_bumpIntensity(5.0f)
                    , m_specularScale(128.0f)
                    , m_alphaThreshold(0.1f) {}

                glm::vec4   m_globalAmbient;
                //-------------------------- ( 16 bytes )
                glm::vec4   m_ambientColor;
                //-------------------------- ( 16 bytes )
                glm::vec4   m_emissiveColor;
                //-------------------------- ( 16 bytes )
                glm::vec4   m_diffuseColor;
                //-------------------------- ( 16 bytes )
                glm::vec4   m_specularColor;
                //-------------------------- ( 16 bytes )
                glm::vec4   m_reflectance;
                //-------------------------- ( 16 bytes )
                // If Opacity < 1, then the material is transparent.
                float       m_opacity;
                float       m_specularPower;
                // For transparent materials, IOR > 0.
                float       m_indexOfRefraction;
                uint32_t    m_hasAmbientTexture;
                //-------------------------- ( 16 bytes )
                uint32_t    m_hasEmissiveTexture;
                uint32_t    m_hasDiffuseTexture;
                uint32_t    m_hasSpecularTexture;
                uint32_t    m_hasSpecularPowerTexture;
                //-------------------------- ( 16 bytes )
                uint32_t    m_hasNormalTexture;
                uint32_t    m_hasBumpTexture;
                uint32_t    m_hasOpacityTexture;
                float       m_bumpIntensity;    // When using bump textures (heightmaps) we need 
                                                // to scale the height values so the normals are visible.
                //-------------------------- ( 16 bytes )
                float       m_specularScale;    // When reading specular power from a texture, 
                                                // we need to scale it into the correct range.
                float       m_alphaThreshold;   // Pixels with alpha < m_AlphaThreshold will be discarded.
                glm::vec2   m_padding;          // Pad to 16 byte boundary.
                //-------------------------- ( 16 bytes )
            };  //--------------------------- ( 16 * 10 = 160 bytes )
		private:
            MaterialProperties* m_pProperties;
            RHI::Ptr<RHI::Buffer> m_materialPropertiesCBuff;
            RHI::Ptr<RHI::BufferView> m_materialPropertiesCBuffView;
            bool m_dirty = false;

            // [todo] I need to setup proper json for the material srg and also its layout.
            RHI::Ptr<RHI::ShaderResourceGroup> m_materialSrg;

            typedef std::unordered_map<TextureType, std::pair<RHI::Ptr<RHI::Image>, RHI::Ptr<RHI::ImageView>>> TextureMap;
            TextureMap m_textures;
		};
	}
}