#pragma once

// RHI
#include "RHI_Common.h"
#include "intrusive_base.h"

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

        protected:
            ShaderStageFunction() = default;
            ShaderStageFunction(ShaderStage shaderStage);

        private:
            ShaderStage m_shaderStage = ShaderStage::Unknown;
        };
	}
}