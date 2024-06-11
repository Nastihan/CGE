#pragma once

// RHI
#include "RHI_Common.h"

namespace CGE
{
	namespace RHI
	{
        enum class ImageBindFlags : uint32_t
        {
            None = 0,
            ShaderRead = (1u << 0u),
            ShaderWrite = (1u << 1u),
            ShaderReadWrite = (ShaderRead | ShaderWrite),
            // Render targets.
            Color = (1u << 2u),
            Depth = (1u << 3u),
            Stencil = (1u << 4u),
            DepthStencil = (Depth | Stencil),
            CopyRead = (1u << 5u),
            CopyWrite = (1u << 6u),
            ShadingRate = (1u << 7u)
        };

        inline ImageBindFlags operator|(ImageBindFlags a, ImageBindFlags b)
        {
            return static_cast<ImageBindFlags>(static_cast<int>(a) | static_cast<int>(b));
        }
        inline ImageBindFlags operator&(ImageBindFlags a, ImageBindFlags b)
        {
            return static_cast<ImageBindFlags>(static_cast<int>(a) & static_cast<int>(b));
        }

        // [todo] Not sure what this is used for ?
        enum class ImageAspect : uint32_t
        {
            Color = 0,
            Depth,
            Stencil,
            Count
        };
        static const uint32_t ImageAspectCount = static_cast<uint32_t>(ImageAspect::Count);

        enum class ImageAspectFlags : uint32_t
        {
            None = 0,
            Color = (1u << static_cast<uint32_t>(ImageAspect::Color)),
            Depth = (1u << static_cast<uint32_t>(ImageAspect::Depth)),
            Stencil = (1u << static_cast<uint32_t>(ImageAspect::Stencil)),
            DepthStencil = Depth | Stencil,
            All = ~uint32_t(0)
        };

        inline uint32_t operator&(ImageAspectFlags a, ImageAspectFlags b)
        {
            return static_cast<uint32_t>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
        }

        inline uint32_t operator|(ImageAspectFlags a, ImageAspectFlags b)
        {
            return static_cast<uint32_t>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
        }
	}
}