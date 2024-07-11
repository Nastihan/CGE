#pragma once

// RHI
#include "ShaderStageFunction.h"
#include "InputStreamLayout.h"
#include "PipelineLayoutDescriptor.h"

namespace CGE
{
	namespace RHI
	{
		struct GraphicsShaderCompileInfo
		{
			std::string m_shaderName;
			ShaderFileInfo m_vertexShaderFileInfo;
			ShaderFileInfo m_pixelShaderFileInfo;
		};

		struct GraphicsShaderCompileResult
		{
			RHI::Ptr<ShaderStageFunction> m_vertexShader;
			RHI::Ptr<ShaderStageFunction> m_pixelShader;
			RHI::InputStreamLayout m_inputStreamLayout;
			RHI::Ptr<PipelineLayoutDescriptor> m_pipelineLayoutDescriptor;
		};

		class ShaderCompiler : public intrusive_base
		{
		public:
			virtual ~ShaderCompiler() = default;

			ResultCode Init();
			void Shutdown();
			GraphicsShaderCompileResult CompileGraphicsShader(const GraphicsShaderCompileInfo& shaderFileInfo);

		protected:
			ShaderCompiler() = default;
		private:
			virtual ResultCode InitInternal() = 0;
			virtual void ShutdownInternal() = 0;
			virtual void CompileGraphicsShaderInternal(const GraphicsShaderCompileInfo& shaderFileInfo, GraphicsShaderCompileResult& compileResult) = 0;
		};
	}
}