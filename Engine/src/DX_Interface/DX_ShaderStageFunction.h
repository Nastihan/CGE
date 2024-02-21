#pragma once

// DX12
#include "DX_CommonHeaders.h"

// RHI
#include "../RHI/ShaderStageFunction.h"

namespace CGE
{
	namespace DX12
	{
        class DX_ShaderStageFunction : public RHI::ShaderStageFunction
        {
        public:
            static RHI::Ptr<DX_ShaderStageFunction> Create(RHI::ShaderStage shaderStage);

            void SetByteCode(std::string filePath);
            ID3DBlob* GetByteCode() const;

        private:
            DX_ShaderStageFunction() = default;
            DX_ShaderStageFunction(RHI::ShaderStage shaderStage);

            wrl::ComPtr<ID3DBlob> m_shaderBlob;
        };
	}
}