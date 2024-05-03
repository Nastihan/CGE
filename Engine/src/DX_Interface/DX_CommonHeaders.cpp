#include "DX_CommonHeaders.h"

namespace CGE
{
	namespace DX12
	{
        bool DXAssertSuccess(HRESULT hr)
        {
            if (FAILED(hr))
            {
                char* msgBuf = nullptr;

                if (FormatMessage(
                    FORMAT_MESSAGE_ALLOCATE_BUFFER |
                    FORMAT_MESSAGE_FROM_SYSTEM |
                    FORMAT_MESSAGE_IGNORE_INSERTS,
                    NULL,
                    hr,
                    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                    (LPTSTR)&msgBuf,
                    0,
                    NULL))
                {
                    printf("HRESULT not a success %x, error msg = %s", hr, msgBuf);
                    LocalFree(msgBuf);
                }
                else
                {
                    printf("HRESULT not a success %x, Unable to retrieve error msg", hr);
                }
                return false;
            }
            return true;
        }

        DXGI_FORMAT GetBaseFormat(DXGI_FORMAT defaultFormat)
        {
            switch (defaultFormat)
            {
            case DXGI_FORMAT_R8G8B8A8_UNORM:
            case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
                return DXGI_FORMAT_R8G8B8A8_TYPELESS;

            case DXGI_FORMAT_B8G8R8A8_UNORM:
            case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
                return DXGI_FORMAT_B8G8R8A8_TYPELESS;

            case DXGI_FORMAT_B8G8R8X8_UNORM:
            case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
                return DXGI_FORMAT_B8G8R8X8_TYPELESS;

                // 32-bit Z w/ Stencil
            case DXGI_FORMAT_R32G8X24_TYPELESS:
            case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
            case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
            case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
                return DXGI_FORMAT_R32G8X24_TYPELESS;

                // No Stencil
            case DXGI_FORMAT_R32_TYPELESS:
            case DXGI_FORMAT_D32_FLOAT:
            case DXGI_FORMAT_R32_FLOAT:
                return DXGI_FORMAT_R32_TYPELESS;

                // 24-bit Z
            case DXGI_FORMAT_R24G8_TYPELESS:
            case DXGI_FORMAT_D24_UNORM_S8_UINT:
            case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
            case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
                return DXGI_FORMAT_R24G8_TYPELESS;

            case DXGI_FORMAT_BC1_UNORM:
            case DXGI_FORMAT_BC1_UNORM_SRGB:
                return DXGI_FORMAT_BC1_TYPELESS;

            case DXGI_FORMAT_BC2_UNORM:
            case DXGI_FORMAT_BC2_UNORM_SRGB:
                return DXGI_FORMAT_BC2_TYPELESS;

            case DXGI_FORMAT_BC3_UNORM:
            case DXGI_FORMAT_BC3_UNORM_SRGB:
                return DXGI_FORMAT_BC3_TYPELESS;

            case DXGI_FORMAT_BC4_UNORM:
            case DXGI_FORMAT_BC4_SNORM:
                return DXGI_FORMAT_BC4_TYPELESS;

            case DXGI_FORMAT_BC5_UNORM:
            case DXGI_FORMAT_BC5_SNORM:
                return DXGI_FORMAT_BC5_TYPELESS;

                // 16-bit Z w/o Stencil
            case DXGI_FORMAT_R16_TYPELESS:
            case DXGI_FORMAT_D16_UNORM:
            case DXGI_FORMAT_R16_UNORM:
                return DXGI_FORMAT_R16_TYPELESS;

            default:
                return defaultFormat;
            }
        }

        DXGI_FORMAT GetUAVFormat(DXGI_FORMAT defaultFormat)
        {
            switch (defaultFormat)
            {
            case DXGI_FORMAT_R8G8B8A8_TYPELESS:
            case DXGI_FORMAT_R8G8B8A8_UNORM:
            case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
                return DXGI_FORMAT_R8G8B8A8_UNORM;

            case DXGI_FORMAT_B8G8R8A8_TYPELESS:
            case DXGI_FORMAT_B8G8R8A8_UNORM:
            case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
                return DXGI_FORMAT_B8G8R8A8_UNORM;

            case DXGI_FORMAT_B8G8R8X8_TYPELESS:
            case DXGI_FORMAT_B8G8R8X8_UNORM:
            case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
                return DXGI_FORMAT_B8G8R8X8_UNORM;

            case DXGI_FORMAT_R32_TYPELESS:
            case DXGI_FORMAT_R32_FLOAT:
                return DXGI_FORMAT_R32_FLOAT;

#if defined(_DEBUG)
            case DXGI_FORMAT_R32G8X24_TYPELESS:
            case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
            case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
            case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
            case DXGI_FORMAT_D32_FLOAT:
            case DXGI_FORMAT_R24G8_TYPELESS:
            case DXGI_FORMAT_D24_UNORM_S8_UINT:
            case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
            case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
            case DXGI_FORMAT_D16_UNORM:

                assert(false, "Requested a UAV format for a depth stencil format.");
#endif

            default:
                return defaultFormat;
            }
        }

        DXGI_FORMAT GetDSVFormat(DXGI_FORMAT defaultFormat)
        {
            switch (defaultFormat)
            {
                // 32-bit Z w/ Stencil
            case DXGI_FORMAT_R32G8X24_TYPELESS:
            case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
            case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
            case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
                return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;

                // No Stencil
            case DXGI_FORMAT_R32_TYPELESS:
            case DXGI_FORMAT_D32_FLOAT:
            case DXGI_FORMAT_R32_FLOAT:
                return DXGI_FORMAT_D32_FLOAT;

                // 24-bit Z
            case DXGI_FORMAT_R24G8_TYPELESS:
            case DXGI_FORMAT_D24_UNORM_S8_UINT:
            case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
            case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
                return DXGI_FORMAT_D24_UNORM_S8_UINT;

                // 16-bit Z w/o Stencil
            case DXGI_FORMAT_R16_TYPELESS:
            case DXGI_FORMAT_D16_UNORM:
            case DXGI_FORMAT_R16_UNORM:
                return DXGI_FORMAT_D16_UNORM;

            default:
                return defaultFormat;
            }
        }

        DXGI_FORMAT GetSRVFormat(DXGI_FORMAT defaultFormat)
        {
            switch (defaultFormat)
            {
                // 32-bit Z w/ Stencil
            case DXGI_FORMAT_R32G8X24_TYPELESS:
            case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
            case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
            case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
                return DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;

                // No Stencil
            case DXGI_FORMAT_R32_TYPELESS:
            case DXGI_FORMAT_D32_FLOAT:
            case DXGI_FORMAT_R32_FLOAT:
                return DXGI_FORMAT_R32_FLOAT;

                // 24-bit Z
            case DXGI_FORMAT_R24G8_TYPELESS:
            case DXGI_FORMAT_D24_UNORM_S8_UINT:
            case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
            case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
                return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;

                // 16-bit Z w/o Stencil
            case DXGI_FORMAT_R16_TYPELESS:
            case DXGI_FORMAT_D16_UNORM:
            case DXGI_FORMAT_R16_UNORM:
                return DXGI_FORMAT_R16_UNORM;

            default:
                return defaultFormat;
            }
        }

        DXGI_FORMAT GetStencilFormat(DXGI_FORMAT defaultFormat)
        {
            switch (defaultFormat)
            {
                // 32-bit Z w/ Stencil
            case DXGI_FORMAT_R32G8X24_TYPELESS:
            case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
            case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
            case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
                return DXGI_FORMAT_X32_TYPELESS_G8X24_UINT;

                // 24-bit Z
            case DXGI_FORMAT_R24G8_TYPELESS:
            case DXGI_FORMAT_D24_UNORM_S8_UINT:
            case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
            case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
                return DXGI_FORMAT_X24_TYPELESS_G8_UINT;

            default:
                return DXGI_FORMAT_UNKNOWN;
            }
        }
	}
}