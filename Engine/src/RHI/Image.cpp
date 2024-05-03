
// RHI
#include "Image.h"

// std
#include <algorithm>

namespace CGE
{
	namespace RHI
	{
        void Image::SetDescriptor(const ImageDescriptor& descriptor)
        {
            m_descriptor = descriptor;
            m_aspectFlags = GetImageAspectFlags(descriptor.m_format);
        }

        void Image::GetSubresourceLayouts(const ImageSubresourceRange& subresourceRange, ImageSubresourceLayout* subresourceLayouts, size_t* totalSizeInBytes) const
        {
            const RHI::ImageDescriptor& imageDescriptor = GetDescriptor();
            ImageSubresourceRange subresourceRangeClamped;
            subresourceRangeClamped.m_mipSliceMin = subresourceRange.m_mipSliceMin;
            subresourceRangeClamped.m_mipSliceMax = std::clamp<uint16_t>(subresourceRange.m_mipSliceMax, subresourceRange.m_mipSliceMin, imageDescriptor.m_mipLevels - 1);
            subresourceRangeClamped.m_arraySliceMin = subresourceRange.m_arraySliceMin;
            subresourceRangeClamped.m_arraySliceMax = std::clamp<uint16_t>(subresourceRange.m_arraySliceMax, subresourceRange.m_arraySliceMin, imageDescriptor.m_arraySize - 1);
            GetSubresourceLayoutsInternal(subresourceRangeClamped, subresourceLayouts, totalSizeInBytes);
        }

        uint32_t Image::GetResidentMipLevel() const
        {
            return m_residentMipLevel;
        }

        Ptr<ImageView> Image::GetImageView(const ImageViewDescriptor& imageViewDescriptor)
        {
            return Base::GetResourceView(imageViewDescriptor);
        }

        const ImageDescriptor& Image::GetDescriptor() const
        {
            return m_descriptor;
        }

        ImageAspectFlags Image::GetAspectFlags() const
        {
            return m_aspectFlags;
        }

        const HashValue64 Image::GetHash() const
        {
            HashValue64 hash = HashValue64{ 0 };
            hash = m_descriptor.GetHash();
            hash = TypeHash64(m_supportedQueueMask, hash);
            hash = TypeHash64(m_residentMipLevel, hash);
            hash = TypeHash64(m_aspectFlags, hash);
            return hash;
        }

        HardwareQueueClassMask Image::GetSupportedQueueMask() const
        {
            return m_supportedQueueMask;
        }
	}
}