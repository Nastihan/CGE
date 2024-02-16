
// RHI
#include "Format.h"

namespace CGE
{
	namespace RHI
	{
        uint32_t GetFormatSize(Format format)
        {
            switch (format)
            {
            case Format::Unknown:
                return 0;

            case Format::R32G32B32A32_FLOAT:
            case Format::R32G32B32A32_UINT:
            case Format::R32G32B32A32_SINT:
                return 16;

            case Format::R32G32B32_FLOAT:
            case Format::R32G32B32_UINT:
            case Format::R32G32B32_SINT:
                return 12;

            case Format::R16G16B16A16_FLOAT:
            case Format::R16G16B16A16_UNORM:
            case Format::R16G16B16A16_UINT:
            case Format::R16G16B16A16_SNORM:
            case Format::R16G16B16A16_SINT:
            case Format::R32G32_FLOAT:
            case Format::R32G32_UINT:
            case Format::R32G32_SINT:
            case Format::D32_FLOAT_S8X24_UINT:
            case Format::Y416:
            case Format::Y210:
            case Format::Y216:
                return 8;

            case Format::R10G10B10A2_UNORM:
            case Format::R10G10B10A2_UINT:
            case Format::R11G11B10_FLOAT:
            case Format::R8G8B8A8_UNORM:
            case Format::R8G8B8A8_UNORM_SRGB:
            case Format::R8G8B8A8_UINT:
            case Format::R8G8B8A8_SNORM:
            case Format::R8G8B8A8_SINT:
            case Format::R16G16_FLOAT:
            case Format::R16G16_UNORM:
            case Format::R16G16_UINT:
            case Format::R16G16_SNORM:
            case Format::R16G16_SINT:
            case Format::D32_FLOAT:
            case Format::R32_FLOAT:
            case Format::R32_UINT:
            case Format::R32_SINT:
            case Format::D24_UNORM_S8_UINT:
            case Format::R9G9B9E5_SHAREDEXP:
            case Format::R8G8_B8G8_UNORM:
            case Format::G8R8_G8B8_UNORM:
            case Format::B8G8R8A8_UNORM:
            case Format::B8G8R8A8_SNORM:
            case Format::B8G8R8X8_UNORM:
            case Format::R10G10B10_XR_BIAS_A2_UNORM:
            case Format::B8G8R8A8_UNORM_SRGB:
            case Format::B8G8R8X8_UNORM_SRGB:
            case Format::AYUV:
            case Format::Y410:
            case Format::YUY2:
                return 4;

            case Format::P010:
            case Format::P016:
                return 24;

            case Format::R8G8_UNORM:
            case Format::R8G8_UINT:
            case Format::R8G8_SNORM:
            case Format::R8G8_SINT:
            case Format::R16_FLOAT:
            case Format::D16_UNORM:
            case Format::R16_UNORM:
            case Format::R16_UINT:
            case Format::R16_SNORM:
            case Format::R16_SINT:
            case Format::R5G6B5_UNORM:
            case Format::B5G6R5_UNORM:
            case Format::B5G5R5A1_UNORM:
            case Format::A8P8:
            case Format::B4G4R4A4_UNORM:
                return 2;

            case Format::NV12:
            case Format::NV11:
                return 12;

            case Format::R8_UNORM:
            case Format::R8_UINT:
            case Format::R8_SNORM:
            case Format::R8_SINT:
            case Format::A8_UNORM:
            case Format::AI44:
            case Format::IA44:
            case Format::P8:
            case Format::R1_UNORM:
                return 1;

            case Format::BC1_UNORM:
            case Format::BC1_UNORM_SRGB:
            case Format::BC4_UNORM:
            case Format::BC4_SNORM:
            case Format::EAC_R11_UNORM:
            case Format::EAC_R11_SNORM:
            case Format::ETC2_UNORM:
            case Format::ETC2_UNORM_SRGB:
            case Format::ETC2A1_UNORM:
            case Format::ETC2A1_UNORM_SRGB:
                return 8;

            case Format::BC2_UNORM:
            case Format::BC2_UNORM_SRGB:
            case Format::BC3_UNORM:
            case Format::BC3_UNORM_SRGB:
            case Format::BC5_UNORM:
            case Format::BC5_SNORM:
            case Format::BC6H_UF16:
            case Format::BC6H_SF16:
            case Format::BC7_UNORM:
            case Format::BC7_UNORM_SRGB:
            case Format::EAC_RG11_UNORM:
            case Format::EAC_RG11_SNORM:
            case Format::ETC2A_UNORM:
            case Format::ETC2A_UNORM_SRGB:
            case Format::ASTC_4x4_UNORM:
            case Format::ASTC_4x4_UNORM_SRGB:
            case Format::ASTC_5x4_UNORM:
            case Format::ASTC_5x4_UNORM_SRGB:
            case Format::ASTC_5x5_UNORM:
            case Format::ASTC_5x5_UNORM_SRGB:
            case Format::ASTC_6x5_UNORM:
            case Format::ASTC_6x5_UNORM_SRGB:
            case Format::ASTC_6x6_UNORM:
            case Format::ASTC_6x6_UNORM_SRGB:
            case Format::ASTC_8x5_UNORM:
            case Format::ASTC_8x5_UNORM_SRGB:
            case Format::ASTC_8x6_UNORM:
            case Format::ASTC_8x6_UNORM_SRGB:
            case Format::ASTC_8x8_UNORM:
            case Format::ASTC_8x8_UNORM_SRGB:
            case Format::ASTC_10x5_UNORM:
            case Format::ASTC_10x5_UNORM_SRGB:
            case Format::ASTC_10x6_UNORM:
            case Format::ASTC_10x6_UNORM_SRGB:
            case Format::ASTC_10x8_UNORM:
            case Format::ASTC_10x8_UNORM_SRGB:
            case Format::ASTC_10x10_UNORM:
            case Format::ASTC_10x10_UNORM_SRGB:
            case Format::ASTC_12x10_UNORM:
            case Format::ASTC_12x10_UNORM_SRGB:
            case Format::ASTC_12x12_UNORM:
            case Format::ASTC_12x12_UNORM_SRGB:
                return 16;

            default:
                assert(false, "Unimplemented format");
                return 0;
            }
        }
	}
}