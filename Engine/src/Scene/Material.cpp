
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

			const auto& bufferPool = RHI::Graphics::GetBufferSystem().GetCommonBufferPool(RHI::CommonBufferPoolType::ReadOnly);
			RHI::ResultCode result = RHI::ResultCode::Fail;

			RHI::BufferInitRequest materialCBufferRequest;
			materialCBufferRequest.m_buffer = m_materialPropertiesCBuff.get();
			materialCBufferRequest.m_descriptor.m_byteCount = sizeof(MaterialProperties);
			materialCBufferRequest.m_descriptor.m_bindFlags = RHI::BufferBindFlags::ShaderRead;
			materialCBufferRequest.m_initialData = m_pProperties;
			result = bufferPool->InitBuffer(materialCBufferRequest);
			assert(result != RHI::ResultCode::Success);

			// [todo] Buffer view (shader read)
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

		RHI::Ptr<RHI::Image> Material::GetTexture(TextureType type) const
		{
			TextureMap::const_iterator itr = m_Textures.find(type);
			if (itr != m_Textures.end())
			{
				return itr->second;
			}

			return nullptr;
		}

		void Material::SetTexture(TextureType type, RHI::Ptr<RHI::Image> texture)
		{
			m_Textures[type] = texture;

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
	}
}