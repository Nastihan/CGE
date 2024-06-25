
// RHI
#include "../RHI/Image.h"
#include "../RHI/Graphics.h"

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
			return m_pProperties->m_GlobalAmbient;
		}

		void Material::SetGlobalAmbientColor(const glm::vec4& globalAmbient)
		{
			m_pProperties->m_GlobalAmbient = globalAmbient;
		}

		const glm::vec4& Material::GetAmbientColor() const
		{
			return m_pProperties->m_AmbientColor;
		}

		void Material::SetAmbientColor(const glm::vec4& ambient)
		{
			m_pProperties->m_AmbientColor = ambient;
		}

		const glm::vec4& Material::GetDiffuseColor() const
		{
			return m_pProperties->m_DiffuseColor;
		}

		void Material::SetDiffuseColor(const glm::vec4& diffuse)
		{
			m_pProperties->m_DiffuseColor = diffuse;
		}

		const glm::vec4& Material::GetEmissiveColor() const
		{
			return m_pProperties->m_EmissiveColor;
		}

		void Material::SetEmissiveColor(const glm::vec4& emissive)
		{
			m_pProperties->m_EmissiveColor = emissive;
		}

		const glm::vec4& Material::GetSpecularColor() const
		{
			return m_pProperties->m_SpecularColor;
		}

		void Material::SetSpecularColor(const glm::vec4& specular)
		{
			m_pProperties->m_SpecularColor = specular;
		}

		float Material::GetSpecularPower() const
		{
			return m_pProperties->m_SpecularPower;
		}

		const float Material::GetOpacity() const
		{
			return m_pProperties->m_Opacity;
		}

		void Material::SetOpacity(float Opacity)
		{
			m_pProperties->m_Opacity = Opacity;
		}

		void Material::SetSpecularPower(float phongPower)
		{
			m_pProperties->m_SpecularPower = phongPower;
		}

		const glm::vec4& Material::GetReflectance() const
		{
			return m_pProperties->m_Reflectance;
		}

		void Material::SetReflectance(const glm::vec4& reflectance)
		{
			m_pProperties->m_Reflectance = reflectance;
		}

		float Material::GetIndexOfRefraction() const
		{
			return m_pProperties->m_IndexOfRefraction;
		}

		void Material::SetIndexOfRefraction(float indexOfRefraction)
		{
			m_pProperties->m_IndexOfRefraction = indexOfRefraction;
		}

		float Material::GetBumpIntensity() const
		{
			return m_pProperties->m_BumpIntensity;
		}
		void Material::SetBumpIntensity(float bumpIntensity)
		{
			m_pProperties->m_BumpIntensity = bumpIntensity;
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
				m_pProperties->m_HasAmbientTexture = (texture != nullptr);
			}
			break;
			case TextureType::Emissive:
			{
				m_pProperties->m_HasEmissiveTexture = (texture != nullptr);
			}
			break;
			case TextureType::Diffuse:
			{
				m_pProperties->m_HasDiffuseTexture = (texture != nullptr);
			}
			break;
			case TextureType::Specular:
			{
				m_pProperties->m_HasSpecularTexture = (texture != nullptr);
			}
			break;
			case TextureType::SpecularPower:
			{
				m_pProperties->m_HasSpecularPowerTexture = (texture != nullptr);
			}
			break;
			case TextureType::Normal:
			{
				m_pProperties->m_HasNormalTexture = (texture != nullptr);
			}
			break;
			case TextureType::Bump:
			{
				m_pProperties->m_HasBumpTexture = (texture != nullptr);
			}
			break;
			case TextureType::Opacity:
			{
				m_pProperties->m_HasOpacityTexture = (texture != nullptr);
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
			const RHI::ShaderPermutation& defaultPBRForward_MaterialShader = *RHI::Graphics::GetAssetProcessor().GetShaderPermutation("DefaultPBRForward_MaterialShader");
			const RHI::ShaderResourceGroupLayout* materialSrgLayout = defaultPBRForward_MaterialShader.m_pipelineLayoutDescriptor->GetShaderResourceGroupLayout(RHI::ShaderResourceGroupType::Material);
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
	}
}