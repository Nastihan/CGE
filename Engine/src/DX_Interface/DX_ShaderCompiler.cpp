
// DX12
#include "DX_ShaderCompiler.h"
#include "DX_Conversions.h"
#include "DX_ShaderStageFunction.h"
#include "DX_PipelineLayoutDescriptor.h"

#include <utility>
#include <ranges>

#define STRINGIFY(x) #x
#define EXPAND(x) STRINGIFY(x)

namespace CGE
{
	namespace DX12
	{
		RHI::Ptr<DX_ShaderCompiler> DX_ShaderCompiler::Create()
		{
			return new DX_ShaderCompiler();
		}

		RHI::ResultCode DX_ShaderCompiler::InitInternal()
		{
			DXAssertSuccess(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&m_compiler)));
			DXAssertSuccess(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&m_utils)));
			DXAssertSuccess(m_utils->CreateDefaultIncludeHandler(&m_includeHandler));
			return RHI::ResultCode::Success;
		}

		void DX_ShaderCompiler::CompileGraphicsShaderInternal(const RHI::GraphicsShaderCompileInfo& graphicsShaderFileInfo, RHI::GraphicsShaderCompileResult& compileResult)
		{
			wrl::ComPtr<IDxcResult> vertexShaderResult{ nullptr };
			wrl::ComPtr<IDxcResult> pixelShaderResult{ nullptr };
			vertexShaderResult = Compile(graphicsShaderFileInfo.m_vertexShaderFileInfo);
			pixelShaderResult = Compile(graphicsShaderFileInfo.m_pixelShaderFileInfo);

			// Get shader reflection data.
			std::array<std::pair<RHI::Ptr<RHI::ShaderResourceGroupLayout>, RHI::ShaderResourceGroupBindingInfo>, RHI::Limits::Pipeline::ShaderResourceGroupCountMax> srgLayouts;
			std::array< RHI::ShaderStageMask, RHI::Limits::Pipeline::ShaderResourceGroupCountMax> srgVisibilityMasks{ RHI::ShaderStageMask::None };
			wrl::ComPtr<IDxcBlob> vertexShaderReflectionBlob;
			wrl::ComPtr<IDxcBlob> pixelShaderReflectionBlob;
			DXAssertSuccess(vertexShaderResult->GetOutput(DXC_OUT_REFLECTION, IID_PPV_ARGS(&vertexShaderReflectionBlob), nullptr));
			DXAssertSuccess(pixelShaderResult->GetOutput(DXC_OUT_REFLECTION, IID_PPV_ARGS(&pixelShaderReflectionBlob), nullptr));
			ReflectShader(vertexShaderReflectionBlob.Get(), compileResult, srgLayouts, srgVisibilityMasks, RHI::ShaderStage::Vertex, graphicsShaderFileInfo.m_shaderName);
			ReflectShader(pixelShaderReflectionBlob.Get(), compileResult, srgLayouts, srgVisibilityMasks, RHI::ShaderStage::Fragment, graphicsShaderFileInfo.m_shaderName);
			for (size_t i = 0; i < RHI::Limits::Pipeline::ShaderResourceGroupCountMax; i++)
			{
				if (srgLayouts[i].first != nullptr)
				{
					srgLayouts[i].first->Finalize();
					compileResult.m_pipelineLayoutDescriptor->AddShaderResourceGroupLayoutInfo(*srgLayouts[i].first, srgLayouts[i].second);

					DX_PipelineLayoutDescriptor& dxPipelineLayoutDesc = static_cast<DX_PipelineLayoutDescriptor&>(*compileResult.m_pipelineLayoutDescriptor);
					DX_ShaderResourceGroupVisibility visibility{};
					visibility.m_descriptorTableShaderStageMask = srgVisibilityMasks[i];
					dxPipelineLayoutDesc.AddShaderResourceGroupVisibility(visibility);
				}
			}
			compileResult.m_pipelineLayoutDescriptor->Finalize(graphicsShaderFileInfo.m_shaderName);

			wrl::ComPtr<IDxcBlob> vertexShader{ nullptr };
			wrl::ComPtr<IDxcBlob> pixelShader{ nullptr };
			DXAssertSuccess(vertexShaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&vertexShader), nullptr));
			DXAssertSuccess(pixelShaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&pixelShader), nullptr));
			static_cast<DX_ShaderStageFunction&>(*compileResult.m_vertexShader.get()).SetByteCode(vertexShader.Detach());
			static_cast<DX_ShaderStageFunction&>(*compileResult.m_pixelShader.get()).SetByteCode(pixelShader.Detach());
		}

		IDxcResult* DX_ShaderCompiler::Compile(const RHI::ShaderFileInfo& fileInfo)
		{
			assert(m_compiler && m_utils && m_includeHandler);

			std::string projPath = EXPAND(UNITTESTPRJ);
			projPath.erase(0, 1);
			projPath.erase(projPath.size() - 2);
			std::filesystem::path fullFilePath = projPath + "Assets\\Shaders\\" + fileInfo.m_fileName;
			std::filesystem::path includePath = projPath + "Assets\\Shaders\\includes\\";

			wrl::ComPtr<IDxcBlobEncoding> sourceBlob{ nullptr };
			DXAssertSuccess(m_utils->LoadFile(fullFilePath.c_str(), nullptr, &sourceBlob));
			assert(sourceBlob && sourceBlob->GetBufferSize());

			std::wstring file{ s2ws(fileInfo.m_fileName) };
			std::wstring func{ s2ws(fileInfo.m_function) };
			std::wstring prof;
			switch (fileInfo.m_stage)
			{
			case RHI::ShaderStage::Vertex:
				prof = L"vs_6_5";
				break;

			case RHI::ShaderStage::Fragment:
				prof = L"ps_6_5";
				break;

			default:
				assert(false, "Shader stage not supported.");
				break;
			}

			// Check https://github.com/microsoft/DirectXShaderCompiler/wiki/Using-dxc.exe-and-dxcompiler.dll for DXC arguments.
			LPCWSTR args[]
			{
				file.c_str(), // Optional shader source file name for error reporting
				L"-E", func.c_str(), // Entry function
				L"-T", prof.c_str(), // Target profile
				L"-I", includePath.c_str(), // Include path
				DXC_ARG_ALL_RESOURCES_BOUND,
#ifdef _DEBUG
				DXC_ARG_DEBUG,
				DXC_ARG_SKIP_OPTIMIZATIONS,
#else
				DXC_ARG_OPTIMIZATION_LEVEL3,
#endif // DEBUG
				DXC_ARG_WARNINGS_ARE_ERRORS,
				L"-Qstrip_reflect", // Strip reflection into a separate blob
				L"-Qstrip_debug", // Strip debug information into separate blob
			};

			DxcBuffer buffer{};
			buffer.Encoding = DXC_CP_ACP;
			buffer.Ptr = sourceBlob->GetBufferPointer();
			buffer.Size = sourceBlob->GetBufferSize();

			wrl::ComPtr<IDxcResult> results{ nullptr };
			DXAssertSuccess(m_compiler->Compile(&buffer, args, _countof(args), m_includeHandler.Get(), IID_PPV_ARGS(&results)));

			wrl::ComPtr<IDxcBlobUtf8> errors{ nullptr };
			DXAssertSuccess(results->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&errors), nullptr));
			if (errors && errors->GetStringLength())
			{
				std::cout << "Shader compilation error: \n" << errors->GetStringPointer();
			}
			else
			{
				std::cout << "Shader compiled successfully." << std::endl;
			}

			HRESULT status{ S_OK };
			DXAssertSuccess(results->GetStatus(&status));
			if (status != S_OK)
			{
				return nullptr;
			}

			wrl::ComPtr<IDxcBlob> shader{ nullptr };
			DXAssertSuccess(results->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shader), nullptr));
			return results.Detach();
		}

		void DX_ShaderCompiler::ShutdownInternal() {}

		void DX_ShaderCompiler::ReflectShader(
			IDxcBlob* reflectionBlob
			, RHI::GraphicsShaderCompileResult& compileResult
			, std::array<std::pair<RHI::Ptr<RHI::ShaderResourceGroupLayout>, RHI::ShaderResourceGroupBindingInfo>, RHI::Limits::Pipeline::ShaderResourceGroupCountMax>& srgLayouts
			, std::array<RHI::ShaderStageMask, RHI::Limits::Pipeline::ShaderResourceGroupCountMax>& srgVisibilityMasks
			, RHI::ShaderStage stage
			, const std::string& shaderName)
		{
			const DxcBuffer reflectionBuffer
			{
				.Ptr = reflectionBlob->GetBufferPointer(),
				.Size = reflectionBlob->GetBufferSize(),
				.Encoding = 0,
			};

			wrl::ComPtr<ID3D12ShaderReflection> shaderReflection{};
			m_utils->CreateReflection(&reflectionBuffer, IID_PPV_ARGS(&shaderReflection));
			D3D12_SHADER_DESC shaderDesc{};
			shaderReflection->GetDesc(&shaderDesc);

			// Build the input stream layout.
			if (stage == RHI::ShaderStage::Vertex)
			{
				RHI::InputStreamLayout& inputStreamLayout = compileResult.m_inputStreamLayout;

				for (const uint32_t parameterIndex : std::views::iota(0u, shaderDesc.InputParameters))
				{
					D3D12_SIGNATURE_PARAMETER_DESC signatureParameterDesc{};
					shaderReflection->GetInputParameterDesc(parameterIndex, &signatureParameterDesc);

					// For now we need to provide one stream buffer for each vertex attribute. (Packed format vertex attributes)
					RHI::StreamBufferDescriptor streamBufferDesc{};
					streamBufferDesc.m_stepFunction = RHI::StreamStepFunction::PerVertex;
					streamBufferDesc.m_stepRate = 1;
					streamBufferDesc.m_byteStride = RHI::GetFormatSize(MaskToFormat(signatureParameterDesc.Mask));

					RHI::StreamChannelDescriptor streamChannelDesc{};
					streamChannelDesc.m_bufferIndex = parameterIndex;
					streamChannelDesc.m_byteOffset = 0;
					streamChannelDesc.m_format = MaskToFormat(signatureParameterDesc.Mask);
					streamChannelDesc.m_semantic.m_name = std::string(signatureParameterDesc.SemanticName);
					streamChannelDesc.m_semantic.m_index = 0;

					inputStreamLayout.AddStreamBuffer(streamBufferDesc);
					inputStreamLayout.AddStreamChannel(streamChannelDesc);
					// [todo] Not a good place to configure this I have to move it based on how the draw item needs to get drawn.
					inputStreamLayout.SetTopology(RHI::PrimitiveTopology::TriangleList);
					inputStreamLayout.Finalize();
				}
			}

			for (const uint32_t i : std::views::iota(0u, shaderDesc.BoundResources))
			{
				D3D12_SHADER_INPUT_BIND_DESC shaderInputBindDesc{};
				DXAssertSuccess(shaderReflection->GetResourceBindingDesc(i, &shaderInputBindDesc));
				

				std::string resourceName(shaderInputBindDesc.Name);
				uint32_t bindingSlotIdx = GetBindingSlotIdx(resourceName);
				auto& srgLayout = srgLayouts[bindingSlotIdx];
				auto& srgVisibility = srgVisibilityMasks[bindingSlotIdx];
				RHI::ShaderStageMask mask = GetShaderStageMask(stage);

				if (srgLayout.first == nullptr)
				{
					srgLayout.first = RHI::ShaderResourceGroupLayout::Create();
					srgLayout.first->SetBindingSlot(bindingSlotIdx);
					// ShaderName + SrgType
					srgLayout.first->SetName(shaderName + "_" + GetSrgName(resourceName) + "Srg");
				}

				if (srgLayout.second.m_resourcesRegisterMap.find(shaderInputBindDesc.Name) != srgLayout.second.m_resourcesRegisterMap.end())
				{
					RHI::ResourceBindingInfo& bindingInfo = srgLayout.second.m_resourcesRegisterMap[shaderInputBindDesc.Name];
					bindingInfo.m_shaderStageMask |= mask;
					if (RHI::CountBitsSet(static_cast<uint32_t>(bindingInfo.m_shaderStageMask)) > 1u)
					{
						srgVisibility = RHI::ShaderStageMask::All;
					}

					assert(bindingInfo.m_registerId == shaderInputBindDesc.BindPoint && bindingInfo.m_spaceId == shaderInputBindDesc.Space);
					continue;
				}
				else
				{
					RHI::ResourceBindingInfo bindingInfo{};
					std::string bindingInfoName{ shaderInputBindDesc.Name };
					bindingInfo.m_registerId = shaderInputBindDesc.BindPoint;
					bindingInfo.m_spaceId = shaderInputBindDesc.Space;
					bindingInfo.m_shaderStageMask |= GetShaderStageMask(stage);
					srgLayout.second.m_resourcesRegisterMap.insert({ bindingInfoName, bindingInfo });
					srgVisibility |= GetShaderStageMask(stage);
				}

				// For now I'm only binding tables for each srg.
				// Also I will add common static samplers for textures at the end.
				RHI::ShaderInputType shaderInputType = GetShaderInputType(shaderInputBindDesc.Type, shaderInputBindDesc.Dimension);
				if (shaderInputType == RHI::ShaderInputType::Buffer)
				{
					RHI::ShaderInputBufferDescriptor shaderInputBufferDescriptor{};
					shaderInputBufferDescriptor.m_name = std::string(shaderInputBindDesc.Name);
					shaderInputBufferDescriptor.m_access = GetShaderInputBufferAccess(shaderInputBindDesc.Type);
					shaderInputBufferDescriptor.m_type = GetShaderInputBufferType(shaderInputBindDesc.Type, shaderInputBindDesc.Dimension);
					shaderInputBufferDescriptor.m_count = shaderInputBindDesc.BindCount;
					if (shaderInputBufferDescriptor.m_access == RHI::ShaderInputBufferAccess::Constant)
					{
						// [todo] For now I can only get constant buffer size with DXC.
						ID3D12ShaderReflectionConstantBuffer* shaderReflectionConstantBuffer = shaderReflection->GetConstantBufferByIndex(i);
						D3D12_SHADER_BUFFER_DESC constantBufferDesc{};
						shaderReflectionConstantBuffer->GetDesc(&constantBufferDesc);
						shaderInputBufferDescriptor.m_strideSize = constantBufferDesc.Size;
					}
					shaderInputBufferDescriptor.m_registerId = shaderInputBindDesc.BindPoint;
					shaderInputBufferDescriptor.m_spaceId = shaderInputBindDesc.Space;

					srgLayout.first->AddShaderInput(shaderInputBufferDescriptor);
				}
				else if (shaderInputType == RHI::ShaderInputType::Image)
				{
					RHI::ShaderInputImageDescriptor shaderInputImageDescriptor{};
					shaderInputImageDescriptor.m_name = shaderInputBindDesc.Name;
					shaderInputImageDescriptor.m_access = GetShaderInputImageAccess(shaderInputBindDesc.Type);
					shaderInputImageDescriptor.m_type = GetShaderInputImageType(shaderInputBindDesc.Dimension);
					shaderInputImageDescriptor.m_count = shaderInputBindDesc.BindCount;
					shaderInputImageDescriptor.m_registerId = shaderInputBindDesc.BindPoint;
					shaderInputImageDescriptor.m_spaceId = shaderInputBindDesc.Space;

					srgLayout.first->AddShaderInput(shaderInputImageDescriptor);
				}
				else if (shaderInputType == RHI::ShaderInputType::Sampler)
				{
					RHI::ShaderInputStaticSamplerDescriptor shaderInputStaticSamplerDescriptor{};
					shaderInputStaticSamplerDescriptor.m_name = shaderInputBindDesc.Name;
					shaderInputStaticSamplerDescriptor.m_registerId = shaderInputBindDesc.BindPoint;
					shaderInputStaticSamplerDescriptor.m_spaceId = shaderInputBindDesc.Space;
					shaderInputStaticSamplerDescriptor.m_samplerState = GetStaticSamplerState(shaderInputBindDesc.Name);
					srgLayout.first->AddStaticSampler(shaderInputStaticSamplerDescriptor);
				}
				else
				{
					// [todo] Have to add more reflection support.
					assert(false, "Shader input type is unknown. Check the shader file.");
				}
			}
		}

		RHI::Format DX_ShaderCompiler::MaskToFormat(BYTE mask)
		{
			uint8_t bitCount = RHI::CountBitsSet(mask);
			switch (bitCount)
			{
			case 1:
				return RHI::Format::R32_FLOAT;
			case 2:
				return RHI::Format::R32G32_FLOAT;
			case 3:
				return RHI::Format::R32G32B32_FLOAT;
			case 4:
				return RHI::Format::R32G32B32A32_FLOAT;
			default:
				assert(false, "Check the Input Parameters in the shader.");
				break;
			}
			return RHI::Format::Unknown;
		}

		uint32_t DX_ShaderCompiler::GetBindingSlotIdx(const std::string& resourceName)
		{
			std::size_t found = resourceName.find("PerScene");
			if (found != std::string::npos)
			{
				return RHI::SrgBindingSlot::Scene;
			}

			found = resourceName.find("PerView");
			if (found != std::string::npos)
			{
				return RHI::SrgBindingSlot::View;
			}

			found = resourceName.find("PerPass");
			if (found != std::string::npos)
			{
				return RHI::SrgBindingSlot::Pass;
			}

			found = resourceName.find("PerMaterial");
			if (found != std::string::npos)
			{
				return RHI::SrgBindingSlot::Material;
			}

			found = resourceName.find("PerObject");
			if (found != std::string::npos)
			{
				return RHI::SrgBindingSlot::Object;
			}

			found = resourceName.find("PerDraw");
			if (found != std::string::npos)
			{
				return RHI::SrgBindingSlot::Draw;
			}

			assert(false, "Check your resource name in the shader file. All resources should be prefixed with Srg type.");
			return -1;
		}

		std::string DX_ShaderCompiler::GetSrgName(const std::string& resourceName)
		{
			std::size_t found = resourceName.find("PerScene");
			if (found != std::string::npos)
			{
				return "PerScene";
			}

			found = resourceName.find("PerView");
			if (found != std::string::npos)
			{
				return "PerView";
			}

			found = resourceName.find("PerPass");
			if (found != std::string::npos)
			{
				return "PerPass";
			}

			found = resourceName.find("PerMaterial");
			if (found != std::string::npos)
			{
				return "PerMaterial";
			}

			found = resourceName.find("PerObject");
			if (found != std::string::npos)
			{
				return "PerObject";
			}

			found = resourceName.find("PerDraw");
			if (found != std::string::npos)
			{
				return "PerDraw";
			}

			assert(false, "Check your resource name in the shader file. All resources should be prefixed with Srg type.");
			return "";
		}

		RHI::ShaderInputType DX_ShaderCompiler::GetShaderInputType(D3D_SHADER_INPUT_TYPE inputType, D3D_SRV_DIMENSION inputDimention)
		{
			switch (inputType)
			{
			case D3D_SIT_CBUFFER:
			case D3D_SIT_UAV_RWSTRUCTURED:
			case D3D_SIT_UAV_RWBYTEADDRESS:
			case D3D_SIT_UAV_APPEND_STRUCTURED:
			case D3D_SIT_UAV_CONSUME_STRUCTURED:
			case D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER:
			case D3D_SIT_STRUCTURED:
			case D3D_SIT_BYTEADDRESS:
				return RHI::ShaderInputType::Buffer;
				break;

			case D3D_SIT_TEXTURE:
				return (inputDimention == D3D_SRV_DIMENSION_BUFFER ? RHI::ShaderInputType::Buffer : RHI::ShaderInputType::Image);
				break;

			case D3D_SIT_SAMPLER:
				return RHI::ShaderInputType::Sampler;
				break;

			case D3D_SIT_UAV_RWTYPED:
				return (inputDimention == D3D_SRV_DIMENSION_BUFFER ? RHI::ShaderInputType::Buffer : RHI::ShaderInputType::Image);
				break;

			default:
				return RHI::ShaderInputType::Unknown;
				break;
			}
		}

		RHI::ShaderInputBufferAccess DX_ShaderCompiler::GetShaderInputBufferAccess(D3D_SHADER_INPUT_TYPE inputType)
		{
			switch (inputType)
			{
			case D3D_SIT_CBUFFER:
				return RHI::ShaderInputBufferAccess::Constant;
				break;

			case D3D_SIT_TEXTURE:
			case D3D_SIT_STRUCTURED:
			case D3D_SIT_BYTEADDRESS:
				return RHI::ShaderInputBufferAccess::Read;
				break;

			case D3D_SIT_UAV_RWTYPED:
			case D3D_SIT_UAV_RWSTRUCTURED:
			case D3D_SIT_UAV_RWBYTEADDRESS:
			case D3D_SIT_UAV_APPEND_STRUCTURED:
			case D3D_SIT_UAV_CONSUME_STRUCTURED:
			case D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER:
				return RHI::ShaderInputBufferAccess::ReadWrite;
				break;

			default:
				assert(false, "Check your resource type in the shader file.");
				return RHI::ShaderInputBufferAccess::Unknown;
				break;
			}
		}

		RHI::ShaderInputImageAccess DX_ShaderCompiler::GetShaderInputImageAccess(D3D_SHADER_INPUT_TYPE inputType)
		{
			switch (inputType)
			{
			case D3D_SIT_TEXTURE:
				return RHI::ShaderInputImageAccess::Read;
				break;

			case D3D_SIT_UAV_RWTYPED:
				return RHI::ShaderInputImageAccess::ReadWrite;
				break;

			default:
				assert(false, "Check your resource type in the shader file.");
				return RHI::ShaderInputImageAccess::Unknown;
				break;
			}
		}

		// [todo] Need to change.
		RHI::ShaderInputBufferType DX_ShaderCompiler::GetShaderInputBufferType(D3D_SHADER_INPUT_TYPE inputType, D3D_SRV_DIMENSION inputDimention)
		{
			switch (inputType)
			{
			case D3D_SIT_CBUFFER:
				return RHI::ShaderInputBufferType::Constant;
				break;

			case D3D_SIT_STRUCTURED:
			case D3D_SIT_UAV_RWSTRUCTURED:
			case D3D_SIT_UAV_APPEND_STRUCTURED:
			case D3D_SIT_UAV_CONSUME_STRUCTURED:
			case D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER:
				return RHI::ShaderInputBufferType::Structured;
				break;

			case D3D_SIT_BYTEADDRESS:
			case D3D_SIT_UAV_RWBYTEADDRESS:
				return RHI::ShaderInputBufferType::Raw;
				break;

			case D3D_SIT_UAV_RWTYPED:
			case D3D_SIT_TBUFFER:
				return RHI::ShaderInputBufferType::Typed;

			default:
				assert(false, "Check your resource type in the shader file.");
				return RHI::ShaderInputBufferType::Unknown;
				break;
			}
		}

		RHI::ShaderInputImageType DX_ShaderCompiler::GetShaderInputImageType(D3D_SRV_DIMENSION inputDimention)
		{
			switch (inputDimention)
			{
			case D3D_SRV_DIMENSION_TEXTURE1D:
				return RHI::ShaderInputImageType::Image1D;
				break;
			case D3D_SRV_DIMENSION_TEXTURE1DARRAY:
				return RHI::ShaderInputImageType::Image1DArray;
				break;
			case D3D_SRV_DIMENSION_TEXTURE2D:
				return RHI::ShaderInputImageType::Image2D;
				break;
			case D3D_SRV_DIMENSION_TEXTURE2DARRAY:
				return RHI::ShaderInputImageType::Image2DArray;
				break;
			case D3D_SRV_DIMENSION_TEXTURE2DMS:
				return RHI::ShaderInputImageType::Image2DMultisample;
				break;
			case D3D_SRV_DIMENSION_TEXTURE2DMSARRAY:
				return RHI::ShaderInputImageType::Image2DMultisampleArray;
				break;
			case D3D_SRV_DIMENSION_TEXTURE3D:
				return RHI::ShaderInputImageType::Image3D;
				break;
			case D3D_SRV_DIMENSION_TEXTURECUBE:
				return RHI::ShaderInputImageType::ImageCube;
				break;
			case D3D_SRV_DIMENSION_TEXTURECUBEARRAY:
				return RHI::ShaderInputImageType::ImageCubeArray;
				break;
			default:
				assert(false, "Check your resource type in the shader file.");
				return RHI::ShaderInputImageType::Unknown;
			}
		}

		RHI::ShaderStageMask DX_ShaderCompiler::GetShaderStageMask(RHI::ShaderStage stage)
		{
			switch (stage)
			{
			case RHI::ShaderStage::Vertex:
				return RHI::ShaderStageMask::Vertex;
				break;
			case RHI::ShaderStage::Fragment:
				return RHI::ShaderStageMask::Fragment;
				break;
			case RHI::ShaderStage::Compute:
				return RHI::ShaderStageMask::Compute;
				break;
			case RHI::ShaderStage::RayTracing:
				return RHI::ShaderStageMask::RayTracing;
				break;
			default:
				assert(false, "Unknown Stage. Check you .shader file.");
				return RHI::ShaderStageMask::None;
			}
		}

		RHI::SamplerState DX_ShaderCompiler::GetStaticSamplerState(const std::string& str)
		{
			if (str == "PerScene_LinearRepeatSampler")
			{
				return RHI::SamplerState::Create(RHI::FilterMode::Linear, RHI::FilterMode::Linear, RHI::AddressMode::Wrap);
			}
			else if (str == "PerScene_LinearClampSampler")
			{
				return RHI::SamplerState::Create(RHI::FilterMode::Linear, RHI::FilterMode::Linear, RHI::AddressMode::Clamp);
			}
			else
			{
				assert(false, "Sampler added somwhare in shader (only use SamplersCommon.hlsli)");
				return RHI::SamplerState{};
			}
		}
	}
}