#pragma once

// RHI
#include "Limits.h"

namespace CGE
{
	namespace RHI
	{
        //! Struct to help cache all indices related to bindless resource types as well as bindless srg binding slot
        struct BindlessSrgDescriptor
        {
            // Use for vulkan backend.
            uint32_t m_roTextureIndex = RHI::Limits::InvalidIndex;
            uint32_t m_rwTextureIndex = RHI::Limits::InvalidIndex;
            uint32_t m_roTextureCubeIndex = RHI::Limits::InvalidIndex;
            uint32_t m_roBufferIndex = RHI::Limits::InvalidIndex;
            uint32_t m_rwBufferIndex = RHI::Limits::InvalidIndex;

            uint32_t m_bindlesSrgBindingSlot = RHI::Limits::InvalidIndex;
        };
	}
}