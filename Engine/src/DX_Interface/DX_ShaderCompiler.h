#pragma once

// DX12
#include "DX_CommonHeaders.h"

// RHI
#include "../RHI/ShaderStages.h"
#include "../RHI/ShaderCompiler.h"

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
		class DX_ShaderCompiler : public RHI::ShaderCompiler
		{
		public:
			static RHI::Ptr<DX_ShaderCompiler> Create();

		private:
			DX_ShaderCompiler() = default;

			// RHI::ShaderComiler
			RHI::ResultCode InitInternal() override;
			void ShutdownInternal() override;
			void CompileGraphicsShaderInternal(const RHI::GraphicsShaderCompileInfo& graphicsShaderFileInfo, RHI::GraphicsShaderCompileResult& compileResult) override;

			IDxcResult* Compile(const RHI::ShaderFileInfo& fileInfo);

			// shaderName will be the .shader files name I'm using it to append to the unique srg layout names.
			void ReflectShader(
				IDxcBlob* reflectionBlob
				, RHI::GraphicsShaderCompileResult& compileResult
				, std::array<std::pair<RHI::Ptr<RHI::ShaderResourceGroupLayout>, RHI::ShaderResourceGroupBindingInfo>, RHI::Limits::Pipeline::ShaderResourceGroupCountMax>& srgLayouts
				, std::array<RHI::ShaderStageMask, RHI::Limits::Pipeline::ShaderResourceGroupCountMax>& srgVisibilityMasks
				, RHI::ShaderStage stage
				, const std::string& shaderName);

			RHI::Format MaskToFormat(BYTE mask);
			uint32_t GetBindingSlotIdx(const std::string& resourceName);
			std::string GetSrgName(const std::string& resourceName);
			RHI::ShaderInputType GetShaderInputType(D3D_SHADER_INPUT_TYPE inputType, D3D_SRV_DIMENSION inputDimention);
			RHI::ShaderInputBufferAccess GetShaderInputBufferAccess(D3D_SHADER_INPUT_TYPE inputType);
			RHI::ShaderInputImageAccess GetShaderInputImageAccess(D3D_SHADER_INPUT_TYPE inputType);
			RHI::ShaderInputBufferType GetShaderInputBufferType(D3D_SHADER_INPUT_TYPE inputType, D3D_SRV_DIMENSION inputDimention);
			RHI::ShaderInputImageType GetShaderInputImageType(D3D_SRV_DIMENSION inputDimention);
			RHI::ShaderStageMask GetShaderStageMask(RHI::ShaderStage stage);

			// For now each shader permutation can have a bundle of global samplers I have in SamplersCommon.hlsli
			// I did not find a way to get the sampler states from the reflected data in DXC.
			// Engines usually get this from the meta data of an intermediate language that gets compiled to hlsl for example.
			// So if you need samplers in your shaders be sure to include SamplersCommon.hlsli in your shader files, and DO NOT add any yourself.
			// I can use sampler views in a sampler heap and bind it on the c++ side.
			RHI::SamplerState GetStaticSamplerState(const std::string& str);
		private:
			wrl::ComPtr<IDxcCompiler3> m_compiler;
			wrl::ComPtr<IDxcUtils> m_utils;
			wrl::ComPtr<IDxcIncludeHandler> m_includeHandler;
		};
	}
}