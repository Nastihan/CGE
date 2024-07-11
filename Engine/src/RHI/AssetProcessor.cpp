
// RHI
#include "AssetProcessor.h"
#include "ShaderStages.h"
#include "Graphics.h"

#include <iostream>
#include <filesystem>
#include <fstream>

// DXToolKit
// [todo] I dont want RHI level classes to be dependant on a platform specific toolkit
// Need to replace this with a platform independent texture loader later.
#include <DirectXTex/DirectXTex.h>
using namespace DirectX;

// DX12
#include "../DX_Interface/DX_CommonHeaders.h"
#include "../DX_Interface/DX_Conversions.h"

#define STRINGIFY(x) #x
#define EXPAND(x) STRINGIFY(x)

namespace CGE
{
	namespace RHI
	{
		AssetProcessor::AssetProcessor()
		{
			// Build root shader path
			std::string projPath = EXPAND(UNITTESTPRJ);
			projPath.erase(0, 1);
			projPath.erase(projPath.size() - 2);

			m_fullAssetPath = projPath + "Assets";
			m_fullShaderAssetPath = m_fullAssetPath + "\\Shaders";
			m_fullModelAssetPath = m_fullAssetPath + "\\Models";
			m_fullTextureAssetPath = m_fullAssetPath + "\\Textures";
			m_fullMaterialAssetPath = m_fullAssetPath + "\\Materials";

			for (const auto& entry : std::filesystem::directory_iterator(m_fullShaderAssetPath))
			{
				if (entry.is_regular_file() && entry.path().extension() == ".json")
				{
					m_shaderFiles.push_back(entry.path().filename().string());
				}
			}

			for (const auto& entry : std::filesystem::directory_iterator(m_fullMaterialAssetPath))
			{
				if (entry.is_regular_file() && entry.path().extension() == ".json" && (entry.path().filename().string().find("Material.json") != std::string::npos))
				{
					m_materialFiles.push_back(entry.path().filename().string());
				}
				else
				{
					m_materialLayoutFiles.push_back(entry.path().filename().string());
				}
			}
		}

		void AssetProcessor::InitShaderCompiler()
		{
			m_shaderCompiler = RHI::Graphics::GetFactory().CreateShaderCompiler();
			m_shaderCompiler->Init();
		}

		RHI::ResultCode AssetProcessor::BuildShaderPermutations()
		{
			for (const auto& fileName : m_shaderFiles)
			{
				if (m_permutationMap.find(fileName) != m_permutationMap.end())
				{
					continue;
				}
				
				std::string fullShaderFilePath = m_fullShaderAssetPath + "\\" + fileName;
				std::ifstream shaderFile(fullShaderFilePath);
				nlohmann::json shaderFileData = nlohmann::json::parse(shaderFile);

				assert(shaderFileData.contains("Name") && shaderFileData.contains("ShaderPermutationType") && shaderFileData.contains("HardwareQueueClass"));
				std::string permutationName = shaderFileData["Name"];
				std::shared_ptr<ShaderPermutation> permutation = std::make_shared<ShaderPermutation>();

				permutation->m_type = GetShaderPermutationType(shaderFileData["ShaderPermutationType"]);
				permutation->m_queueClass = GetHardwareQueueClass(shaderFileData["HardwareQueueClass"]);

				// Build graphics state
				if (shaderFileData["HardwareQueueClass"] == "Graphics")
				{
					// Render States
					RHI::RenderStates renderStates{};
					if (shaderFileData.contains("RasterState"))
					{
						nlohmann::json jRasterState = shaderFileData["RasterState"];
						SetRasterState(jRasterState, renderStates.m_rasterState);
					}
					if (shaderFileData.contains("DepthStencilState"))
					{
						nlohmann::json jDepthStencilState = shaderFileData["DepthStencilState"];
						SetDepthStencilState(jDepthStencilState, renderStates.m_depthStencilState);
					}
					if (shaderFileData.contains("BlendState"))
					{
						nlohmann::json jBlendState = shaderFileData["BlendState"];
						SetBlendState(jBlendState, renderStates.m_blendState.m_targets[0]);
					}
					permutation->m_renderState = renderStates;

					// Shader reflection
					RHI::GraphicsShaderCompileInfo shaderCompileInfo{};
					shaderCompileInfo.m_shaderName = shaderFileData["Name"];

					RHI::GraphicsShaderCompileResult shaderCompileResult{};
					assert(shaderFileData.contains("ProgramSettings"), "No shader stage info specified.");
					if (shaderFileData.contains("ProgramSettings"))
					{
						nlohmann::json jProgramSettings = shaderFileData["ProgramSettings"];
						assert(jProgramSettings.contains("ShaderStages"), "No shader stage info specified.");
						if (jProgramSettings.contains("ShaderStages"))
						{
							for (const auto& jShaderStage : jProgramSettings["ShaderStages"])
							{
								if (jShaderStage["type"] == "Vertex")
								{
									shaderCompileInfo.m_vertexShaderFileInfo.m_fileName = jShaderStage["source"];
									shaderCompileInfo.m_vertexShaderFileInfo.m_function = jShaderStage["name"];
									shaderCompileInfo.m_vertexShaderFileInfo.m_stage = GetShaderStage(jShaderStage["type"]);
								}
								else if (jShaderStage["type"] == "Fragment")
								{
									shaderCompileInfo.m_pixelShaderFileInfo.m_fileName = jShaderStage["source"];
									shaderCompileInfo.m_pixelShaderFileInfo.m_function = jShaderStage["name"];
									shaderCompileInfo.m_pixelShaderFileInfo.m_stage = GetShaderStage(jShaderStage["type"]);
								}
							}
							shaderCompileResult = m_shaderCompiler->CompileGraphicsShader(shaderCompileInfo);
						}
						permutation->m_inputStreamLayout = shaderCompileResult.m_inputStreamLayout;
						permutation->m_vertexShader = shaderCompileResult.m_vertexShader;
						permutation->m_pixelShader = shaderCompileResult.m_pixelShader;
						permutation->m_pipelineLayoutDescriptor = shaderCompileResult.m_pipelineLayoutDescriptor;
					}
				}
				m_permutationMap.insert({ permutationName, permutation });
			}
			return ResultCode::Success;
		}

		RHI::ResultCode AssetProcessor::BuildMaterialLayouts()
		{
			for (const auto& fileName : m_materialLayoutFiles)
			{
				if (m_materialLayouts.find(fileName) != m_materialLayouts.end())
				{
					continue;
				}
				std::string fullMaterialLayoutFilePath = m_fullMaterialAssetPath + "\\" + fileName;
				std::ifstream materialLayoutFile(fullMaterialLayoutFilePath);
				nlohmann::json materialLayoutFileData = nlohmann::json::parse(materialLayoutFile);

				assert(materialLayoutFileData.contains("Name") && materialLayoutFileData.contains("ShaderName") && materialLayoutFileData.contains("PropertyLayout"));
				if (materialLayoutFileData.contains("Name"))
				{
					m_materialLayouts.insert({ materialLayoutFileData["Name"], std::make_shared<Scene::Material>() });
				}

				Scene::Material& material = *m_materialLayouts[materialLayoutFileData["Name"]];

				if (materialLayoutFileData.contains("ShaderName"))
				{
					material.SetShaderPermutation(m_permutationMap[materialLayoutFileData["ShaderName"]]);
				}

				if (materialLayoutFileData.contains("PropertyLayout"))
				{
					uint32_t offset = 0;
					uint32_t totalMaterialPropertySizeInBytes = 0;
					uint32_t packing = 0;
					for (const auto& jPropertyLayout : materialLayoutFileData["PropertyLayout"])
					{
						Scene::Material::PropertyInfo propertyInfo;
						std::string propertyName;

						assert(jPropertyLayout.contains("Name") && jPropertyLayout.contains("Type"));

						if (jPropertyLayout.contains("Name"))
						{
							propertyName = jPropertyLayout["Name"];
						}
						if (jPropertyLayout.contains("Type"))
						{
							propertyInfo.m_type = jPropertyLayout["Type"];
							uint32_t propertyTypeSizeInBytes = GetNumberOfBytes(jPropertyLayout["Type"]);
							packing += propertyTypeSizeInBytes;

							// Check if we need padding
							if (packing >= 16)
							{
								uint32_t padding = 16 - packing;
								totalMaterialPropertySizeInBytes += padding;
								offset += padding;
								packing = 0;
							}
							propertyInfo.m_offset = offset;
							offset += propertyTypeSizeInBytes;
							totalMaterialPropertySizeInBytes += propertyTypeSizeInBytes;

							if (jPropertyLayout.contains("ReflectionUI"))
							{
								propertyInfo.m_reflectionUI = jPropertyLayout["ReflectionUI"];
								if (jPropertyLayout.contains("MinValue"))
								{
									propertyInfo.m_reflectionMinMax.first = jPropertyLayout["MinValue"];
								}
								if (jPropertyLayout.contains("MaxValue"))
								{
									propertyInfo.m_reflectionMinMax.second = jPropertyLayout["MaxValue"];
								}
							}
						}
						material.InsertProperty(propertyName, propertyInfo);
					}
					uint32_t endPadding = totalMaterialPropertySizeInBytes % 16;
					material.InitMaterialProperty(totalMaterialPropertySizeInBytes + endPadding);
					for (const auto& jPropertyLayout : materialLayoutFileData["PropertyLayout"])
					{
						if (jPropertyLayout.contains("Name") && jPropertyLayout.contains("Type") && jPropertyLayout.contains("DefaultValue"))
						{
							std::string name = jPropertyLayout["Name"];
							std::string type = jPropertyLayout["Type"];

							if (type == "float4")
							{
								std::vector<float> values = jPropertyLayout["DefaultValue"];
								glm::vec4 value(values[0], values[1], values[2], values[3]);

								material.SetProperty<glm::vec4>(name, value);
							}
							else if (type == "float3")
							{
								std::vector<float> values = jPropertyLayout["DefaultValue"];
								glm::vec3 value(values[0], values[1], values[2]);

								material.SetProperty<glm::vec3>(name, value);
							}
							else if (type == "float2")
							{
								std::vector<float> values = jPropertyLayout["DefaultValue"];
								glm::vec2 value(values[0], values[1]);

								material.SetProperty<glm::vec2>(name, value);
							}
							else if (type == "float")
							{
								float value = jPropertyLayout["DefaultValue"];
								material.SetProperty<float>(name, value);
							}
							else if (type == "bool")
							{
								float value = jPropertyLayout["DefaultValue"] == "true" ? 1.0 : 0.0;
								material.SetProperty<uint32_t>(name, value);
							}
							else
							{
								assert(false, "Unknown type");
							}
						}
					}
				}
			}
			return RHI::ResultCode::Success;
		}

		RHI::ResultCode AssetProcessor::BuildMaterials()
		{
			for (const auto& fileName : m_materialFiles)
			{
				if (m_materials.find(fileName) != m_materials.end())
				{
					continue;
				}
				std::string fullMaterialFilePath = m_fullMaterialAssetPath + "\\" + fileName;
				std::ifstream materialFile(fullMaterialFilePath);
				nlohmann::json materialFileData = nlohmann::json::parse(materialFile);

				assert(materialFileData.contains("Name") && materialFileData.contains("MaterialLayout"));
				if (materialFileData.contains("Name"))
				{
					m_materials.insert({ materialFileData["Name"], std::make_shared<Scene::Material>() });
				}
				Scene::Material& material = *m_materials[materialFileData["Name"]];

				if (materialFileData.contains("MaterialLayout"))
				{
					material = *m_materialLayouts[materialFileData["MaterialLayout"]];
				}

				if (materialFileData.contains("Properties"))
				{
					for (const auto& jProperty : materialFileData["Properties"])
					{
						assert(jProperty.contains("Name") && jProperty.contains("Value"));
						if (jProperty.contains("Name") && jProperty.contains("Value"))
						{
							std::string propertyName = jProperty["Name"];
							const auto& propertyInfo = material.GetPropertyInfo(propertyName);
							if (propertyInfo.m_type == "float4")
							{
								std::vector<float> values = jProperty["Value"];
								glm::vec4 value(values[0], values[1], values[2], values[3]);

								material.SetProperty<glm::vec4>(propertyName, value);
							}
							else if (propertyInfo.m_type == "float3")
							{
								std::vector<float> values = jProperty["Value"];
								glm::vec3 value(values[0], values[1], values[2]);

								material.SetProperty<glm::vec3>(propertyName, value);
							}
							else if (propertyInfo.m_type == "float2")
							{
								std::vector<float> values = jProperty["Value"];
								glm::vec2 value(values[0], values[1]);

								material.SetProperty<glm::vec2>(propertyName, value);
							}
							else if (propertyInfo.m_type == "float")
							{
								float value = jProperty["Value"];
								material.SetProperty<float>(propertyName, value);
							}
							else if (propertyInfo.m_type == "bool")
							{
								auto pro = jProperty["Value"];
								float value = jProperty["Value"] == "true" ? 1.0 : 0.0;
								material.SetProperty<uint32_t>(propertyName, value);
							}
							else
							{
								assert(false, "Unknown type");
							}
						}
					}
				}

				if (materialFileData.contains("Textures"))
				{
					const auto& materialSrgLayout = material.GetShaderPermutation()->m_pipelineLayoutDescriptor->GetShaderResourceGroupLayout(RHI::ShaderResourceGroupType::Material);
					for (const auto& jTextures : materialFileData["Textures"])
					{
						assert(jTextures.contains("Name") && jTextures.contains("File"));
						if (jTextures.contains("Name") && jTextures.contains("File"))
						{
							RHI::ShaderInputImageIndex inputIdx = materialSrgLayout->FindShaderInputImageIndex(jTextures["Name"]);
							RHI::ShaderInputImageDescriptor inputImageDesc = materialSrgLayout->GetShaderInput(inputIdx);

							// [todo] Add all other types.
							if (inputImageDesc.m_type == RHI::ShaderInputImageType::Image2D)
							{
								std::string name = jTextures["Name"];
								std::string fileName = jTextures["File"];
								RHI::Ptr<RHI::Image> image = CreateTexture2D(fileName);

								RHI::ImageDescriptor imageDesc = image->GetDescriptor();
								RHI::Ptr<RHI::ImageView> imageView = RHI::Graphics::GetFactory().CreateImageView();
								RHI::ImageViewDescriptor imageViewDesc = RHI::ImageViewDescriptor::Create(imageDesc.m_format, 0, 0);
								imageView->Init(*image, imageViewDesc);

								material.SetTexture(name, image, imageView);
							}
						}
					}
				}
				material.InitMaterialCbuff();
				material.InitMaterialSrg();
			}
			return RHI::ResultCode::Success;
		}

		const std::shared_ptr<const ShaderPermutation> AssetProcessor::GetShaderPermutation(const std::string& name) const
		{
			if (m_permutationMap.find(name) == m_permutationMap.end())
			{
				return nullptr;
			}
			else
			{
				// AssetProcessor::GetShaderPermutation so we use .at(name)
				return std::shared_ptr<const ShaderPermutation>(m_permutationMap.at(name));
			}
		}

		const std::shared_ptr<const Scene::Material> AssetProcessor::GetMaterialLayout(const std::string& name) const
		{
			if (m_materialLayouts.find(name) == m_materialLayouts.end())
			{
				return nullptr;
			}
			else
			{
				// AssetProcessor::GetShaderPermutation so we use .at(name)
				return std::shared_ptr<const Scene::Material>(m_materialLayouts.at(name));
			}
		}

		const std::shared_ptr<Scene::Material> AssetProcessor::GetMaterial(const std::string& name) const
		{
			if (m_materials.find(name) == m_materials.end())
			{
				return nullptr;
			}
			else
			{
				// AssetProcessor::GetShaderPermutation so we use .at(name)
				return std::shared_ptr<Scene::Material>(m_materials.at(name));
			}
		}

		RHI::Ptr<RHI::Image> AssetProcessor::CreateTexture2D(const std::string& fileName)
		{
			RHI::Ptr<RHI::Image> texture;

			TexMetadata metaData;
			ScratchImage scratchImage;
			std::string fullPath = m_fullTextureAssetPath + "\\" + fileName;
			DX12::DXAssertSuccess(LoadFromWICFile(DX12::s2ws(fullPath).c_str(), WIC_FLAGS_FORCE_RGB, &metaData, scratchImage));

			texture = RHI::Graphics::GetFactory().CreateImage();
			const auto& imagePool = RHI::Graphics::GetImageSystem().GetSimpleImagePool();
			RHI::ResultCode result = RHI::ResultCode::Fail;

			// Init the image
			RHI::ImageInitRequest imageInitRequest;
			imageInitRequest.m_image = texture.get();
			imageInitRequest.m_descriptor = RHI::ImageDescriptor::Create2D(RHI::ImageBindFlags::ShaderRead,
				static_cast<uint32_t>(metaData.width),
				static_cast<uint32_t>(metaData.width),
				DX12::ConvertFormat(metaData.format));
			result = imagePool->InitImage(imageInitRequest);
			assert(result == RHI::ResultCode::Success);

			// Update the image contents with the scratch image
			RHI::ImageUpdateRequest imageUpdateRequest;
			imageUpdateRequest.m_image = texture.get();
			imageUpdateRequest.m_sourceSubresourceLayout = RHI::GetImageSubresourceLayout(texture->GetDescriptor(), RHI::ImageSubresource{});
			imageUpdateRequest.m_sourceData = scratchImage.GetImages()[0].pixels;
			result = imagePool->UpdateImageContents(imageUpdateRequest);
			assert(result == RHI::ResultCode::Success);

			return texture;
			RHI::ImageDescriptor imageDesc = texture->GetDescriptor();
			RHI::Ptr<RHI::ImageView> imageView = RHI::Graphics::GetFactory().CreateImageView();
			RHI::ImageViewDescriptor imageViewDesc = RHI::ImageViewDescriptor::Create(imageDesc.m_format, 0, 0);
			imageView->Init(*texture, imageViewDesc);
		}

		void AssetProcessor::SetRasterState(const nlohmann::json& jRasterState, RHI::RasterState& rasterState)
		{
			if (jRasterState.contains("FillMode"))
			{
				std::string fillModeStr = jRasterState["FillMode"];
				RHI::FillMode fillMode = GetFillMode(fillModeStr);
				assert(fillMode != RHI::FillMode::Invalid, "Error in json file for RasterState::m_fillMode");
				rasterState.m_fillMode = fillMode;
			}

			if (jRasterState.contains("CullMode"))
			{
				std::string cullModeStr = jRasterState["CullMode"];
				RHI::CullMode cullMode = GetCullMode(cullModeStr);
				assert(cullMode != RHI::CullMode::Invalid, "Error in json file for RasterState::m_cullMode");
				rasterState.m_cullMode = cullMode;
			}

			if (jRasterState.contains("DepthBias"))
			{
				rasterState.m_depthBias = jRasterState["DepthBias"];
			}

			if (jRasterState.contains("DepthBiasClamp"))
			{
				rasterState.m_depthBiasClamp = jRasterState["DepthBiasClamp"];
			}

			if (jRasterState.contains("DepthBiasSlopeScale"))
			{
				rasterState.m_depthBiasSlopeScale = jRasterState["DepthBiasSlopeScale"];
			}

			if (jRasterState.contains("MultisampleEnable"))
			{
				rasterState.m_multisampleEnable = jRasterState["MultisampleEnable"] ? 1 : 0;
			}

			if (jRasterState.contains("DepthClipEnable"))
			{
				rasterState.m_depthClipEnable = jRasterState["DepthClipEnable"] ? 1 : 0;
			}

			if (jRasterState.contains("ConservativeRasterEnable"))
			{
				rasterState.m_conservativeRasterEnable = jRasterState["ConservativeRasterEnable"] ? 1 : 0;
			}

			if (jRasterState.contains("ForcedSampleCount"))
			{
				rasterState.m_forcedSampleCount = jRasterState["ForcedSampleCount"];
			}
		}

		void AssetProcessor::SetDepthStencilState(const nlohmann::json& jDepthStencilState, RHI::DepthStencilState& depthStencilState)
		{
			if (jDepthStencilState.contains("Depth"))
			{
				nlohmann::json jDepth = jDepthStencilState["Depth"];
				
				if (jDepth.contains("Enable"))
				{
					depthStencilState.m_depth.m_enable = jDepth["Enable"] ? 1 : 0;
				}
				
				if (jDepth.contains("WriteMask"))
				{
					std::string writeMaskStr = jDepth["WriteMask"];
					RHI::DepthWriteMask writeMask = GetDepthWriteMask(writeMaskStr);
					assert(writeMask != RHI::DepthWriteMask::Invalid, "Error in json file for DepthStencilState::m_depth::m_writeMask");
					depthStencilState.m_depth.m_writeMask = writeMask;
				}
				
				if (jDepth.contains("CompareFunc"))
				{
					std::string compareFuncStr = jDepth["CompareFunc"];
					RHI::ComparisonFunc compareFunc = GetComparisonFunc(compareFuncStr);
					assert(compareFunc != RHI::ComparisonFunc::Invalid, "Error in json file for DepthStencilState::m_depth::m_func");
					depthStencilState.m_depth.m_func = compareFunc;
				}
			}

			if (jDepthStencilState.contains("Stencil"))
			{
				nlohmann::json jStencil = jDepthStencilState["Stencil"];

				if (jStencil.contains("Enable"))
				{
					depthStencilState.m_stencil.m_enable = jStencil["Enable"];
				}

				if (jStencil.contains("ReadMask"))
				{
					std::string hexString = jStencil["ReadMask"];
					depthStencilState.m_stencil.m_readMask = std::stoul(hexString, nullptr, 16);
				}

				if (jStencil.contains("WriteMask"))
				{
					std::string hexString = jStencil["WriteMask"];
					depthStencilState.m_stencil.m_writeMask = std::stoul(hexString, nullptr, 16);
				}

				if (jStencil.contains("FrontFace"))
				{
					nlohmann::json jStencilOpState = jStencil["FrontFace"];
					
					if (jStencilOpState.contains("FailOp"))
					{
						std::string failOpStr = jStencilOpState["FailOp"];
						RHI::StencilOp failOp = GetStencilOp(failOpStr);
						assert(failOp != RHI::StencilOp::Invalid, "Error in json file for DepthStencilState::m_stencil::m_frontFace::m_failOp");
						depthStencilState.m_stencil.m_frontFace.m_failOp = failOp;
					}

					if (jStencilOpState.contains("DepthFailOp"))
					{
						std::string failOpStr = jStencilOpState["DepthFailOp"];
						RHI::StencilOp failOp = GetStencilOp(failOpStr);
						assert(failOp != RHI::StencilOp::Invalid, "Error in json file for DepthStencilState::m_stencil::m_frontFace::m_depthFailOp");
						depthStencilState.m_stencil.m_frontFace.m_depthFailOp = failOp;
					}

					if (jStencilOpState.contains("PassOp"))
					{
						std::string passOpStr = jStencilOpState["PassOp"];
						RHI::StencilOp passOp = GetStencilOp(passOpStr);
						assert(passOp != RHI::StencilOp::Invalid, "Error in json file for DepthStencilState::m_stencil::m_frontFace::m_passOp");
						depthStencilState.m_stencil.m_frontFace.m_passOp = passOp;
					}

					if (jStencilOpState.contains("Func"))
					{
						std::string compareFuncStr = jStencilOpState["Func"];
						RHI::ComparisonFunc compareFunc = GetComparisonFunc(compareFuncStr);
						assert(compareFunc != RHI::ComparisonFunc::Invalid, "Error in json file for DepthStencilState::m_stencil::m_frontFace::m_func");
						depthStencilState.m_stencil.m_frontFace.m_func = compareFunc;
					}
				}

				if (jStencil.contains("BackFace"))
				{
					nlohmann::json jStencilOpState = jStencil["BackFace"];

					if (jStencilOpState.contains("FailOp"))
					{
						std::string failOpStr = jStencilOpState["FailOp"];
						RHI::StencilOp failOp = GetStencilOp(failOpStr);
						assert(failOp != RHI::StencilOp::Invalid, "Error in json file for DepthStencilState::m_stencil::m_backFace::m_failOp");
						depthStencilState.m_stencil.m_backFace.m_failOp = failOp;
					}

					if (jStencilOpState.contains("DepthFailOp"))
					{
						std::string failOpStr = jStencilOpState["DepthFailOp"];
						RHI::StencilOp failOp = GetStencilOp(failOpStr);
						assert(failOp != RHI::StencilOp::Invalid, "Error in json file for DepthStencilState::m_stencil::m_backFace::m_depthFailOp");
						depthStencilState.m_stencil.m_backFace.m_depthFailOp = failOp;
					}

					if (jStencilOpState.contains("PassOp"))
					{
						std::string passOpStr = jStencilOpState["PassOp"];
						RHI::StencilOp passOp = GetStencilOp(passOpStr);
						assert(passOp != RHI::StencilOp::Invalid, "Error in json file for DepthStencilState::m_stencil::m_backFace::m_passOp");
						depthStencilState.m_stencil.m_backFace.m_passOp = passOp;
					}

					if (jStencilOpState.contains("Func"))
					{
						std::string compareFuncStr = jStencilOpState["Func"];
						RHI::ComparisonFunc compareFunc = GetComparisonFunc(compareFuncStr);
						assert(compareFunc != RHI::ComparisonFunc::Invalid, "Error in json file for DepthStencilState::m_stencil::m_backFace::m_func");
						depthStencilState.m_stencil.m_backFace.m_func = compareFunc;
					}
				}
			}
		}

		void AssetProcessor::SetBlendState(nlohmann::json jBlendState, RHI::TargetBlendState& blendState)
		{
			if (jBlendState.contains("Enable"))
			{
				blendState.m_enable = jBlendState["Enable"] ? 1 : 0;
			}

			if (jBlendState.contains("writeMask"))
			{
				std::string hexString = jBlendState["writeMask"];
				blendState.m_writeMask = std::stoul(hexString, nullptr, 16);
			}

			if (jBlendState.contains("BlendSource"))
			{
				std::string blendFactorStr = jBlendState["BlendSource"];
				RHI::BlendFactor blendFactor = GetBlendFactor(blendFactorStr);
				assert(blendFactor != RHI::BlendFactor::Invalid, "Error in json file for TargetBlendState::m_blendSource");
				blendState.m_blendSource = blendFactor;
			}

			if (jBlendState.contains("BlendDest"))
			{
				std::string blendFactorStr = jBlendState["BlendDest"];
				RHI::BlendFactor blendFactor = GetBlendFactor(blendFactorStr);
				assert(blendFactor != RHI::BlendFactor::Invalid, "Error in json file for TargetBlendState::m_blendDest");
				blendState.m_blendDest = blendFactor;
			}

			if (jBlendState.contains("BlendOp"))
			{
				std::string blendOpStr = jBlendState["BlendOp"];
				RHI::BlendOp blendOp = GetBlendOp(blendOpStr);
				assert(blendOp != RHI::BlendOp::Invalid, "Error in json file for TargetBlendState::m_blendOp");
				blendState.m_blendOp = blendOp;
			}

			if (jBlendState.contains("BlendAlphaSource"))
			{
				std::string blendFactorStr = jBlendState["BlendAlphaSource"];
				RHI::BlendFactor blendFactor = GetBlendFactor(blendFactorStr);
				assert(blendFactor != RHI::BlendFactor::Invalid, "Error in json file for TargetBlendState::m_blendAlphaSource");
				blendState.m_blendAlphaSource = blendFactor;
			}

			if (jBlendState.contains("BlendAlphaDest"))
			{
				std::string blendFactorStr = jBlendState["BlendAlphaDest"];
				RHI::BlendFactor blendFactor = GetBlendFactor(blendFactorStr);
				assert(blendFactor != RHI::BlendFactor::Invalid, "Error in json file for TargetBlendState::m_blendAlphaDest");
				blendState.m_blendAlphaDest = blendFactor;
			}

			if (jBlendState.contains("BlendAlphaOp"))
			{
				std::string blendOpStr = jBlendState["BlendAlphaOp"];
				RHI::BlendOp blendOp = GetBlendOp(blendOpStr);
				assert(blendOp != RHI::BlendOp::Invalid, "Error in json file for TargetBlendState::m_blendAlphaOp");
				blendState.m_blendAlphaOp = blendOp;
			}
		}

		void AssetProcessor::CompileAndSetShaderStates(nlohmann::json jProgramSettings)
		{

		}

		RHI::FillMode AssetProcessor::GetFillMode(const std::string& str)
		{
			if (str == "Solid")
			{
				return RHI::FillMode::Solid;
			}
			else if (str == "Wireframe")
			{
				return RHI::FillMode::Wireframe;
			}
			else
			{
				return RHI::FillMode::Invalid;
			}
		}

		RHI::CullMode AssetProcessor::GetCullMode(const std::string& str)
		{
			if (str == "None")
			{
				return RHI::CullMode::None;
			}
			else if (str == "Front")
			{
				return RHI::CullMode::Front;
			}
			else if (str == "Back")
			{
				return RHI::CullMode::Back;
			}
			else
			{
				return RHI::CullMode::Invalid;
			}
		}

		RHI::DepthWriteMask AssetProcessor::GetDepthWriteMask(const std::string& str)
		{
			if (str == "Zero")
			{
				return RHI::DepthWriteMask::Zero;
			}
			else if (str == "All")
			{
				return RHI::DepthWriteMask::All;
			}
			else
			{
				return RHI::DepthWriteMask::Invalid;
			}
		}

		RHI::ComparisonFunc AssetProcessor::GetComparisonFunc(const std::string& str)
		{
			if (str == "Never")
			{
				return RHI::ComparisonFunc::Never;
			}
			else if (str == "Less")
			{
				return RHI::ComparisonFunc::Less;
			}
			else if (str == "Equal")
			{
				return RHI::ComparisonFunc::Equal;
			}
			else if (str == "LessEqual")
			{
				return RHI::ComparisonFunc::LessEqual;
			}
			else if (str == "Greater")
			{
				return RHI::ComparisonFunc::Greater;
			}
			else if (str == "NotEqual")
			{
				return RHI::ComparisonFunc::NotEqual;
			}
			else if (str == "GreaterEqual")
			{
				return RHI::ComparisonFunc::GreaterEqual;
			}
			else if (str == "Always")
			{
				return RHI::ComparisonFunc::Always;
			}
			else
			{
				return RHI::ComparisonFunc::Invalid;
			}
		}

		RHI::StencilOp AssetProcessor::GetStencilOp(const std::string& str)
		{
			if (str == "Keep")
			{
				return RHI::StencilOp::Keep;
			}
			else if (str == "Zero")
			{
				return RHI::StencilOp::Zero;
			}
			else if (str == "Replace")
			{
				return RHI::StencilOp::Replace;
			}
			else if (str == "IncrementSaturate")
			{
				return RHI::StencilOp::IncrementSaturate;
			}
			else if (str == "DecrementSaturate")
			{
				return RHI::StencilOp::DecrementSaturate;
			}
			else if (str == "Invert")
			{
				return RHI::StencilOp::Invert;
			}
			else if (str == "Increment")
			{
				return RHI::StencilOp::Increment;
			}
			else if (str == "Decrement")
			{
				return RHI::StencilOp::Decrement;
			}
			else
			{
				return RHI::StencilOp::Invalid;
			}
		}

		RHI::BlendFactor AssetProcessor::GetBlendFactor(const std::string& str)
		{
			if (str == "Zero")
			{
				return RHI::BlendFactor::Zero;
			}
			else if (str == "One")
			{
				return RHI::BlendFactor::One;
			}
			else if (str == "ColorSource")
			{
				return RHI::BlendFactor::ColorSource;
			}
			else if (str == "ColorSourceInverse")
			{
				return RHI::BlendFactor::ColorSourceInverse;
			}
			else if (str == "AlphaSource")
			{
				return RHI::BlendFactor::AlphaSource;
			}
			else if (str == "AlphaSourceInverse")
			{
				return RHI::BlendFactor::AlphaSourceInverse;
			}
			else if (str == "AlphaDest")
			{
				return RHI::BlendFactor::AlphaDest;
			}
			else if (str == "AlphaDestInverse")
			{
				return RHI::BlendFactor::AlphaDestInverse;
			}
			else if (str == "ColorDest")
			{
				return RHI::BlendFactor::ColorDest;
			}
			else if (str == "ColorDestInverse")
			{
				return RHI::BlendFactor::ColorDestInverse;
			}
			else if (str == "AlphaSourceSaturate")
			{
				return RHI::BlendFactor::AlphaSourceSaturate;
			}
			else if (str == "Factor")
			{
				return RHI::BlendFactor::Factor;
			}
			else if (str == "FactorInverse")
			{
				return RHI::BlendFactor::FactorInverse;
			}
			else if (str == "ColorSource1")
			{
				return RHI::BlendFactor::ColorSource1;
			}
			else if (str == "ColorSource1Inverse")
			{
				return RHI::BlendFactor::ColorSource1Inverse;
			}
			else if (str == "AlphaSource1")
			{
				return RHI::BlendFactor::AlphaSource1;
			}
			else if (str == "AlphaSource1Inverse")
			{
				return RHI::BlendFactor::AlphaSource1Inverse;
			}
			else
			{
				return RHI::BlendFactor::Invalid;
			}
		}

		RHI::BlendOp AssetProcessor::GetBlendOp(const std::string& str)
		{
			if (str == "Add")
			{
				return RHI::BlendOp::Add;
			}
			else if (str == "Subtract")
			{
				return RHI::BlendOp::Subtract;
			}
			else if (str == "SubtractReverse")
			{
				return RHI::BlendOp::SubtractReverse;
			}
			else if (str == "Minimum")
			{
				return RHI::BlendOp::Minimum;
			}
			else if (str == "Maximum")
			{
				return RHI::BlendOp::Maximum;
			}
			else
			{
				return RHI::BlendOp::Invalid;
			}
		}

		RHI::ShaderStage AssetProcessor::GetShaderStage(const std::string& str)
		{
			if (str == "Vertex")
			{
				return RHI::ShaderStage::Vertex;
			}
			else if (str == "Fragment")
			{
				return RHI::ShaderStage::Fragment;
			}
			else if (str == "Compute")
			{
				return RHI::ShaderStage::Compute;
			}
			else if (str == "RayTracing")
			{
				return RHI::ShaderStage::RayTracing;
			}
			else
			{
				return RHI::ShaderStage::Unknown;
			}
		}

		RHI::ShaderPermutationType AssetProcessor::GetShaderPermutationType(const std::string& str)
		{
			if (str == "Engine")
			{
				return RHI::ShaderPermutationType::Engine;
			}
			else if (str == "Material")
			{
				return RHI::ShaderPermutationType::Material;
			}
			else
			{
				assert(false, "Unknown permutation type. Check your .shader file.");
				return RHI::ShaderPermutationType::Unknown;
			}
		}

		RHI::HardwareQueueClass AssetProcessor::GetHardwareQueueClass(const std::string& str)
		{
			if (str == "Graphics")
			{
				return RHI::HardwareQueueClass::Graphics;
			}
			else if (str == "Compute")
			{
				return RHI::HardwareQueueClass::Compute;
			}
			else if (str == "Copy")
			{
				return RHI::HardwareQueueClass::Copy;
			}
			else
			{
				assert(false, "Unknown HardwareQueueClass. Check your .shader file.");
				return RHI::HardwareQueueClass::Unknown;
			}
		}

		uint32_t AssetProcessor::GetNumberOfBytes(const std::string& type)
		{
			if (type == "float4")
			{
				return 16;
			}
			else if (type == "float3")
			{
				return 12;
			}
			else if (type == "float2")
			{
				return 8;
			}
			else if (type == "float")
			{
				return 4;
			}
			else if (type == "bool")
			{
				return 4;
			}
			else
			{
				assert(false, "Unknown type");
				return -1;
			}
		}
	}
}