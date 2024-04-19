#pragma once

// RHI
#include "ResourcePool.h"
#include "Image.h"
#include "ClearValue.h"
#include "ImagePoolDescriptor.h"
#include "ImageSubresource.h"

namespace CGE
{
	namespace RHI
	{
        // Used to initialize an image with a clear value.
        struct ImageInitRequest
        {
            ImageInitRequest() = default;
            ImageInitRequest(Image& image, const ImageDescriptor& descriptor, const ClearValue* optimizedClearValue = nullptr);

            Image* m_image = nullptr;
            ImageDescriptor m_descriptor;
            const ClearValue* m_optimizedClearValue = nullptr;
        };

        // Used to update the textures data. (check DX_ImagePoolResolver::UpdateImage function.)
        // A staging buffer will be used and the m_sourceData will get copied there first.
        // Will fill D3D12_PLACED_SUBRESOURCE_FOOTPRINT from m_sourceSubresourceLayout that is needed for image / buffer copies.
        // m_imageSubresourcePixelOffset used in ID3D12CommandList::CopyTextureRegion finction in DX_ImagePoolResolver::Resolve fucntion.
        struct ImageUpdateRequest
        {
            ImageUpdateRequest() = default;

            Image* m_image = nullptr;
            ImageSubresource m_imageSubresource;
            ImageSubresourceLayout m_sourceSubresourceLayout;
            Origin m_imageSubresourcePixelOffset;

            const void* m_sourceData = nullptr;
        };

        // Image pool for textures that will persist during the lifetime of the application.
        // No intra-frame aliasing support on this pool.
        class ImagePool : public ResourcePool
        {
        public:
            virtual ~ImagePool() = default;

            ResultCode Init(Device& device, const ImagePoolDescriptor& descriptor);
            ResultCode InitImage(const ImageInitRequest& initRequest);
            ResultCode UpdateImageContents(const ImageUpdateRequest& request);
            const ImagePoolDescriptor& GetDescriptor() const override final;

        protected:
            ImagePool() = default;

        private:
            using ResourcePool::Init;
            bool ValidateUpdateRequest(const ImageUpdateRequest& updateRequest) const;

            virtual ResultCode InitInternal(Device& device, const ImagePoolDescriptor& descriptor) = 0;
            virtual ResultCode UpdateImageContentsInternal(const ImageUpdateRequest& request) = 0;
            virtual ResultCode InitImageInternal(const ImageInitRequest& request) = 0;

            ImagePoolDescriptor m_descriptor;
        };
	}
}