
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

		HashValue64 ShaderStageFunction::GetHash() const
		{
			return m_hash;
		}

		void ShaderStageFunction::SetHash(HashValue64 hash)
		{
			m_hash = hash;
		}

		ResultCode ShaderStageFunction::Finalize()
		{
			if (m_shaderStage == ShaderStage::Unknown)
			{
				assert(false, "The shader stage is Unknown. This is not valid.");
				return ResultCode::InvalidArgument;
			}

			m_hash = HashValue64{ 0 };
			const ResultCode resultCode = FinalizeInternal();

			if (resultCode == ResultCode::Success)
			{
				if (m_hash == HashValue64{ 0 })
				{
					assert(false, "The hash value was not assigned in the platform Finalize implementation.");
					return ResultCode::Fail;
				}
			}

			return resultCode;
		}
	}
}