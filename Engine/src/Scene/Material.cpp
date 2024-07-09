
// RHI
#include "../RHI/Image.h"
#include "../RHI/Graphics.h"
#include "../RHI/AssetProcessor.h"

// DX12
#include "../DX_Interface/DX_CommonHeaders.h"
#include "../DX_Interface/DX_Conversions.h"

// assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// DXToolKit
#include <DirectXTex/DirectXTex.h>

// glm
#include <glm/glm.hpp>

// Scene
#include "Material.h"

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"

namespace CGE
{
	namespace Scene
	{
		Material::Material()
		{
			// Material properties have to be 16 byte aligned.
			// To guarantee alignment, we'll use _aligned_malloc to allocate memory for the material properties.
			m_pProperties = (MaterialProperties*)_aligned_malloc(sizeof(MaterialProperties), 16);
			*m_pProperties = MaterialProperties();
		}

		Material::~Material()
		{
			if (m_pProperties)
			{
				_aligned_free(m_pProperties);
				m_pProperties = nullptr;
			}
		}

		const glm::vec4& Material::GetGlobalAmbientColor() const
		{
			return m_pProperties->m_globalAmbient;
		}

		void Material::SetGlobalAmbientColor(const glm::vec4& globalAmbient)
		{
			m_pProperties->m_globalAmbient = globalAmbient;
		}

		const glm::vec4& Material::GetAmbientColor() const
		{
			return m_pProperties->m_ambientColor;
		}

		void Material::SetAmbientColor(const glm::vec4& ambient)
		{
			m_pProperties->m_ambientColor = ambient;
		}

		const glm::vec4& Material::GetDiffuseColor() const
		{
			return m_pProperties->m_diffuseColor;
		}

		void Material::SetDiffuseColor(const glm::vec4& diffuse)
		{
			m_pProperties->m_diffuseColor = diffuse;
		}

		const glm::vec4& Material::GetEmissiveColor() const
		{
			return m_pProperties->m_emissiveColor;
		}

		void Material::SetEmissiveColor(const glm::vec4& emissive)
		{
			m_pProperties->m_emissiveColor = emissive;
		}

		const glm::vec4& Material::GetSpecularColor() const
		{
			return m_pProperties->m_specularColor;
		}

		void Material::SetSpecularColor(const glm::vec4& specular)
		{
			m_pProperties->m_specularColor = specular;
		}

		float Material::GetSpecularPower() const
		{
			return m_pProperties->m_specularPower;
		}

		const float Material::GetOpacity() const
		{
			return m_pProperties->m_opacity;
		}

		void Material::SetOpacity(float Opacity)
		{
			m_pProperties->m_opacity = Opacity;
		}

		void Material::SetSpecularPower(float phongPower)
		{
			m_pProperties->m_specularPower = phongPower;
		}

		const glm::vec4& Material::GetReflectance() const
		{
			return m_pProperties->m_reflectance;
		}

		void Material::SetReflectance(const glm::vec4& reflectance)
		{
			m_pProperties->m_reflectance = reflectance;
		}

		float Material::GetIndexOfRefraction() const
		{
			return m_pProperties->m_indexOfRefraction;
		}

		void Material::SetIndexOfRefraction(float indexOfRefraction)
		{
			m_pProperties->m_indexOfRefraction = indexOfRefraction;
		}

		float Material::GetBumpIntensity() const
		{
			return m_pProperties->m_bumpIntensity;
		}
		void Material::SetBumpIntensity(float bumpIntensity)
		{
			m_pProperties->m_bumpIntensity = bumpIntensity;
		}

		std::pair<RHI::Ptr<RHI::Image>, RHI::Ptr<RHI::ImageView>> Material::GetTextureAndView(TextureType type) const
		{
			auto itr = m_textures.find(type);
			if (itr != m_textures.end())
			{
				return { itr->second.first, itr->second.second };
			}

			return { nullptr, nullptr };
		}

		void Material::SetTexture(TextureType type, RHI::Ptr<RHI::Image> texture, RHI::Ptr<RHI::ImageView> textureView)
		{
			if (texture != nullptr)
			{
				m_textures.insert({ type, {texture, textureView} });
			}

			switch (type)
			{
			case TextureType::Ambient:
			{
				m_pProperties->m_hasAmbientTexture = (texture != nullptr);
			}
			break;
			case TextureType::Emissive:
			{
				m_pProperties->m_hasEmissiveTexture = (texture != nullptr);
			}
			break;
			case TextureType::Diffuse:
			{
				m_pProperties->m_hasDiffuseTexture = (texture != nullptr);
			}
			break;
			case TextureType::Specular:
			{
				m_pProperties->m_hasSpecularTexture = (texture != nullptr);
			}
			break;
			case TextureType::SpecularPower:
			{
				m_pProperties->m_hasSpecularPowerTexture = (texture != nullptr);
			}
			break;
			case TextureType::Normal:
			{
				m_pProperties->m_hasNormalTexture = (texture != nullptr);
			}
			break;
			case TextureType::Bump:
			{
				m_pProperties->m_hasBumpTexture = (texture != nullptr);
			}
			break;
			case TextureType::Opacity:
			{
				m_pProperties->m_hasOpacityTexture = (texture != nullptr);
			}
			break;
			}
		}

		RHI::Ptr<RHI::BufferView> Material::GetMaterialCbuffView()
		{
			return m_materialPropertiesCBuffView;
		}

		void Material::InitMaterialCbuff()
		{
			const auto& constantBufferPool = RHI::Graphics::GetBufferSystem().GetCommonBufferPool(RHI::CommonBufferPoolType::Constant);
			auto& rhiFactory = RHI::Graphics::GetFactory();
			m_materialPropertiesCBuff = rhiFactory.CreateBuffer();
			RHI::ResultCode result = RHI::ResultCode::Fail;

			RHI::BufferInitRequest materialCBufferRequest;
			materialCBufferRequest.m_buffer = m_materialPropertiesCBuff.get();
			materialCBufferRequest.m_descriptor.m_byteCount = sizeof(MaterialProperties);
			materialCBufferRequest.m_descriptor.m_bindFlags = RHI::BufferBindFlags::Constant;
			materialCBufferRequest.m_initialData = m_pProperties;
			result = constantBufferPool->InitBuffer(materialCBufferRequest);
			assert(result == RHI::ResultCode::Success);

			RHI::BufferViewDescriptor materialPropertiesBufferViewDescriptor = RHI::BufferViewDescriptor::CreateRaw(0, sizeof(MaterialProperties));
			m_materialPropertiesCBuffView = rhiFactory.CreateBufferView();
			m_materialPropertiesCBuffView->Init(*m_materialPropertiesCBuff, materialPropertiesBufferViewDescriptor);
		}

		void Material::InitMaterialSrg()
		{
			// [todo] Currently I'm building the material srg from the DefaultPBRForward_MaterialShader this will not allow fully custom material layouts.
			// Later I have to enable a material layout + material json files so the user can create fully custom materials for their usecase.
			const RHI::ShaderPermutation& specularGlossiness_Shader = *RHI::Graphics::GetAssetProcessor().GetShaderPermutation("SpecularGlossiness_Shader");
			const RHI::ShaderResourceGroupLayout* materialSrgLayout = specularGlossiness_Shader.m_pipelineLayoutDescriptor->GetShaderResourceGroupLayout(RHI::ShaderResourceGroupType::Material);
			m_materialSrg = RHI::Graphics::GetFactory().CreateShaderResourceGroup();
			RHI::ShaderResourceGroupData materialSrgData(materialSrgLayout);

			std::vector<RHI::ShaderInputImageIndex> materialTextureIndicies;
			materialTextureIndicies.push_back(materialSrgLayout->FindShaderInputImageIndex("PerMaterial_AmbientTexture"));
			materialTextureIndicies.push_back(materialSrgLayout->FindShaderInputImageIndex("PerMaterial_EmissiveTexture"));
			materialTextureIndicies.push_back(materialSrgLayout->FindShaderInputImageIndex("PerMaterial_DiffuseTexture"));
			materialTextureIndicies.push_back(materialSrgLayout->FindShaderInputImageIndex("PerMaterial_SpecularTexture"));
			materialTextureIndicies.push_back(materialSrgLayout->FindShaderInputImageIndex("PerMaterial_NormalTexture"));
			materialTextureIndicies.push_back(materialSrgLayout->FindShaderInputImageIndex("PerMaterial_SpecularPowerTexture"));
			materialTextureIndicies.push_back(materialSrgLayout->FindShaderInputImageIndex("PerMaterial_BumpTexture"));
			materialTextureIndicies.push_back(materialSrgLayout->FindShaderInputImageIndex("PerMaterial_OpacityTexture"));
			for (size_t arrayIdx = 0; arrayIdx < materialTextureIndicies.size(); arrayIdx++)
			{
				auto viewPtr = GetTextureAndView(static_cast<Material::TextureType>(arrayIdx)).second;
				if (viewPtr)
				{
					materialSrgData.SetImageView(materialTextureIndicies[arrayIdx], viewPtr.get(), 0);
				}
			}

			RHI::ShaderInputBufferIndex materialPropertiesBufferIdx = materialSrgLayout->FindShaderInputBufferIndex("PerMaterial_MaterialProperties");
			materialSrgData.SetBufferView(materialPropertiesBufferIdx, GetMaterialCbuffView().get(), 0);
			m_materialSrg->Init(m_materialPropertiesCBuff->GetDevice(), materialSrgData);
			m_materialSrg->Compile();
		}

		RHI::ShaderResourceGroup* Material::GetMaterialSrg() const
		{
			return m_materialSrg.get();
		}

		void Material::SpawnImGuiWindow()
		{
			const auto updateChange = [this](bool check) {m_dirty = check || m_dirty; };

			if (ImGui::TreeNode("Material"))
			{
				updateChange(ImGui::ColorEdit4("Global Ambient", &m_pProperties->m_globalAmbient[0]));
				updateChange(ImGui::ColorEdit4("Ambient Color", &m_pProperties->m_ambientColor[0]));
				updateChange(ImGui::ColorEdit4("Emissive Color", &m_pProperties->m_emissiveColor[0]));
				updateChange(ImGui::ColorEdit4("Diffuse Color", &m_pProperties->m_diffuseColor[0]));
				updateChange(ImGui::ColorEdit4("Specular Color", &m_pProperties->m_specularColor[0]));
				updateChange(ImGui::ColorEdit4("Reflectance", &m_pProperties->m_reflectance[0]));

				updateChange(ImGui::SliderFloat("Opacity", &m_pProperties->m_opacity, 0.0f, 1.0f));
				updateChange(ImGui::SliderFloat("Specular Power", &m_pProperties->m_specularPower, 0.0f, 128.0f));
				updateChange(ImGui::SliderFloat("Index of Refraction", &m_pProperties->m_indexOfRefraction, 0.0f, 5.0f));

				updateChange(ImGui::Checkbox("Has Ambient Texture", (bool*)&m_pProperties->m_hasAmbientTexture));
				updateChange(ImGui::Checkbox("Has Emissive Texture", (bool*)&m_pProperties->m_hasEmissiveTexture));
				updateChange(ImGui::Checkbox("Has Diffuse Texture", (bool*)&m_pProperties->m_hasDiffuseTexture));
				updateChange(ImGui::Checkbox("Has Specular Texture", (bool*)&m_pProperties->m_hasSpecularTexture));
				updateChange(ImGui::Checkbox("Has Specular Power Texture", (bool*)&m_pProperties->m_hasSpecularPowerTexture));
				updateChange(ImGui::Checkbox("Has Normal Texture", (bool*)&m_pProperties->m_hasNormalTexture));
				updateChange(ImGui::Checkbox("Has Bump Texture", (bool*)&m_pProperties->m_hasBumpTexture));
				updateChange(ImGui::Checkbox("Has Opacity Texture", (bool*)&m_pProperties->m_hasOpacityTexture));

				updateChange(ImGui::SliderFloat("Bump Intensity", &m_pProperties->m_bumpIntensity, 0.0f, 10.0f));
				updateChange(ImGui::SliderFloat("Specular Scale", &m_pProperties->m_specularScale, 0.0f, 128.0f));
				updateChange(ImGui::SliderFloat("Alpha Threshold", &m_pProperties->m_alphaThreshold, 0.0f, 1.0f));
				ImGui::TreePop();
			}
		}

		RHI::ResultCode Material::UpdateMaterialBuffer()
		{
			if (m_dirty)
			{
				const auto& constantBufferPool = RHI::Graphics::GetBufferSystem().GetCommonBufferPool(RHI::CommonBufferPoolType::Constant);

				RHI::BufferMapRequest mapRequest{};
				mapRequest.m_buffer = m_materialPropertiesCBuff.get();
				mapRequest.m_byteCount = sizeof(MaterialProperties);
				mapRequest.m_byteOffset = 0;

				RHI::BufferMapResponse mapResponse{};

				RHI::ResultCode mapSuccess = constantBufferPool->MapBuffer(mapRequest, mapResponse);
				if (mapSuccess == RHI::ResultCode::Success)
				{
					memcpy(mapResponse.m_data, m_pProperties, sizeof(MaterialProperties));
					constantBufferPool->UnmapBuffer(*m_materialPropertiesCBuff);
				}
				m_dirty = false;
				return mapSuccess;
			}
		}

		void Material::SetShaderPermutation(std::shared_ptr<RHI::ShaderPermutation> permutation)
		{
			m_shaderPermutation = permutation;
		}

		void Material::InsertProperty(const std::string& name, const PropertyInfo& propertyInfo)
		{
			assert(m_nameToInfoMap.find(name) == m_nameToInfoMap.end(), "The property map should not contain duplicate names. Check your material layout json file.");
			m_nameToInfoMap.insert({ name, propertyInfo });
		}

		void Material::InitMaterialProperty(uint32_t totalSizeInBytes)
		{
			m_materialProperties.reserve(totalSizeInBytes);
		}
	}
}