#pragma once

// DX12
#include "DX_CommonHeaders.h"
#include "DX_ShaderCompiler.h"

// RHI
#include "../RHI/ShaderStageFunction.h"

namespace CGE
{
	namespace DX12
	{
        class DX_ShaderStageFunction : public RHI::ShaderStageFunction
        {
        public:
            static RHI::Ptr<DX_ShaderStageFunction> Create();
            void SetByteCode(IDxcBlob* shaderBlob);
            IDxcBlob* GetByteCode() const;

        private:
            DX_ShaderStageFunction() = default;

            // RHI::ShaderStageFunction
            RHI::ResultCode InitInternal() override;
            RHI::ResultCode FinalizeInternal() override;

            wrl::ComPtr<IDxcBlob> m_shaderBlob;
        };
	}
}