#pragma once

// DX12
#include "DX_CommonHeaders.h"
#include "DX_Descriptor.h"

// RHI
#include "../RHI/ImageView.h"
#include "../RHI/AttachmentEnums.h"

namespace CGE
{
	namespace DX12
	{
        class DX_Image;

        class DX_ImageView final : public RHI::ImageView
        {
            using Base = RHI::ImageView;
        public:
            static RHI::Ptr<DX_ImageView> Create();

            const DX_Image& GetImage() const;
            DXGI_FORMAT GetFormat() const;
            ID3D12Resource* GetMemory() const;

            DX_DescriptorHandle GetReadDescriptor() const;
            DX_DescriptorHandle GetReadWriteDescriptor() const;
            DX_DescriptorHandle GetClearDescriptor() const;
            DX_DescriptorHandle GetColorDescriptor() const;
            DX_DescriptorHandle GetDepthStencilDescriptor(RHI::ScopeAttachmentAccess access) const;

            uint32_t GetBindlessReadIndex() const override;
            uint32_t GetBindlessReadWriteIndex() const override;

        private:
            DX_ImageView() = default;

            RHI::ResultCode InitInternal(RHI::Device& device, const RHI::Resource& resourceBase) override;
            RHI::ResultCode InvalidateInternal() override;
            void ShutdownInternal() override;

            ID3D12Resource* m_memory = nullptr;
            DXGI_FORMAT m_format = DXGI_FORMAT_UNKNOWN;
            DX_DescriptorHandle m_readDescriptor;
            DX_DescriptorHandle m_readWriteDescriptor;
            DX_DescriptorHandle m_clearDescriptor;
            DX_DescriptorHandle m_colorDescriptor;
            DX_DescriptorHandle m_depthStencilDescriptor;
            DX_DescriptorHandle m_depthStencilReadDescriptor;

            DX_DescriptorHandle m_staticReadDescriptor;
            DX_DescriptorHandle m_staticReadWriteDescriptor;
        };
	}
}