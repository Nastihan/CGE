#pragma once

// RHI
#include "../RHI/BufferPoolDescriptor.h"
#include "../RHI/Limits.h"
#include "../RHI/Device.h"

namespace CGE
{
	namespace DX12
	{
        class DX_BufferPoolDescriptor : public RHI::BufferPoolDescriptor
        {
        public:
            // Use to override default value
            DX_BufferPoolDescriptor(const RHI::Device& device);
            uint32_t m_bufferPoolPageSizeInBytes = RHI::Limits::DefaultValues::Memory::BufferPoolPageSizeInBytes;
        };
	}
}