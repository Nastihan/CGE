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
			Material() = default;
            ~Material() = default;

            void SetTexture(const std::string& name, RHI::Ptr<RHI::Image> texture, RHI::Ptr<RHI::ImageView> textureView);
            RHI::Ptr<RHI::BufferView> GetMaterialCbuffView();
            void InitMaterialCbuff();
            void InitMaterialSrg();
            RHI::ShaderResourceGroup* GetMaterialSrg() const;
            void SpawnImGuiWindow();
            RHI::ResultCode UpdateMaterialBuffer();

            void SetShaderPermutation(std::shared_ptr<RHI::ShaderPermutation> permutation);
            const std::shared_ptr<const RHI::ShaderPermutation> GetShaderPermutation();
            void InsertProperty(const std::string& name, const PropertyInfo& propertyInfo);
            const PropertyInfo& GetPropertyInfo(const std::string& name);
            void InitMaterialProperty(uint32_t totalSizeInBytes);

            template<typename T>
            void SetProperty(const std::string& propertyName, T value);

		private:
            RHI::Ptr<RHI::Buffer> m_materialPropertiesCBuff;
            RHI::Ptr<RHI::BufferView> m_materialPropertiesCBuffView;
            bool m_dirty = false;

            // [todo] I need to setup proper json for the material srg and also its layout.
            RHI::Ptr<RHI::ShaderResourceGroup> m_materialSrg;

            typedef std::unordered_map<std::string, std::pair<RHI::Ptr<RHI::Image>, RHI::Ptr<RHI::ImageView>>> TextureMap;
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