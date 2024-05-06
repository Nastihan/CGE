#pragma once

// RHI
#include "RHI_Common.h"
#include "intrusive_base.h"
#include "TypeHash.h"
#include "ShaderStages.h"

namespace CGE
{
	namespace RHI
	{
        class ShaderStageFunction : public intrusive_base
        {
        public:
            virtual ~ShaderStageFunction() = default;
            ShaderStage GetShaderStage() const;
            HashValue64 GetHash() const;
            ResultCode Finalize();

        protected:
            ShaderStageFunction() = default;
            ShaderStageFunction(ShaderStage shaderStage);
            void SetHash(HashValue64 hash);

        private:
            virtual ResultCode FinalizeInternal() = 0;

            ShaderStage m_shaderStage = ShaderStage::Unknown;
            HashValue64 m_hash = HashValue64{ 0 };
        };
	}
}