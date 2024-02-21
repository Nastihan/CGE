
// DX12
#include "DX_ShaderStageFunction.h"
#include "DX_Conversions.h"

#include <d3dcompiler.h>

namespace CGE
{
	namespace DX12
	{
		RHI::Ptr<DX_ShaderStageFunction> DX_ShaderStageFunction::Create(RHI::ShaderStage shaderStage)
		{
			return new DX_ShaderStageFunction(shaderStage);
		}

		void DX_ShaderStageFunction::SetByteCode(std::string filePath)
		{
			DXAssertSuccess(D3DReadFileToBlob(s2ws(filePath).c_str(), &m_shaderBlob));
		}

		ID3DBlob* DX_ShaderStageFunction::GetByteCode() const
		{
			return m_shaderBlob.Get();
		}

		DX_ShaderStageFunction::DX_ShaderStageFunction(RHI::ShaderStage shaderStage) : RHI::ShaderStageFunction(shaderStage) {}
	}
}