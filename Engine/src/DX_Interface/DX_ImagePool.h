#pragma once

// DX12
#include "DX_ImagePoolResolver.h"

// RHI
#include "../RHI/ImagePool.h"

namespace CGE
{
	namespace DX12
	{
        class DX_Device;
        class DX_ImagePoolResolver;

        class DX_ImagePool final : public RHI::ImagePool
        {
            using Base = RHI::ImagePool;
        public:
            static RHI::Ptr<DX_ImagePool> Create();
            DX_Device& GetDevice() const;

        private:
            DX_ImagePool() = default;
            DX_ImagePoolResolver* GetResolver();

            // RHI::ImagePool
            RHI::ResultCode InitInternal(RHI::Device&, const RHI::ImagePoolDescriptor&) override;
            RHI::ResultCode InitImageInternal(const RHI::ImageInitRequest& request) override;
            RHI::ResultCode UpdateImageContentsInternal(const RHI::ImageUpdateRequest& request) override;
            void ShutdownResourceInternal(RHI::Resource& resourceBase) override;
        };
	}
}