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
            DX_CpuVirtualAddress MapBuffer(const RHI::BufferMapRequest& request);
            void OnResourceShutdown(const RHI::Resource& resource) override;

        private:
            struct DX_BufferUploadPacket
            {
                DX_Buffer* m_buffer = nullptr;
                DX_Memory* m_memory = nullptr;
                size_t m_memoryByteOffset = 0;
                DX_MemoryView m_sourceMemory;
            };

            DX_Device* m_device = nullptr;
            D3D12_RESOURCE_STATES m_readOnlyState = D3D12_RESOURCE_STATE_COMMON;
            std::vector<DX_BufferUploadPacket> m_uploadPackets;
        };
	}
}