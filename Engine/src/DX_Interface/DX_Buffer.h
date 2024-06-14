#pragma once

// RHI
#include "../RHI/Buffer.h"

// DX12
#include "DX_BufferMemoryView.h"

namespace CGE
{
	namespace DX12
	{
        class DX_Device;
        class DX_BufferPool;

        class DX_Buffer final : public RHI::Buffer
        {
            using Base = RHI::Buffer;
        public:
            ~DX_Buffer() = default;
            static RHI::Ptr<DX_Buffer> Create();

            const DX_MemoryView& GetMemoryView() const;
            DX_MemoryView& GetMemoryView();

        private:
            friend class DX_BufferPool;
            friend class DX_BufferPoolResolver;

            DX_Buffer() = default;

            // RHI::Object
            void SetNameInternal(const std::string& name) override;
            using RHI::Buffer::SetDescriptor;

        public:
            // The initial state for the graph compiler to use when compiling the resource transition chain.
            D3D12_RESOURCE_STATES m_initialAttachmentState = D3D12_RESOURCE_STATE_COMMON;

        private:
            // DX_BufferPool will set this
            DX_BufferMemoryView m_memoryView;

            // Tracking the resolve operations for this buffer.
            std::atomic<uint32_t> m_pendingResolves = 0;
        };
	}
}