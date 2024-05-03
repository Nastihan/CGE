#pragma once

// RHI
#include "Resource.h"
#include "ImageDescriptor.h"
#include "ImageView.h"
#include "ImageSubresource.h"

namespace CGE
{
	namespace RHI
	{
        class ImageView;
        struct ImageViewDescriptor;

        // Subresources are flattened out into an index (mipSliceOffset + arraySliceOffset * arraySize)
        // An interval map is used to track the state of all the subresources.
        class Image : public Resource
        {
            friend class ImagePool;
            friend class StreamingImagePool;
            using Base = Resource;

        public:
            virtual ~Image() = default;

            const ImageDescriptor& GetDescriptor() const;

            // Pass it a subresource range and it will give you the ImageSubresourceLayout which can be used to do transfers between buffers and images.
            void GetSubresourceLayouts(const ImageSubresourceRange& subresourceRange, ImageSubresourceLayout* subresourceLayouts, size_t* totalSizeInBytes) const;
            uint32_t GetResidentMipLevel() const;
            Ptr<ImageView> GetImageView(const ImageViewDescriptor& imageViewDescriptor);
            ImageAspectFlags GetAspectFlags() const;
            const HashValue64 GetHash() const;
            HardwareQueueClassMask GetSupportedQueueMask() const;

        protected:
            Image() = default;
            virtual void SetDescriptor(const ImageDescriptor& descriptor);
            virtual bool IsStreamableInternal() const { return false; };
            virtual void GetSubresourceLayoutsInternal(const ImageSubresourceRange& subresourceRange, ImageSubresourceLayout* subresourceLayouts, size_t* totalSizeInBytes) const = 0;

        private:
            ImageDescriptor m_descriptor;
            HardwareQueueClassMask m_supportedQueueMask = HardwareQueueClassMask::All;
            uint32_t m_residentMipLevel = 0;
            ImageAspectFlags m_aspectFlags = ImageAspectFlags::None;
        };
	}
}