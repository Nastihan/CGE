
// DX12
#include "DX_DXShaderCompiler.h"
#include "DX_Conversions.h"

#define STRINGIFY(x) #x
#define EXPAND(x) STRINGIFY(x)

namespace CGE
{
	namespace DX12
	{
		DX_DXShaderCompiler::DX_DXShaderCompiler()
		{
			DXAssertSuccess(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&m_compiler)));
			DXAssertSuccess(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&m_utils)));
			DXAssertSuccess(m_utils->CreateDefaultIncludeHandler(&m_includeHandler));
		}

		IDxcBlob* DX_DXShaderCompiler::Compile(const RHI::ShaderFileInfo& fileInfo)
		{
			assert(m_compiler && m_utils && m_includeHandler);

			std::string projPath = EXPAND(UNITTESTPRJ);
			projPath.erase(0, 1);
			projPath.erase(projPath.size() - 2);
			std::filesystem::path fullFilePath = projPath + "Shaders\\" + fileInfo.m_fileName;
			std::filesystem::path includePath = projPath + "Shaders\\includes\\";

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

			return Compile(sourceBlob.Get(), args, _countof(args));
		}

		IDxcBlob* DX_DXShaderCompiler::Compile(IDxcBlobEncoding* sourceBlob, LPCWSTR* args, uint32_t numArgs)
		{
			DxcBuffer buffer{};
			buffer.Encoding = DXC_CP_ACP;
			buffer.Ptr = sourceBlob->GetBufferPointer();
			buffer.Size = sourceBlob->GetBufferSize();

			wrl::ComPtr<IDxcResult> results{ nullptr };
			DXAssertSuccess(m_compiler->Compile(&buffer, args, numArgs, m_includeHandler.Get(), IID_PPV_ARGS(&results)));

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
			return shader.Detach();
		}
	}
}