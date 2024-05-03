#pragma once

// DX12
#include "DX_Descriptor.h"
#include "DX_MemoryView.h"

// RHI
#include "../RHI/BufferView.h"

namespace CGE
{
	namespace DX12
	{
		class DX_Buffer;

        // This will be the platform specific views for a buffer resource.
        // It will house discriptor heap handles spcific to this view.
        // It will use the rhi view descriptor the user passes to initilize the DX views.
        class DX_BufferView final : public RHI::BufferView
        {
            using Base = RHI::BufferView;

        public:
            static RHI::Ptr<DX_BufferView> Create();

            const DX_Buffer& GetBuffer() const;

            DX_DescriptorHandle GetReadDescriptor() const;
            DX_DescriptorHandle GetReadWriteDescriptor() const;
            DX_DescriptorHandle GetClearDescriptor() const;
            DX_DescriptorHandle GetConstantDescriptor() const;
            DX_GpuVirtualAddress GetGpuAddress() const;
            ID3D12Resource* GetMemory() const;

            // RHI::BufferView
            uint32_t GetBindlessReadIndex() const override;
            uint32_t GetBindlessReadWriteIndex() const override;

        private:
            DX_BufferView() = default;

            // RHI::BufferView
            RHI::ResultCode InitInternal(RHI::Device& device, const RHI::Resource& resourceBase) override;
            RHI::ResultCode InvalidateInternal() override;
            void ShutdownInternal() override;

        private:
            // These will be in the cpu visible heap.
            DX_DescriptorHandle m_readDescriptor;
            DX_DescriptorHandle m_readWriteDescriptor;
            DX_DescriptorHandle m_clearDescriptor;
            DX_DescriptorHandle m_constantDescriptor;
            DX_GpuVirtualAddress m_gpuAddress = 0;

            // The following indicies are offsets to the static descriptor associated with this
            // resource view in the static region of the shader-visible descriptor heap.
            // The static region is used in bindless. Check DX_DescriptorContext class.
            DX_DescriptorHandle m_staticReadDescriptor;
            DX_DescriptorHandle m_staticReadWriteDescriptor;
            DX_DescriptorHandle m_staticConstantDescriptor;

            ID3D12Resource* m_memory = nullptr;
        };
	}
}