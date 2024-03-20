#pragma once

// RHI
#include "RHI_Common.h"
#include "intrusive_base.h"
#include "TypeHash.h"

namespace CGE
{
	namespace RHI
	{
        enum class ShaderStage : uint32_t
        {
            // Used for invalid state.
            Unknown = static_cast<uint32_t>(-1),
            Vertex = 0,
            Fragment,
            Compute,
            RayTracing,
            Count
        };

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