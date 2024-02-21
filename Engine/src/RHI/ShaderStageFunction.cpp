
// RHI
#include "ShaderStageFunction.h"

namespace CGE
{
	namespace RHI
	{
		ShaderStage ShaderStageFunction::GetShaderStage() const
		{
			return m_shaderStage;
		}

		ShaderStageFunction::ShaderStageFunction(ShaderStage shaderStage) : m_shaderStage{ shaderStage } {}
	}
}