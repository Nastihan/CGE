#pragma once

// RHI
#include "ImageEnums.h"
#include "ImageDescriptor.h"

namespace CGE
{
	namespace RHI
	{
        // Image subresources mip level, array slice (has its own mip chain) and aspect slice (color, depth and stencil)
        struct ImageSubresource
        {
            // Defaults to the highest detail mip and first array element.
            ImageSubresource() = default;
            ImageSubresource(uint16_t mipSlice, uint16_t arraySlice);
            ImageSubresource(uint16_t mipSlice, uint16_t arraySlice, ImageAspect aspect);

            uint16_t m_mipSlice = 0;
            uint16_t m_arraySlice = 0;
            ImageAspect m_aspect = ImageAspect::Color;
        };

        struct ImageSubresourceRange
        {
            static const uint16_t HighestSliceIndex = static_cast<uint16_t>(-1);

            // Defaults to the full mapping of the image.
            ImageSubresourceRange() = default;
            ImageSubresourceRange(uint16_t mipSliceMin, uint16_t mipSliceMax, uint16_t arraySliceMin, uint16_t arraySliceMax);
            explicit ImageSubresourceRange(const ImageDescriptor& descriptor);

            // This will map to the same region as the view.
            explicit ImageSubresourceRange(const ImageViewDescriptor& descriptor);

            HashValue64 GetHash(HashValue64 seed = HashValue64{ 0 }) const;
            bool operator==(const ImageSubresourceRange& other) const;

            uint16_t m_mipSliceMin = 0;
            uint16_t m_mipSliceMax = HighestSliceIndex;
            uint16_t m_arraySliceMin = 0;
            uint16_t m_arraySliceMax = HighestSliceIndex;
            ImageAspectFlags m_aspectFlags = ImageAspectFlags::All;
        };

        // This structure will be used to do some image/buffer copy operations.
        // Its the subresource layouts and total size of the image contents, if represented linearly (like a buffer).
        struct ImageSubresourceLayout
        {
            ImageSubresourceLayout() = default;
            ImageSubresourceLayout(
                Size size,
                uint32_t rowCount,
                uint32_t bytesPerRow,
                uint32_t bytesPerImage,
                uint32_t numBlocksWidth,
                uint32_t numBlocksHeight,
                uint32_t offset = 0);

            // Size in texel
            // m_depth will be set if we have a 3D texture (D3D12_RESOURCE_DIMENSION_TEXTURE3D)
            Size m_size;

            // Number of rows in texels
            uint32_t m_rowCount = 0;

            // Byte size of each row (row-pitch)
            uint32_t m_bytesPerRow = 0;

            // Bytes per image slice.
            uint32_t m_bytesPerImage = 0;

            // These are used for interpreting block compressed texure formats.
            // Multiple texels can be pack and compressed into a block.
            // For example, in BC1, BC2, and BC3 formats, each block is 4x4 texels in size. 
            // Therefore, m_blockElementWidth and m_blockElementHeight would both be set to 4 for these formats.
            // Check GetImageSubresourceLayout function.
            uint32_t m_blockElementWidth = 1;
            uint32_t m_blockElementHeight = 1;

            // Offset of the image data in the buffer. (Lets say the offset to a specific subresource of that image. Check DX_Image::GetSubresourceLayoutsInternal)
            uint32_t m_offset = 0;
        };

        struct Origin
        {
            Origin() = default;
            Origin(uint32_t left, uint32_t top, uint32_t front);

            bool operator==(const Origin& rhs) const;
            bool operator!=(const Origin& rhs) const;

            uint32_t m_left = 0;
            uint32_t m_top = 0;
            uint32_t m_front = 0;
        };

        // This family of helper function provide a standard subresource layout suitable for
        // the source of a copy from system memory to a destination RHI staging buffer. The results are
        // platform agnostic. It works by inspecting the image size and format, and then computing the required
        // size and memory layout requirements to represent the data as linear rows.
        ImageSubresourceLayout GetImageSubresourceLayout(Size imageSize, Format imageFormat);
        ImageSubresourceLayout GetImageSubresourceLayout(const ImageDescriptor& imageDescriptor, const ImageSubresource& subresource);

        // Returns the image subresource index given the mip and array slices, and the total mip levels. Subresources
        // are organized by arrays of mip chains. The formula is: subresourceIndex = mipSlice + arraySlice * mipLevels.
        uint32_t GetImageSubresourceIndex(uint32_t mipSlice, uint32_t arraySlice, uint32_t mipLevels);
        uint32_t GetImageSubresourceIndex(ImageSubresource subresource, uint32_t mipLevels);
	}
}