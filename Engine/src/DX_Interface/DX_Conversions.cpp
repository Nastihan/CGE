// DX12
#include "DX_Conversions.h"
#include "DX_CommonHeaders.h"
#include <d3dx12.h>

// RHI
#include "../RHI/ClearValue.h"

namespace CGE
{
    namespace DX12
    {
        D3D12_COMMAND_LIST_TYPE ConvertHardwareQueueClass(RHI::HardwareQueueClass type)
        {
            static const D3D12_COMMAND_LIST_TYPE table[] = { D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_LIST_TYPE_COMPUTE, D3D12_COMMAND_LIST_TYPE_COPY };
            return table[static_cast<size_t>(type)];
        }

        std::wstring s2ws(const std::string& str)
        {
            int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
            std::wstring wstrTo(size_needed, 0);
            MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
            return wstrTo;
        }

        DXGI_FORMAT ConvertFormat(RHI::Format format)
        {
            switch (format)
            {
            case RHI::Format::Unknown:
                return DXGI_FORMAT_UNKNOWN;
            case RHI::Format::R32G32B32A32_FLOAT:
                return DXGI_FORMAT_R32G32B32A32_FLOAT;
            case RHI::Format::R32G32B32A32_UINT:
                return DXGI_FORMAT_R32G32B32A32_UINT;
            case RHI::Format::R32G32B32A32_SINT:
                return DXGI_FORMAT_R32G32B32A32_SINT;
            case RHI::Format::R32G32B32_FLOAT:
                return DXGI_FORMAT_R32G32B32_FLOAT;
            case RHI::Format::R32G32B32_UINT:
                return DXGI_FORMAT_R32G32B32_UINT;
            case RHI::Format::R32G32B32_SINT:
                return DXGI_FORMAT_R32G32B32_SINT;
            case RHI::Format::R16G16B16A16_FLOAT:
                return DXGI_FORMAT_R16G16B16A16_FLOAT;
            case RHI::Format::R16G16B16A16_UNORM:
                return DXGI_FORMAT_R16G16B16A16_UNORM;
            case RHI::Format::R16G16B16A16_UINT:
                return DXGI_FORMAT_R16G16B16A16_UINT;
            case RHI::Format::R16G16B16A16_SNORM:
                return DXGI_FORMAT_R16G16B16A16_SNORM;
            case RHI::Format::R16G16B16A16_SINT:
                return DXGI_FORMAT_R16G16B16A16_SINT;
            case RHI::Format::R32G32_FLOAT:
                return DXGI_FORMAT_R32G32_FLOAT;
            case RHI::Format::R32G32_UINT:
                return DXGI_FORMAT_R32G32_UINT;
            case RHI::Format::R32G32_SINT:
                return DXGI_FORMAT_R32G32_SINT;
            case RHI::Format::D32_FLOAT_S8X24_UINT:
                return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
            case RHI::Format::R10G10B10A2_UNORM:
                return DXGI_FORMAT_R10G10B10A2_UNORM;
            case RHI::Format::R10G10B10A2_UINT:
                return DXGI_FORMAT_R10G10B10A2_UINT;
            case RHI::Format::R11G11B10_FLOAT:
                return DXGI_FORMAT_R11G11B10_FLOAT;
            case RHI::Format::R8G8B8A8_UNORM:
                return DXGI_FORMAT_R8G8B8A8_UNORM;
            case RHI::Format::R8G8B8A8_UNORM_SRGB:
                return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
            case RHI::Format::R8G8B8A8_UINT:
                return DXGI_FORMAT_R8G8B8A8_UINT;
            case RHI::Format::R8G8B8A8_SNORM:
                return DXGI_FORMAT_R8G8B8A8_SNORM;
            case RHI::Format::R8G8B8A8_SINT:
                return DXGI_FORMAT_R8G8B8A8_SINT;
            case RHI::Format::R16G16_FLOAT:
                return DXGI_FORMAT_R16G16_FLOAT;
            case RHI::Format::R16G16_UNORM:
                return DXGI_FORMAT_R16G16_UNORM;
            case RHI::Format::R16G16_UINT:
                return DXGI_FORMAT_R16G16_UINT;
            case RHI::Format::R16G16_SNORM:
                return DXGI_FORMAT_R16G16_SNORM;
            case RHI::Format::R16G16_SINT:
                return DXGI_FORMAT_R16G16_SINT;
            case RHI::Format::D32_FLOAT:
                return DXGI_FORMAT_D32_FLOAT;
            case RHI::Format::R32_FLOAT:
                return DXGI_FORMAT_R32_FLOAT;
            case RHI::Format::R32_UINT:
                return DXGI_FORMAT_R32_UINT;
            case RHI::Format::R32_SINT:
                return DXGI_FORMAT_R32_SINT;
            case RHI::Format::D24_UNORM_S8_UINT:
                return DXGI_FORMAT_D24_UNORM_S8_UINT;
            case RHI::Format::R8G8_UNORM:
                return DXGI_FORMAT_R8G8_UNORM;
            case RHI::Format::R8G8_UINT:
                return DXGI_FORMAT_R8G8_UINT;
            case RHI::Format::R8G8_SNORM:
                return DXGI_FORMAT_R8G8_SNORM;
            case RHI::Format::R8G8_SINT:
                return DXGI_FORMAT_R8G8_SINT;
            case RHI::Format::R16_FLOAT:
                return DXGI_FORMAT_R16_FLOAT;
            case RHI::Format::D16_UNORM:
                return DXGI_FORMAT_D16_UNORM;
            case RHI::Format::R16_UNORM:
                return DXGI_FORMAT_R16_UNORM;
            case RHI::Format::R16_UINT:
                return DXGI_FORMAT_R16_UINT;
            case RHI::Format::R16_SNORM:
                return DXGI_FORMAT_R16_SNORM;
            case RHI::Format::R16_SINT:
                return DXGI_FORMAT_R16_SINT;
            case RHI::Format::R8_UNORM:
                return DXGI_FORMAT_R8_UNORM;
            case RHI::Format::R8_UINT:
                return DXGI_FORMAT_R8_UINT;
            case RHI::Format::R8_SNORM:
                return DXGI_FORMAT_R8_SNORM;
            case RHI::Format::R8_SINT:
                return DXGI_FORMAT_R8_SINT;
            case RHI::Format::A8_UNORM:
                return DXGI_FORMAT_A8_UNORM;
            case RHI::Format::R1_UNORM:
                return DXGI_FORMAT_R1_UNORM;
            case RHI::Format::R9G9B9E5_SHAREDEXP:
                return DXGI_FORMAT_R9G9B9E5_SHAREDEXP;
            case RHI::Format::R8G8_B8G8_UNORM:
                return DXGI_FORMAT_R8G8_B8G8_UNORM;
            case RHI::Format::G8R8_G8B8_UNORM:
                return DXGI_FORMAT_G8R8_G8B8_UNORM;
            case RHI::Format::BC1_UNORM:
                return DXGI_FORMAT_BC1_UNORM;
            case RHI::Format::BC1_UNORM_SRGB:
                return DXGI_FORMAT_BC1_UNORM_SRGB;
            case RHI::Format::BC2_UNORM:
                return DXGI_FORMAT_BC2_UNORM;
            case RHI::Format::BC2_UNORM_SRGB:
                return DXGI_FORMAT_BC2_UNORM_SRGB;
            case RHI::Format::BC3_UNORM:
                return DXGI_FORMAT_BC3_UNORM;
            case RHI::Format::BC3_UNORM_SRGB:
                return DXGI_FORMAT_BC3_UNORM_SRGB;
            case RHI::Format::BC4_UNORM:
                return DXGI_FORMAT_BC4_UNORM;
            case RHI::Format::BC4_SNORM:
                return DXGI_FORMAT_BC4_SNORM;
            case RHI::Format::BC5_UNORM:
                return DXGI_FORMAT_BC5_UNORM;
            case RHI::Format::BC5_SNORM:
                return DXGI_FORMAT_BC5_SNORM;
            case RHI::Format::B5G6R5_UNORM:
                return DXGI_FORMAT_B5G6R5_UNORM;
            case RHI::Format::B5G5R5A1_UNORM:
                return DXGI_FORMAT_B5G5R5A1_UNORM;
            case RHI::Format::B8G8R8A8_UNORM:
                return DXGI_FORMAT_B8G8R8A8_UNORM;
            case RHI::Format::B8G8R8X8_UNORM:
                return DXGI_FORMAT_B8G8R8X8_UNORM;
            case RHI::Format::R10G10B10_XR_BIAS_A2_UNORM:
                return DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM;
            case RHI::Format::B8G8R8A8_UNORM_SRGB:
                return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
            case RHI::Format::B8G8R8X8_UNORM_SRGB:
                return DXGI_FORMAT_B8G8R8X8_UNORM_SRGB;
            case RHI::Format::BC6H_UF16:
                return DXGI_FORMAT_BC6H_UF16;
            case RHI::Format::BC6H_SF16:
                return DXGI_FORMAT_BC6H_SF16;
            case RHI::Format::BC7_UNORM:
                return DXGI_FORMAT_BC7_UNORM;
            case RHI::Format::BC7_UNORM_SRGB:
                return DXGI_FORMAT_BC7_UNORM_SRGB;
            case RHI::Format::AYUV:
                return DXGI_FORMAT_AYUV;
            case RHI::Format::Y410:
                return DXGI_FORMAT_Y410;
            case RHI::Format::Y416:
                return DXGI_FORMAT_Y416;
            case RHI::Format::NV12:
                return DXGI_FORMAT_NV12;
            case RHI::Format::P010:
                return DXGI_FORMAT_P010;
            case RHI::Format::P016:
                return DXGI_FORMAT_P016;
            case RHI::Format::YUY2:
                return DXGI_FORMAT_YUY2;
            case RHI::Format::Y210:
                return DXGI_FORMAT_Y210;
            case RHI::Format::Y216:
                return DXGI_FORMAT_Y216;
            case RHI::Format::NV11:
                return DXGI_FORMAT_NV11;
            case RHI::Format::AI44:
                return DXGI_FORMAT_AI44;
            case RHI::Format::IA44:
                return DXGI_FORMAT_IA44;
            case RHI::Format::P8:
                return DXGI_FORMAT_P8;
            case RHI::Format::A8P8:
                return DXGI_FORMAT_A8P8;
            case RHI::Format::B4G4R4A4_UNORM:
                return DXGI_FORMAT_B4G4R4A4_UNORM;
            case RHI::Format::P208:
                return DXGI_FORMAT_P208;
            case RHI::Format::V208:
                return DXGI_FORMAT_V208;
            case RHI::Format::V408:
                return DXGI_FORMAT_V408;

            default:
                return DXGI_FORMAT_UNKNOWN;
            }
        }

        DXGI_SCALING ConvertScaling(RHI::Scaling scaling)
        {
            switch (scaling)
            {
            case RHI::Scaling::None:
                return DXGI_SCALING_NONE;
            case RHI::Scaling::Stretch:
                return DXGI_SCALING_STRETCH;
            case RHI::Scaling::AspectRatioStretch:
                return DXGI_SCALING_ASPECT_RATIO_STRETCH;
            default:
                return DXGI_SCALING_STRETCH;
            }
        }

        D3D12_HEAP_TYPE ConvertHeapType(RHI::HeapMemoryLevel heapMemoryLevel, RHI::HostMemoryAccess hostMemoryAccess)
        {
            switch (heapMemoryLevel)
            {
            case RHI::HeapMemoryLevel::Host:
                switch (hostMemoryAccess)
                {
                case RHI::HostMemoryAccess::Write:
                    return D3D12_HEAP_TYPE_UPLOAD;
                case RHI::HostMemoryAccess::Read:
                    return D3D12_HEAP_TYPE_READBACK;
                };
            case RHI::HeapMemoryLevel::Device:
                return D3D12_HEAP_TYPE_DEFAULT;
            }
            assert(false);
            return D3D12_HEAP_TYPE_CUSTOM;
        }

        D3D12_RESOURCE_STATES ConvertInitialResourceState(RHI::HeapMemoryLevel heapMemoryLevel, RHI::HostMemoryAccess hostMemoryAccess)
        {
            if (heapMemoryLevel == RHI::HeapMemoryLevel::Host)
            {
                return hostMemoryAccess == RHI::HostMemoryAccess::Write ? D3D12_RESOURCE_STATE_GENERIC_READ : D3D12_RESOURCE_STATE_COPY_DEST;
            }
            return D3D12_RESOURCE_STATE_COMMON;
        }

        void ConvertBufferDescriptor(const RHI::BufferDescriptor& descriptor, D3D12_RESOURCE_DESC& resourceDesc)
        {
            resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
            resourceDesc.Alignment = 0;
            resourceDesc.Width = RHI::AlignUp(descriptor.m_byteCount, DX_Alignment::CommittedBuffer);
            resourceDesc.Height = 1;
            resourceDesc.DepthOrArraySize = 1;
            resourceDesc.MipLevels = 1;
            resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
            resourceDesc.SampleDesc = DXGI_SAMPLE_DESC{ 1, 0 };
            resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
            resourceDesc.Flags = ConvertBufferBindFlags(descriptor.m_bindFlags);
        }

        D3D12_RESOURCE_FLAGS ConvertBufferBindFlags(RHI::BufferBindFlags bufferFlags)
        {
            D3D12_RESOURCE_FLAGS resourceFlags = D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE;
            if (RHI::CheckBitsAll(static_cast<uint32_t>(bufferFlags), static_cast<uint32_t>(RHI::BufferBindFlags::ShaderWrite)))
            {
                resourceFlags = RHI::SetBits(resourceFlags, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
            }
            if (RHI::CheckBitsAny(static_cast<uint32_t>(bufferFlags), static_cast<uint32_t>(RHI::BufferBindFlags::ShaderRead)))
            {
                resourceFlags = RHI::ResetBits(resourceFlags, D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE);
            }
            return resourceFlags;
        }

        void ConvertBufferView(const DX_Buffer& buffer, const RHI::BufferViewDescriptor& bufferViewDescriptor, D3D12_SHADER_RESOURCE_VIEW_DESC& shaderResourceView)
        {
            const uint32_t elementOffsetBase = static_cast<uint32_t>(buffer.GetMemoryView().GetOffset()) / bufferViewDescriptor.m_elementSize;
            const uint32_t elementOffset = elementOffsetBase + bufferViewDescriptor.m_elementOffset;
            if (elementOffsetBase * bufferViewDescriptor.m_elementSize != buffer.GetMemoryView().GetOffset())
            {
                assert(false, "SRV: Buffer wasn't aligned with element size. Buffer should be created with proper alignment");
            }

            shaderResourceView = {};
            shaderResourceView.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
            shaderResourceView.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
            shaderResourceView.Format = ConvertFormat(bufferViewDescriptor.m_elementFormat);
            shaderResourceView.Buffer.FirstElement = elementOffset;
            shaderResourceView.Buffer.NumElements = bufferViewDescriptor.m_elementCount;
            if (bufferViewDescriptor.m_elementFormat == RHI::Format::R32_UINT)
            {
                shaderResourceView.Format = DXGI_FORMAT_R32_TYPELESS;
                shaderResourceView.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_RAW;
            }
            else if (shaderResourceView.Format == DXGI_FORMAT_UNKNOWN)
            {
                shaderResourceView.Buffer.StructureByteStride = bufferViewDescriptor.m_elementSize;
            }
        }

        void ConvertBufferView(const DX_Buffer& buffer, const RHI::BufferViewDescriptor& bufferViewDescriptor, D3D12_UNORDERED_ACCESS_VIEW_DESC& unorderedAccessView)
        {
            const uint32_t elementOffsetBase = static_cast<uint32_t>(buffer.GetMemoryView().GetOffset()) / bufferViewDescriptor.m_elementSize;
            const uint32_t elementOffset = elementOffsetBase + bufferViewDescriptor.m_elementOffset;
            if (elementOffsetBase * bufferViewDescriptor.m_elementSize != buffer.GetMemoryView().GetOffset())
            {
                assert(false, "UAV: Buffer wasn't aligned with element size. Buffer should be created with proper alignment");
            }

            unorderedAccessView = {};
            unorderedAccessView.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
            unorderedAccessView.Format = ConvertFormat(bufferViewDescriptor.m_elementFormat);
            unorderedAccessView.Buffer.FirstElement = elementOffset;
            unorderedAccessView.Buffer.NumElements = bufferViewDescriptor.m_elementCount;
            if (bufferViewDescriptor.m_elementFormat == RHI::Format::R32_UINT)
            {
                unorderedAccessView.Format = DXGI_FORMAT_R32_TYPELESS;
                unorderedAccessView.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_RAW;
            }
            else if (unorderedAccessView.Format == DXGI_FORMAT_UNKNOWN)
            {
                unorderedAccessView.Buffer.StructureByteStride = bufferViewDescriptor.m_elementSize;
            }
        }

        void ConvertBufferView(const DX_Buffer& buffer, const RHI::BufferViewDescriptor& bufferViewDescriptor, D3D12_CONSTANT_BUFFER_VIEW_DESC& constantBufferView)
        {
            assert(RHI::IsAligned(buffer.GetMemoryView().GetGpuAddress(), DX_Alignment::Constant), "Constant Buffer memory is not aligned to %d bytes.", DX_Alignment::Constant);

            const uint32_t bufferOffset = bufferViewDescriptor.m_elementOffset * bufferViewDescriptor.m_elementSize;
            assert(RHI::IsAligned(bufferOffset, DX_Alignment::Constant), "Buffer View offset is not aligned to %d bytes, the view won't have the appropiate alignment for Constant Buffer reads.", Alignment::Constant);

            // In DX12 Constant data reads must be a multiple of 256 bytes.
            // It's not a problem if the actual buffer size is smaller since the heap (where the buffer resides) must be multiples of 64KB.
            // This means the buffer view will never go out of heap memory, it might read pass the Constant Buffer size, but it will never be used.
            const uint32_t bufferSize = RHI::AlignUp(bufferViewDescriptor.m_elementCount * bufferViewDescriptor.m_elementSize, DX_Alignment::Constant);

            constantBufferView.BufferLocation = buffer.GetMemoryView().GetGpuAddress() + bufferOffset;
            constantBufferView.SizeInBytes = bufferSize;
        }

        D3D12_CLEAR_VALUE ConvertClearValue(RHI::Format format, RHI::ClearValue clearValue)
        {
            switch (clearValue.m_type)
            {
            case RHI::ClearValueType::DepthStencil:
                return CD3DX12_CLEAR_VALUE(ConvertFormat(format), clearValue.m_depth, clearValue.m_stencil);;
            case RHI::ClearValueType::Vector4Float:
            {
                float color[] =
                {
                    clearValue.m_vector4Float[0],
                    clearValue.m_vector4Float[1],
                    clearValue.m_vector4Float[2],
                    clearValue.m_vector4Float[3]
                };
                return CD3DX12_CLEAR_VALUE(ConvertFormat(format), color);
            }
            case RHI::ClearValueType::Vector4Uint:
                assert(false, "Can't convert unsigned type to DX12 clear value. Use float instead.");
                return CD3DX12_CLEAR_VALUE{};
            }
            return CD3DX12_CLEAR_VALUE{};
        }

        void ConvertImageDescriptor(const RHI::ImageDescriptor& descriptor, D3D12_RESOURCE_DESC& resourceDesc)
        {
            resourceDesc.Dimension = ConvertImageDimension(descriptor.m_dimension);
            resourceDesc.Alignment = 0;
            resourceDesc.Width = descriptor.m_size.m_width;
            resourceDesc.Height = descriptor.m_size.m_height;
            resourceDesc.DepthOrArraySize = static_cast<uint16_t>(resourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE3D ? descriptor.m_size.m_depth : descriptor.m_arraySize);
            resourceDesc.MipLevels = descriptor.m_mipLevels;
            resourceDesc.Format = GetBaseFormat(ConvertFormat(descriptor.m_format));
            resourceDesc.SampleDesc = DXGI_SAMPLE_DESC{ descriptor.m_multisampleState.m_samples, descriptor.m_multisampleState.m_quality };
            resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
            resourceDesc.Flags = ConvertImageBindFlags(descriptor.m_bindFlags);
        }

        DXGI_FORMAT ConvertImageViewFormat(const DX_Image& image, const RHI::ImageViewDescriptor& imageViewDescriptor)
        {
            // unknown format means we're falling back to the image format.
            return imageViewDescriptor.m_overrideFormat != RHI::Format::Unknown ? ConvertFormat(imageViewDescriptor.m_overrideFormat) : ConvertFormat(image.GetDescriptor().m_format);
        }

        D3D12_RESOURCE_DIMENSION ConvertImageDimension(RHI::ImageDimension dimension)
        {
            switch (dimension)
            {
            case RHI::ImageDimension::Image1D:
                return D3D12_RESOURCE_DIMENSION_TEXTURE1D;
            case RHI::ImageDimension::Image2D:
                return D3D12_RESOURCE_DIMENSION_TEXTURE2D;
            case RHI::ImageDimension::Image3D:
                return D3D12_RESOURCE_DIMENSION_TEXTURE3D;

            default:
                assert(false, "failed to convert image type");
                return D3D12_RESOURCE_DIMENSION_TEXTURE2D;
            }
        }

        D3D12_RESOURCE_FLAGS ConvertImageBindFlags(RHI::ImageBindFlags imageFlags)
        {
            D3D12_RESOURCE_FLAGS resourceFlags = D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE;
            if (RHI::CheckBitsAll(static_cast<uint32_t>(imageFlags), static_cast<uint32_t>(RHI::ImageBindFlags::ShaderWrite)))
            {
                resourceFlags = RHI::SetBits(resourceFlags, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
            }
            if (RHI::CheckBitsAll(static_cast<uint32_t>(imageFlags), static_cast<uint32_t>(RHI::ImageBindFlags::ShaderRead)))
            {
                resourceFlags = RHI::ResetBits(resourceFlags, D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE);
            }
            if (RHI::CheckBitsAll(static_cast<uint32_t>(imageFlags), static_cast<uint32_t>(RHI::ImageBindFlags::Color)))
            {
                resourceFlags = RHI::SetBits(resourceFlags, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);
            }
            if (RHI::CheckBitsAny(static_cast<uint32_t>(imageFlags), static_cast<uint32_t>(RHI::ImageBindFlags::DepthStencil)))
            {
                resourceFlags = RHI::SetBits(resourceFlags, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
            }
            else
            {
                resourceFlags = RHI::ResetBits(resourceFlags, D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE);
            }
            return resourceFlags;
        }

        void ConvertImageView(const DX_Image& image, const RHI::ImageViewDescriptor& imageViewDescriptor, D3D12_RENDER_TARGET_VIEW_DESC& renderTargetView)
        {
            const RHI::ImageDescriptor& imageDescriptor = image.GetDescriptor();

            renderTargetView = {};
            renderTargetView.Format = ConvertImageViewFormat(image, imageViewDescriptor);

            const bool bIsArray = imageDescriptor.m_arraySize > 1 || imageViewDescriptor.m_isArray;
            const bool bIsMsaa = imageDescriptor.m_multisampleState.m_samples > 1;

            uint32_t ArraySize = (imageViewDescriptor.m_arraySliceMax - imageViewDescriptor.m_arraySliceMin) + 1;
            ArraySize = std::min<uint32_t>(ArraySize, imageDescriptor.m_arraySize);

            switch(imageDescriptor.m_dimension)
            {
            case RHI::ImageDimension::Image1D:
                if(bIsArray)
                {
                    renderTargetView.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE1DARRAY;
                    renderTargetView.Texture1DArray.ArraySize = static_cast<uint16_t>(ArraySize);
                    renderTargetView.Texture1DArray.FirstArraySlice = imageViewDescriptor.m_arraySliceMin;
                    renderTargetView.Texture1DArray.MipSlice = imageViewDescriptor.m_mipSliceMin;
                }
                else
                {
                    renderTargetView.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE1D;
                    renderTargetView.Texture1D.MipSlice = imageViewDescriptor.m_mipSliceMin;
                }
                break;
            case RHI::ImageDimension::Image2D:
                if(bIsArray)
                {
                    if(bIsMsaa)
                    {
                        renderTargetView.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMSARRAY;
                        renderTargetView.Texture2DMSArray.ArraySize = static_cast<uint16_t>(ArraySize);
                        renderTargetView.Texture2DMSArray.FirstArraySlice = imageViewDescriptor.m_arraySliceMin;
                        // MipSlice is not set because multi-sample textures do not have mipmaps
                    }
                    else
                    {
                        renderTargetView.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
                        renderTargetView.Texture2DArray.ArraySize = static_cast<uint16_t>(ArraySize);
                        renderTargetView.Texture2DArray.FirstArraySlice = imageViewDescriptor.m_arraySliceMin;
                        renderTargetView.Texture2DArray.MipSlice = imageViewDescriptor.m_mipSliceMin;
                    }
                }
                else
                {
                    if(bIsMsaa)
                    {
                        renderTargetView.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMS;
                    }
                    else
                    {
                        renderTargetView.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
                        renderTargetView.Texture2D.MipSlice = imageViewDescriptor.m_mipSliceMin;
                    }
                }
                break;
            case RHI::ImageDimension::Image3D:
                renderTargetView.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE3D;
                renderTargetView.Texture3D.MipSlice = imageViewDescriptor.m_mipSliceMin;
                renderTargetView.Texture3D.FirstWSlice = imageViewDescriptor.m_depthSliceMin;

                if (imageViewDescriptor.m_depthSliceMax == RHI::ImageViewDescriptor::HighestSliceIndex)
                {
                    renderTargetView.Texture3D.WSize = std::numeric_limits<UINT>::max();
                }
                else
                {
                    renderTargetView.Texture3D.WSize = (imageViewDescriptor.m_depthSliceMax - imageViewDescriptor.m_depthSliceMin) + 1;
                }
                break;
            default:
                assert(false, "Image dimension error.");
            }
        }

        void ConvertImageView(const DX_Image& image, const RHI::ImageViewDescriptor& imageViewDescriptor, D3D12_DEPTH_STENCIL_VIEW_DESC& depthStencilView)
        {
            const RHI::ImageDescriptor& imageDescriptor = image.GetDescriptor();

            depthStencilView = {};
            depthStencilView.Format = GetDSVFormat(ConvertImageViewFormat(image, imageViewDescriptor));

            const bool bIsArray = imageDescriptor.m_arraySize > 1 || imageViewDescriptor.m_isArray;;
            const bool bIsMsaa = imageDescriptor.m_multisampleState.m_samples > 1;

            uint32_t ArraySize = (imageViewDescriptor.m_arraySliceMax - imageViewDescriptor.m_arraySliceMin) + 1;
            ArraySize = std::min<uint32_t>(ArraySize, imageDescriptor.m_arraySize);

            switch (imageDescriptor.m_dimension)
            {
            case RHI::ImageDimension::Image2D:
                if (bIsArray)
                {
                    if (bIsMsaa)
                    {
                        depthStencilView.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMSARRAY;
                        depthStencilView.Texture2DMSArray.ArraySize = static_cast<uint16_t>(ArraySize);
                        depthStencilView.Texture2DMSArray.FirstArraySlice = imageViewDescriptor.m_arraySliceMin;
                    }
                    else
                    {
                        depthStencilView.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
                        depthStencilView.Texture2DArray.ArraySize = static_cast<uint16_t>(ArraySize);
                        depthStencilView.Texture2DArray.FirstArraySlice = imageViewDescriptor.m_arraySliceMin;
                        depthStencilView.Texture2DArray.MipSlice = imageViewDescriptor.m_mipSliceMin;
                    }
                }
                else
                {
                    if (bIsMsaa)
                    {
                        depthStencilView.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMS;
                    }
                    else
                    {
                        depthStencilView.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
                        depthStencilView.Texture2D.MipSlice = imageViewDescriptor.m_mipSliceMin;
                    }
                }
                break;

            default:
                assert(false, "unsupported");
            }
        }

        void ConvertImageView(const DX_Image& image, const RHI::ImageViewDescriptor& imageViewDescriptor, D3D12_SHADER_RESOURCE_VIEW_DESC& shaderResourceView)
        {
            const RHI::ImageDescriptor& imageDescriptor = image.GetDescriptor();

            shaderResourceView = {};
            shaderResourceView.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
            shaderResourceView.Format = GetSRVFormat(ConvertImageViewFormat(image, imageViewDescriptor));

            const bool bIsArray = imageDescriptor.m_arraySize > 1 || imageViewDescriptor.m_isArray;;
            const bool bIsMsaa = imageDescriptor.m_multisampleState.m_samples > 1;
            const bool bIsCubemap = imageViewDescriptor.m_isCubemap != 0;

            uint32_t ArraySize = (imageViewDescriptor.m_arraySliceMax - imageViewDescriptor.m_arraySliceMin) + 1;
            ArraySize = std::min<uint32_t>(ArraySize, imageDescriptor.m_arraySize);

            assert(imageViewDescriptor.m_mipSliceMax < imageDescriptor.m_mipLevels, "ImageViewDescriptor specifies a mipSliceMax must be strictly smaller than the mip level count.");
            uint32_t mipLevelCount = (imageViewDescriptor.m_mipSliceMax - imageViewDescriptor.m_mipSliceMin) + 1;

            switch (imageDescriptor.m_dimension)
            {
            case RHI::ImageDimension::Image1D:
                if (bIsArray)
                {
                    shaderResourceView.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1DARRAY;
                    shaderResourceView.Texture1DArray.ArraySize = static_cast<uint16_t>(ArraySize);
                    shaderResourceView.Texture1DArray.FirstArraySlice = imageViewDescriptor.m_arraySliceMin;
                    shaderResourceView.Texture1DArray.MipLevels = mipLevelCount;
                    shaderResourceView.Texture1DArray.MostDetailedMip = imageViewDescriptor.m_mipSliceMin;
                }
                else
                {
                    shaderResourceView.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1D;
                    shaderResourceView.Texture1D.MipLevels = mipLevelCount;
                    shaderResourceView.Texture1D.MostDetailedMip = imageViewDescriptor.m_mipSliceMin;
                }
                break;
            case RHI::ImageDimension::Image2D:
                if (bIsArray)
                {
                    if (bIsMsaa)
                    {
                        shaderResourceView.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMSARRAY;
                        shaderResourceView.Texture2DMSArray.ArraySize = static_cast<uint16_t>(ArraySize);
                        shaderResourceView.Texture2DMSArray.FirstArraySlice = imageViewDescriptor.m_arraySliceMin;
                    }
                    else if (bIsCubemap)
                    {
                        uint32_t cubeSliceCount = (ArraySize / 6);
                        if (cubeSliceCount > 1)
                        {
                            shaderResourceView.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBEARRAY;
                            shaderResourceView.TextureCubeArray.First2DArrayFace = imageViewDescriptor.m_arraySliceMin;
                            shaderResourceView.TextureCubeArray.MipLevels = mipLevelCount;
                            shaderResourceView.TextureCubeArray.MostDetailedMip = imageViewDescriptor.m_mipSliceMin;
                            shaderResourceView.TextureCubeArray.NumCubes = cubeSliceCount;
                        }
                        else
                        {
                            shaderResourceView.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
                            shaderResourceView.TextureCube.MipLevels = mipLevelCount;
                            shaderResourceView.TextureCube.MostDetailedMip = imageViewDescriptor.m_mipSliceMin;
                        }
                    }
                    else
                    {
                        shaderResourceView.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
                        shaderResourceView.Texture2DArray.ArraySize = static_cast<uint16_t>(ArraySize);
                        shaderResourceView.Texture2DArray.FirstArraySlice = imageViewDescriptor.m_arraySliceMin;
                        shaderResourceView.Texture2DArray.MipLevels = mipLevelCount;
                        shaderResourceView.Texture2DArray.MostDetailedMip = imageViewDescriptor.m_mipSliceMin;
                    }
                }
                else
                {
                    if (bIsMsaa)
                    {
                        shaderResourceView.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMS;
                    }
                    else
                    {
                        shaderResourceView.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
                        shaderResourceView.Texture2D.MipLevels = mipLevelCount;
                        shaderResourceView.Texture2D.MostDetailedMip = imageViewDescriptor.m_mipSliceMin;
                    }
                }
                break;
            case RHI::ImageDimension::Image3D:
                shaderResourceView.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;
                shaderResourceView.Texture3D.MipLevels = mipLevelCount;
                shaderResourceView.Texture3D.MostDetailedMip = imageViewDescriptor.m_mipSliceMin;
                break;
            default:
                assert(false, "Image dimension error.");
            }
        }

        void ConvertImageView(const DX_Image& image, const RHI::ImageViewDescriptor& imageViewDescriptor, D3D12_UNORDERED_ACCESS_VIEW_DESC& unorderedAccessView)
        {
            const RHI::ImageDescriptor& imageDescriptor = image.GetDescriptor();

            unorderedAccessView = {};
            unorderedAccessView.Format = GetUAVFormat(ConvertImageViewFormat(image, imageViewDescriptor));

            const bool bIsArray = imageDescriptor.m_arraySize > 1 || imageViewDescriptor.m_isArray;;
            uint32_t ArraySize = (imageViewDescriptor.m_arraySliceMax - imageViewDescriptor.m_arraySliceMin) + 1;
            ArraySize = std::min<uint32_t>(ArraySize, imageDescriptor.m_arraySize);

            switch (imageDescriptor.m_dimension)
            {
            case RHI::ImageDimension::Image1D:
                if (bIsArray)
                {
                    unorderedAccessView.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE1DARRAY;
                    unorderedAccessView.Texture1DArray.ArraySize = static_cast<uint16_t>(ArraySize);
                    unorderedAccessView.Texture1DArray.FirstArraySlice = imageViewDescriptor.m_arraySliceMin;
                    unorderedAccessView.Texture1DArray.MipSlice = imageViewDescriptor.m_mipSliceMin;
                }
                else
                {
                    unorderedAccessView.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE1D;
                    unorderedAccessView.Texture1D.MipSlice = imageViewDescriptor.m_mipSliceMin;
                }
                break;
            case RHI::ImageDimension::Image2D:
                if (bIsArray)
                {
                    unorderedAccessView.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
                    unorderedAccessView.Texture2DArray.ArraySize = static_cast<uint16_t>(ArraySize);
                    unorderedAccessView.Texture2DArray.FirstArraySlice = imageViewDescriptor.m_arraySliceMin;
                    unorderedAccessView.Texture2DArray.MipSlice = imageViewDescriptor.m_mipSliceMin;
                }
                else
                {
                    unorderedAccessView.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
                    unorderedAccessView.Texture2D.MipSlice = imageViewDescriptor.m_mipSliceMin;
                }
                break;
            case RHI::ImageDimension::Image3D:
                unorderedAccessView.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE3D;
                unorderedAccessView.Texture3D.MipSlice = imageViewDescriptor.m_mipSliceMin;
                unorderedAccessView.Texture3D.FirstWSlice = imageViewDescriptor.m_depthSliceMin;

                if (imageViewDescriptor.m_depthSliceMax == RHI::ImageViewDescriptor::HighestSliceIndex)
                {
                    unorderedAccessView.Texture3D.WSize = std::numeric_limits<UINT>::max();
                }
                else
                {
                    unorderedAccessView.Texture3D.WSize = (imageViewDescriptor.m_depthSliceMax - imageViewDescriptor.m_depthSliceMin) + 1;
                }
                break;
            default:
                assert(false, "Image dimension error.");
            }
        }

        uint16_t ConvertImageAspectToPlaneSlice(RHI::ImageAspect aspect)
        {
            switch (aspect)
            {
            case RHI::ImageAspect::Color:
            case RHI::ImageAspect::Depth:
                return 0;
            case RHI::ImageAspect::Stencil:
                return 1;
            default:
                assert(false, "Invalid image aspect");
                return 0;
            }
        }
    }
}