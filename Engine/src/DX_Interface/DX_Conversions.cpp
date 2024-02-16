// DX12
#include "DX_Conversions.h"
#include "DX_CommonHeaders.h"

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
    }
}