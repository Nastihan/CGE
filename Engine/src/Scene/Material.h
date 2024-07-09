#pragma once

// RHI
#include "../RHI/ShaderStageFunction.h"
#include "../RHI/Image.h"
#include "../RHI/ShaderResourceGroup.h"

// std
#include <filesystem>
#include <map>

// glm
#include <glm/glm.hpp>

struct aiMaterial;

namespace DirectX
{
	struct TexMetadata;
	class ScratchImage;
}

namespace CGE
{
    namespace RHI
    {
        struct ShaderPermutation;
    }
	namespace Scene
	{
		class Material
		{
        public:
            struct PropertyInfo
            {
                uint32_t m_offset = 0;
                std::string m_type;
                std::string m_reflectionUI;
                std::pair<std::optional<float>, std::optional<float>> m_reflectionMinMax;
            };
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

            void SetShaderPermutation(std::shared_ptr<RHI::ShaderPermutation> permutation);
            void InsertProperty(const std::string& name, const PropertyInfo& propertyInfo);
            void InitMaterialProperty(uint32_t totalSizeInBytes);

            template<typename T>
            void SetProperty(const std::string& propertyName, T value);

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

            // The asset processor will set this based on what it detects from the MaterialLayout file.
            // Each material layout json file should have set the "ShaderName" field will have 
            std::shared_ptr<RHI::ShaderPermutation> m_shaderPermutation = nullptr;

            // Each material will spawn a property layout which will get populated in the asset processor.
            // This span of bytes will be used to set a material property constant buffer on the shaders side.
            // The engine will make sure the data has the correct alignment and follows packing rules.
            std::vector<uint8_t> m_materialProperties;
            std::unordered_map<std::string, PropertyInfo> m_nameToInfoMap;
		};

        template<typename T>
        void Material::SetProperty(const std::string& propertyName, T value)
        {
            if (m_nameToInfoMap.find(propertyName) != m_nameToInfoMap.end())
            {
                const PropertyInfo& prop = m_nameToInfoMap[propertyName];
                uint32_t offset = prop.m_offset;
                std::string type = prop.m_type;

                if (type == "float4")
                {
                    glm::vec4& val = *reinterpret_cast<glm::vec4*>(m_materialProperties.data() + offset);
                    val = *reinterpret_cast<glm::vec4*>(&value);
                }
                else if (type == "float3")
                {
                    glm::vec3& val = *reinterpret_cast<glm::vec3*>(m_materialProperties.data() + offset);
                    val = *reinterpret_cast<glm::vec3*>(&value);
                }
                else if (type == "float2")
                {
                    glm::vec2& val = *reinterpret_cast<glm::vec2*>(m_materialProperties.data() + offset);
                    val = *reinterpret_cast<glm::vec2*>(&value);
                }
                else if (type == "float")
                {
                    float& val = *reinterpret_cast<float*>(m_materialProperties.data() + offset);
                    val = *reinterpret_cast<float*>(&value);
                }
                else if (type == "bool")
                {
                    uint32_t& val = *reinterpret_cast<uint32_t*>(m_materialProperties.data() + offset);
                    val = *reinterpret_cast<uint32_t*>(&value);
                }
                else
                {
                    assert(false, "Unknown type");
                }
            }
        }
	}
}