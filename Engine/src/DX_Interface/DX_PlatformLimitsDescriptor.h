#pragma once

// RHI
#include "../RHI/PlatformLimitsDescriptor.h"

// std
#include <unordered_map>
#include <array>

namespace CGE
{
	namespace DX12
	{
        class DX_PlatformLimitsDescriptor final : public RHI::PlatformLimitsDescriptor
        {
            using Base = RHI::PlatformLimitsDescriptor;
        public:
            DX_PlatformLimitsDescriptor() = default;
            void LoadPlatformLimitsDescriptor(const char* rhiName) override;

        public:
            static const uint32_t NumHeapFlags = 2;// D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE + 1;

            // Max count for descriptors 
            std::unordered_map<std::string, std::array<uint32_t, NumHeapFlags>> m_descriptorHeapLimits;

            // Denote portion of the shader-visible descriptor heap used to maintain static handles.
            // NOTE: dynamic descriptors are needed to allocate per-frame descriptor tables for resources that are
            // not bound via bindless, so this number should reflect that. If the majority of resources correctly
            // leverage the bindless mechanism, this number can be higher (e.g. [0.8f, 0.9f]).
            float m_staticDescriptorRatio = 0.5f;

        };
	}
}