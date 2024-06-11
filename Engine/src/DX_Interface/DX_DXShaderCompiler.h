#pragma once

// DX12
#include "DX_CommonHeaders.h"

// RHI
#include "../RHI/ShaderStages.h"

// std
#include <string>
#include <filesystem>

#include <DirectXShaderCompiler/dxcapi.h>
#include <DirectXShaderCompiler/d3d12shader.h>

namespace CGE
{
	namespace DX12
	{
		// [todo] Save the shader byte code to disk.
		class DX_DXShaderCompiler
		{
		public:
			REMOVE_COPY_AND_MOVE(DX_DXShaderCompiler);
			DX_DXShaderCompiler();
			IDxcBlob* Compile(const RHI::ShaderFileInfo& fileInfo);

		private:
			IDxcBlob* Compile(IDxcBlobEncoding* sourceBlob, LPCWSTR* args, uint32_t numArgs);

		private:
			wrl::ComPtr<IDxcCompiler3> m_compiler;
			wrl::ComPtr<IDxcUtils> m_utils;
			wrl::ComPtr<IDxcIncludeHandler> m_includeHandler;
		};
	}
}