#pragma once

// DX12
#include "DX_ResourcePoolResolver.h"
#include "DX_Device.h"
#include "DX_BufferPool.h"
#include "DX_Buffer.h"

// RHI
#include "../RHI/BufferPoolDescriptor.h"

namespace CGE
{
	namespace DX12
	{
        class DX_BufferPoolResolver : public DX_ResourcePoolResolver
        {
        public:
            DX_BufferPoolResolver(DX_Device& device, const RHI::BufferPoolDescriptor& descriptor);

            // This function will be called by DX_BufferPool::MapBufferInternal if the buffer is in RHI::HeapMemoryLevel::Device heap.
            // The reason is we need to acquire a staging memory and queue an upload request and the frame scheduler will call the resolve (CopyBufferRegion)
            // at the correct time (at the begining of the frame)
            DX_CpuVirtualAddress MapBuffer(const RHI::BufferMapRequest& request);

            // DX_ResourcePoolResolver

            // This functions at the begining of the frame
            void Compile(DX_Scope& scope) override;
            void Resolve(DX_CommandList&) const override;
            void QueueEpilogueTransitionBarriers(DX_CommandList&) const override;
            void Deactivate() override;
            void OnResourceShutdown(const RHI::Resource& resource) override;

        private:
            struct DX_BufferUploadPacket
            {
                // Destination
                DX_Buffer* m_buffer = nullptr;
                DX_Memory* m_memory = nullptr;
                size_t m_memoryByteOffset = 0;

                // Staging buffer
                DX_MemoryView m_sourceMemory;
            };

            DX_Device* m_device = nullptr;

            // The resource will transition from D3D12_RESOURCE_STATE_COPY_DEST to m_readOnlyState.
            // This will be initilized based on the pools bind flags (usage)
            D3D12_RESOURCE_STATES m_readOnlyState = D3D12_RESOURCE_STATE_COMMON;
            std::vector<DX_BufferUploadPacket> m_uploadPackets;
        };
	}
}