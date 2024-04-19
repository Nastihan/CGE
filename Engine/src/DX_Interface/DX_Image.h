#pragma once

// DX12
#include "DX_MemoryView.h"

// RHI
#include "../RHI/Image.h"
#include "../RHI/ImageSubresource.h"
#include "../RHI/ImageProperty.h"

namespace CGE
{
	namespace DX12
	{
        class DX_Image final : public RHI::Image
        {
            using Base = RHI::Image;
        public:
            // Describes the state of a subresource by index.
            struct SubresourceAttachmentState
            {
                // This will be calculated using (D3D12CalcSubresource function in d3dx12.h)
                // It is the subresource index for API calls that take Images need. (check DX_Image::GetAttachmentStateByIndex)
                uint32_t m_subresourceIndex = 0;
                D3D12_RESOURCE_STATES m_state = D3D12_RESOURCE_STATE_COMMON;
            };

        public:
            ~DX_Image() = default;
            static RHI::Ptr<DX_Image> Create();

            const DX_MemoryView& GetMemoryView() const;
            DX_MemoryView& GetMemoryView();

            void FinalizeAsyncUpload(uint32_t newStreamedMipLevels);
            uint32_t GetStreamedMipLevel() const;
            void SetStreamedMipLevel(uint32_t streamedMipLevel);
            void SetUploadFenceValue(uint64_t fenceValue);
            uint64_t GetUploadFenceValue() const;

            using SubresourceRangeAttachmentState = RHI::ImageProperty<D3D12_RESOURCE_STATES>::PropertyRange;

            // This function is based on image by their RHI::ImageSubresourceRange
            std::vector<SubresourceRangeAttachmentState> GetAttachmentStateByRange(const RHI::ImageSubresourceRange* range = nullptr) const;

            // Get the attachment state of some of the subresources of the image by their subresource index that dx12 API calls that use images need.
            // If argument "range" is nullptr, then the state for all subresource will be return.
            std::vector<SubresourceAttachmentState> GetAttachmentStateByIndex(const RHI::ImageSubresourceRange* range = nullptr) const;

            // If argument "range" is nullptr, then the new state will be applied to all subresources.
            void SetAttachmentState(D3D12_RESOURCE_STATES state, const RHI::ImageSubresourceRange* range = nullptr);

            // Return the initial state of this image (Set in DX_Image::SetDescriptor).
            D3D12_RESOURCE_STATES GetInitialResourceState() const;

        private:
            DX_Image() = default;

            friend class DX_SwapChain;
            friend class DX_ImagePool;
            friend class DX_ImagePoolResolver;

            // RHI::Object
            void SetNameInternal(const std::string& name) override;

            // RHI::Image
            void GetSubresourceLayoutsInternal(const RHI::ImageSubresourceRange& subresourceRange, RHI::ImageSubresourceLayout* subresourceLayouts, size_t* totalSizeInBytes) const override;
            bool IsStreamableInternal() const override;
            void SetDescriptor(const RHI::ImageDescriptor& descriptor) override;
            void GenerateSubresourceLayouts();

        private:
            // Wont do any mapping just init the name.
            DX_MemoryView m_memoryView;

            // The number of bytes actually resident.
            // For committed resources, this size won't change after image is initialized.
            size_t m_residentSizeInBytes = 0;

            // Subresource layout for each level of the mipchain. (use for buffer/image copies)
            std::array<RHI::ImageSubresourceLayout, RHI::Limits::Image::MipCountMax> m_subresourceLayoutsPerMipChain;

            // This is all the subresource states of the texture.
            RHI::ImageProperty<D3D12_RESOURCE_STATES> m_attachmentState;
            D3D12_RESOURCE_STATES m_initialResourceState = D3D12_RESOURCE_STATE_COMMON;

            uint32_t m_streamedMipLevel = 0;
            uint64_t m_uploadFenceValue = 0;

            // This will get incremented when you request an image update operation on the DX_ImagePoolResolver.
            std::atomic<uint32_t> m_pendingResolves = 0;
        };
	}
}