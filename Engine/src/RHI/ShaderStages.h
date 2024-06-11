#pragma once

// RHI
#include "RHI_Common.h"

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

        enum class ShaderStageMask : uint32_t
        {
            None = 0,
            Vertex = 1u << static_cast<uint32_t>(ShaderStage::Vertex),
            Fragment = 1u << static_cast<uint32_t>(ShaderStage::Fragment),
            Compute = 1u << static_cast<uint32_t>(ShaderStage::Compute),
            RayTracing = 1u << static_cast<uint32_t>(ShaderStage::RayTracing),
            All = Vertex | Fragment | Compute | RayTracing
        };

        struct ShaderFileInfo
        {
            std::string m_fileName;
            std::string m_function;
            RHI::ShaderStage m_stage;
        };
	}
}