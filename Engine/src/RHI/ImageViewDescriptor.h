#pragma once

// RHI
#include "RHI_Common.h"
#include "Format.h"
#include "TypeHash.h"

namespace CGE
{
	namespace RHI
	{
        // GPU Image view abstraction. Used to initilize a texture resource view.
        struct ImageViewDescriptor
        {
            static ImageViewDescriptor Create(Format overrideFormat, uint16_t mipSliceMin, uint16_t mipSliceMax);
            static ImageViewDescriptor Create(Format overrideFormat, uint16_t mipSliceMin, uint16_t mipSliceMax, uint16_t arraySliceMin, uint16_t arraySliceMax);

            static ImageViewDescriptor CreateCubemap();
            static ImageViewDescriptor CreateCubemap(Format overrideFormat, uint16_t mipSliceMin, uint16_t mipSliceMax);
            static ImageViewDescriptor CreateCubemap(Format overrideFormat, uint16_t mipSliceMin, uint16_t mipSliceMax, uint16_t cubeSliceMin, uint16_t cubeSliceMax);
            static ImageViewDescriptor CreateCubemapFace(Format overrideFormat, uint16_t mipSliceMin, uint16_t mipSliceMax, uint16_t faceSlice);

            static ImageViewDescriptor Create3D(Format overrideFormat, uint16_t mipSliceMin, uint16_t mipSliceMax, uint16_t depthSliceMin, uint16_t depthSliceMax);

            static const uint16_t HighestSliceIndex = static_cast<uint16_t>(-1);

            ImageViewDescriptor() = default;
            explicit ImageViewDescriptor(Format overrideFormat);
            bool operator==(const ImageViewDescriptor& other) const;
            HashValue64 GetHash(HashValue64 seed = HashValue64{ 0 }) const;
            bool IsSameSubResource(const ImageViewDescriptor& other) const;

            // Mip range for resource view creation
            uint16_t m_mipSliceMin = 0;
            uint16_t m_mipSliceMax = HighestSliceIndex;

            // Array range for resource view creation
            uint16_t m_arraySliceMin = 0;
            uint16_t m_arraySliceMax = HighestSliceIndex;

            // Depth range for resource view creation
            uint16_t m_depthSliceMin = 0;
            uint16_t m_depthSliceMax = HighestSliceIndex;

            // Typed format of the view, which may override a format in
            // the image. A value of Unknown will default to the image format.
            Format m_overrideFormat = Format::Unknown;

            // The bind flags used by this view. Should be compatible with the bind flags of the underlying image.
            ImageBindFlags m_overrideBindFlags = ImageBindFlags::None;

            // Whether to treat this image as a cubemap in the shader (Check ConvertImageView in DX_Conversions.h)
            uint32_t m_isCubemap = 0;

            ImageAspectFlags m_aspectFlags = ImageAspectFlags::All;

            // If the image is an array of textures.
            uint32_t m_isArray = 0;
        };
	}
}