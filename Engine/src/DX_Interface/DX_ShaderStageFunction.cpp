
// DX12
#include "DX_ShaderStageFunction.h"
#include "DX_Conversions.h"

#include <d3dcompiler.h>

namespace CGE
{
	namespace DX12
	{
		RHI::Ptr<DX_ShaderStageFunction> DX_ShaderStageFunction::Create()
		{
			return new DX_ShaderStageFunction();
		}

		IDxcBlob* DX_ShaderStageFunction::GetByteCode() const
		{
			return m_shaderBlob.Get();
		}

		void DX_ShaderStageFunction::SetByteCode(IDxcBlob* shaderBlob)
		{
			m_shaderBlob = shaderBlob;
		}

		RHI::ResultCode DX_ShaderStageFunction::InitInternal()
		{
			return (m_shaderBlob != nullptr) ? RHI::ResultCode::Success : RHI::ResultCode::Fail;
		}

		RHI::ResultCode DX_ShaderStageFunction::FinalizeInternal()
		{
			bool emptyByteCodes = true;

			std::vector<uint8_t> shaderByteCode;
			const void* data = m_shaderBlob->GetBufferPointer();
			size_t size = m_shaderBlob->GetBufferSize();
			shaderByteCode.assign(static_cast<const uint8_t*>(data), static_cast<const uint8_t*>(data) + size);

			RHI::HashValue64 hash = RHI::HashValue64{ 0 };
			hash = RHI::TypeHash64(shaderByteCode.data(), shaderByteCode.size(), hash);
			SetHash(hash);

			return RHI::ResultCode::Success;
		}
	}
}