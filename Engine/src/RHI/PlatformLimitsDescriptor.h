#pragma once

#include "RHI_Common.h"
#include "Limits.h"
#include "intrusive_base.h"

namespace CGE
{
	namespace RHI
	{
        struct PlatformDefaultValues
        {
            u64 m_bufferPoolPageSizeInBytes = Limits::DefaultValues::Memory::BufferPoolPageSizeInBytes;
            u64 m_mediumStagingBufferPageSizeInBytes = Limits::DefaultValues::Memory::MediumStagingBufferPageSizeInBytes;
            u64 m_largestStagingBufferPageSizeInBytes = Limits::DefaultValues::Memory::LargestStagingBufferPageSizeInBytes;
        };

        class PlatformLimitsDescriptor : public intrusive_base
        {
        public:
            static RHI::Ptr<PlatformLimitsDescriptor> Create();
            virtual ~PlatformLimitsDescriptor() = default;
            PlatformLimitsDescriptor() = default;
            virtual void LoadPlatformLimitsDescriptor(const char* rhiName);

        public:
            PlatformDefaultValues m_platformDefaultValues;
        };
	}
}