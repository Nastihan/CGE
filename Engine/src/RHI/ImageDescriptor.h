#pragma once

#include "RHI_Common.h"
#include "AttachmentEnums.h"
#include "ImageViewDescriptor.h"
#include "MultisampleState.h"
#include "ImageEnums.h"

namespace CGE
{
	namespace RHI
	{
        enum class ImageDimension
        {
            Image1D = 1,
            Image2D = 2,
            Image3D = 3,
        };

        struct Size
        {
            Size() = default;
            Size(uint32_t width, uint32_t height, uint32_t depth);
            Size GetReducedMip(uint32_t mipLevel) const;

            uint32_t m_width = 1;
            uint32_t m_height = 1;
            uint32_t m_depth = 1;

            bool operator==(const Size& rhs) const;
            bool operator!=(const Size& rhs) const;

            uint32_t& operator[](uint32_t idx);
            uint32_t operator[](uint32_t idx) const;
        };

        // Images are comprised of sub-resources corresponding to the number of mip-mip levels
        // and array slices. Image data is stored as pixels in opaque swizzled formats. Images
        // represent texture data to the shader.
        // This class will be used to initilize an image.
        struct ImageDescriptor
        {
            static const uint16_t NumCubeMapSlices = 6;

            static ImageDescriptor Create1D(ImageBindFlags bindFlags, uint32_t width, Format format);
            static ImageDescriptor Create1DArray(ImageBindFlags bindFlags, uint32_t width, uint16_t arraySize, Format format);

            static ImageDescriptor Create2D(ImageBindFlags bindFlags, uint32_t width, uint32_t height, Format format);
            static ImageDescriptor Create2DArray(ImageBindFlags bindFlags, uint32_t width, uint32_t height, uint16_t arraySize, Format format);

            static ImageDescriptor CreateCubemap(ImageBindFlags bindFlags, uint32_t width, Format format);
            static ImageDescriptor CreateCubemapArray(ImageBindFlags bindFlags, uint32_t width, uint16_t arraySize, Format format);

            static ImageDescriptor Create3D(ImageBindFlags bindFlags, uint32_t width, uint32_t height, uint32_t depth, Format format);

            ImageDescriptor() = default;
            HashValue64 GetHash(HashValue64 seed = HashValue64{ 0 }) const;

            // Width, Height and Depth.
            Size m_size;
            ImageDimension m_dimension = ImageDimension::Image2D;
            ImageBindFlags m_bindFlags = ImageBindFlags::ShaderRead;
            uint16_t m_arraySize = 1;
            uint16_t m_mipLevels = 1;
            Format m_format = Format::Unknown;
            HardwareQueueClassMask m_sharedQueueMask = HardwareQueueClassMask::All;
            MultisampleState m_multisampleState;
            uint32_t m_isCubemap = 0;
        };

        inline bool IsMipMoreDetailedThan(uint32_t mipA, uint32_t mipB)
        {
            return mipA < mipB;
        }

        inline bool IsMipLessDetailedThan(uint32_t mipA, uint32_t mipB)
        {
            return mipA > mipB;
        }

        inline uint32_t IncreaseMipDetailBy(uint32_t mipLevel, uint32_t increaseBy)
        {
            assert(mipLevel >= increaseBy, "Exceeded mip detail.");
            return mipLevel - increaseBy;
        }

        inline uint32_t DecreaseMipDetailBy(uint32_t mipLevel, uint32_t decreaseBy)
        {
            return mipLevel + decreaseBy;
        }
	}
}