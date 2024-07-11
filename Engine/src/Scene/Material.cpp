
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
		void Material::SetTexture(const std::string& name, RHI::Ptr<RHI::Image> texture, RHI::Ptr<RHI::ImageView> textureView)
		{
			m_textures.insert({ name, {texture, textureView} });
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
			materialCBufferRequest.m_descriptor.m_byteCount = m_materialProperties.size();
			materialCBufferRequest.m_descriptor.m_bindFlags = RHI::BufferBindFlags::Constant;
			materialCBufferRequest.m_initialData = m_materialProperties.data();
			result = constantBufferPool->InitBuffer(materialCBufferRequest);
			assert(result == RHI::ResultCode::Success);

			RHI::BufferViewDescriptor materialPropertiesBufferViewDescriptor = RHI::BufferViewDescriptor::CreateRaw(0, m_materialProperties.size());
			m_materialPropertiesCBuffView = rhiFactory.CreateBufferView();
			m_materialPropertiesCBuffView->Init(*m_materialPropertiesCBuff, materialPropertiesBufferViewDescriptor);
		}

		void Material::InitMaterialSrg()
		{
			const RHI::ShaderResourceGroupLayout* materialSrgLayout = m_shaderPermutation->m_pipelineLayoutDescriptor->GetShaderResourceGroupLayout(RHI::ShaderResourceGroupType::Material);
			m_materialSrg = RHI::Graphics::GetFactory().CreateShaderResourceGroup();
			RHI::ShaderResourceGroupData materialSrgData(materialSrgLayout);

			for (const auto& shaderInput : m_textures)
			{
				RHI::ShaderInputImageIndex inputIdx = materialSrgLayout->FindShaderInputImageIndex(shaderInput.first);
				materialSrgData.SetImageView(inputIdx, shaderInput.second.second.get(), 0);
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
				for (const auto& propertyInfo : m_nameToInfoMap)
				{
					if (propertyInfo.second.m_reflectionUI == "ColorEdit4")
					{
						updateChange(ImGui::ColorEdit4(propertyInfo.first.c_str(), reinterpret_cast<float*>(m_materialProperties.data() + propertyInfo.second.m_offset)));
					}
					else if (propertyInfo.second.m_reflectionUI == "SliderFloat" || propertyInfo.second.m_reflectionUI == "InputFloat")
					{
						if (propertyInfo.second.m_reflectionMinMax.first.has_value() && propertyInfo.second.m_reflectionMinMax.second.has_value())
						{
							updateChange(ImGui::SliderFloat(propertyInfo.first.c_str(), reinterpret_cast<float*>(m_materialProperties.data() + propertyInfo.second.m_offset), 0.0f, 1.0f));
						}
						else
						{
							updateChange(ImGui::InputFloat(propertyInfo.first.c_str(), reinterpret_cast<float*>(m_materialProperties.data() + propertyInfo.second.m_offset)));
						}
					}
					else if (propertyInfo.second.m_reflectionUI == "Checkbox")
					{
						updateChange(ImGui::Checkbox(propertyInfo.first.c_str(), reinterpret_cast<bool*>(m_materialProperties.data() + propertyInfo.second.m_offset)));
					}
				}
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
				mapRequest.m_byteCount = m_materialProperties.size();
				mapRequest.m_byteOffset = 0;

				RHI::BufferMapResponse mapResponse{};

				RHI::ResultCode mapSuccess = constantBufferPool->MapBuffer(mapRequest, mapResponse);
				if (mapSuccess == RHI::ResultCode::Success)
				{
					memcpy(mapResponse.m_data, m_materialProperties.data(), m_materialProperties.size());
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

		const std::shared_ptr<const RHI::ShaderPermutation> Material::GetShaderPermutation()
		{
			return m_shaderPermutation;
		}

		void Material::InsertProperty(const std::string& name, const PropertyInfo& propertyInfo)
		{
			assert(m_nameToInfoMap.find(name) == m_nameToInfoMap.end(), "The property map should not contain duplicate names. Check your material layout json file.");
			m_nameToInfoMap.insert({ name, propertyInfo });
		}

		const Material::PropertyInfo& Material::GetPropertyInfo(const std::string& name)
		{
			return m_nameToInfoMap[name];
		}

		void Material::InitMaterialProperty(uint32_t totalSizeInBytes)
		{
			m_materialProperties.resize(totalSizeInBytes);
		}
	}
}