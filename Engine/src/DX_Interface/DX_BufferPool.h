#pragma once

// DX12
#include "DX_Device.h"
#include "DX_BufferMemoryAllocator.h"

// RHI
#include "../RHI/BufferPool.h"

namespace CGE
{
	namespace DX12
	{
        class DX_BufferPoolResolver;

        class DX_BufferPool final : public RHI::BufferPool
        {
            using Base = RHI::BufferPool;
        public:
            virtual ~DX_BufferPool() = default;
            static RHI::Ptr<DX_BufferPool> Create();

        private:
            DX_BufferPool() = default;
            DX_Device& GetDevice() const;

            // RHI::BufferPool
            RHI::ResultCode InitInternal(RHI::Device& device, const RHI::BufferPoolDescriptor& descriptor) override;
            void ShutdownInternal() override;
            RHI::ResultCode InitBufferInternal(RHI::Buffer& buffer, const RHI::BufferDescriptor& rhiDescriptor) override;
            void ShutdownResourceInternal(RHI::Resource& resource) override;
            RHI::ResultCode OrphanBufferInternal(RHI::Buffer& buffer) override;
            RHI::ResultCode MapBufferInternal(const RHI::BufferMapRequest& mapRequest, RHI::BufferMapResponse& response) override;
            void UnmapBufferInternal(RHI::Buffer& buffer) override;
            RHI::ResultCode StreamBufferInternal(const RHI::BufferStreamRequest& request) override;

            DX_BufferPoolResolver* GetResolver();

        private:
            DX_BufferMemoryAllocator m_allocator;
        };
	}
}