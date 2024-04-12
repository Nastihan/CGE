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

        // Used to update the textures data.
        struct ImageUpdateRequest
        {
            ImageUpdateRequest() = default;

            Image* m_image = nullptr;
            ImageSubresource m_imageSubresource;
            Origin m_imageSubresourcePixelOffset;
            const void* m_sourceData = nullptr;
            ImageSubresourceLayout m_sourceSubresourceLayout;
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