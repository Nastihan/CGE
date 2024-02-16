#pragma once

// RHI
#include "../RHI/BufferView.h"
#include "DX_Descriptor.h"
#include "DX_MemoryView.h"

namespace CGE
{
	namespace DX12
	{
		class DX_Buffer;

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

        private:
            DX_BufferView() = default;

            // RHI::BufferView
            RHI::ResultCode InitInternal(RHI::Device& device, const RHI::Resource& resourceBase) override;
            RHI::ResultCode InvalidateInternal() override;
            void ShutdownInternal() override;

        private:
            DX_DescriptorHandle m_readDescriptor;
            DX_DescriptorHandle m_readWriteDescriptor;
            DX_DescriptorHandle m_clearDescriptor;
            DX_DescriptorHandle m_constantDescriptor;
            DX_GpuVirtualAddress m_gpuAddress = 0;

            DX_DescriptorHandle m_staticReadDescriptor;
            DX_DescriptorHandle m_staticReadWriteDescriptor;
            DX_DescriptorHandle m_staticConstantDescriptor;

            ID3D12Resource* m_memory = nullptr;
        };
	}
}