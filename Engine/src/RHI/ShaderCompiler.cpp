
// RHI
#include "ShaderCompiler.h"
#include "Graphics.h"

namespace CGE
{
	namespace RHI
	{
		ResultCode ShaderCompiler::Init()
		{
			return InitInternal();
		}

		void ShaderCompiler::Shutdown()
		{
			ShutdownInternal();
		}

		GraphicsShaderCompileResult ShaderCompiler::CompileGraphicsShader(const GraphicsShaderCompileInfo& shaderFileInfo)
		{
			auto& rhiFactory = Graphics::GetFactory();
			GraphicsShaderCompileResult compileResult{};
			compileResult.m_vertexShader = rhiFactory.CreateShaderStageFunction();
			compileResult.m_pixelShader = rhiFactory.CreateShaderStageFunction();
			compileResult.m_pipelineLayoutDescriptor = rhiFactory.CreatePipelineLayoutDescriptor();
			
			CompileGraphicsShaderInternal(shaderFileInfo, compileResult);

			// [todo] Check compile was successful

			return compileResult;
		}
	}
}