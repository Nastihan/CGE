
// DX12
#include "DX_ShaderResourceGroup.h"
#include "DX_Device.h"
#include "DX_Conversions.h"
#include "DX_BufferView.h"
#include "DX_ImageView.h"

namespace CGE
{
    namespace DX12
    {
        RHI::Ptr<DX_ShaderResourceGroup> DX_ShaderResourceGroup::Create()
        {
            return new DX_ShaderResourceGroup();
        }

        const DX_ShaderResourceGroupCompiledData& DX_ShaderResourceGroup::GetCompiledData() const
        {
            return m_compiledData[m_compiledDataIndex];
        }

        RHI::ResultCode DX_ShaderResourceGroup::InitInternal(const RHI::ShaderResourceGroupData& shaderResourceGroupData)
        {
            // Buffers occupy the first region of the table, then images.
            m_descriptorTableBufferOffset = 0;
            m_descriptorTableImageOffset = GetLayout()->GetGroupSizeForBuffers();
            m_descriptorContext = &static_cast<DX_Device&>(GetDevice()).GetDescriptorContext();
            return RHI::ResultCode::Success;
        }

        RHI::ResultCode DX_ShaderResourceGroup::CompileInternal()
        {
            typedef RHI::ShaderResourceGroupData::ResourceTypeMask ResourceMask;
            m_compiledDataIndex = (m_compiledDataIndex + 1) % RHI::Limits::Device::FrameCountMax;

            if (GetLayout()->GetShaderInputConstant().has_value() && GetData().GetConstantDataPointer())
            {
                m_compiledData[m_compiledDataIndex].m_rootConstant = GetData().GetConstantDataPointer();
                m_compiledData[m_compiledDataIndex].m_rootConstantSize = GetLayout()->GetShaderInputConstant()->m_constantByteCount;
            }

            // Check to see if we have any CBV_SRV_UAV views. If so we need to build the table which holds the GPU visible continuous descriptors.
            // Each srg will get one table in the CBV_SRV_UAV GPU visible heap all the ranges will get built continuously.
            // DX_PipelineLayout::Init uses D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND for the ranges.
            uint32_t viewsDescriptorTableSize = GetLayout()->GetGroupSizeForBuffers() + GetLayout()->GetGroupSizeForImages();;
            if (viewsDescriptorTableSize)
            {
                // If we dont have a table set we initilize the handles first the size will be viewsDescriptorTableSize * RHI::Limits::Device::FrameCountMax to
                // account for triple buffering system.
                if (!m_viewsDescriptorTable.IsValid())
                {
                    uint32_t viewsDescriptorTableRingSize = viewsDescriptorTableSize * RHI::Limits::Device::FrameCountMax;
                    m_viewsDescriptorTable = m_descriptorContext->CreateDescriptorTable(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, viewsDescriptorTableRingSize);
                    if (!m_viewsDescriptorTable.IsValid())
                    {
                        assert(false, "Descriptor heap ran out of memory.");
                        return RHI::ResultCode::OutOfMemory;
                    }
                    // Cache the GPU handles (D3D12_GPU_DESCRIPTOR_HANDLE) in the compiled data which will be set on the command list.
                    for (uint32_t i = 0; i < RHI::Limits::Device::FrameCountMax; ++i)
                    {
                        const DX_DescriptorHandle descriptorHandle = m_viewsDescriptorTable.GetOffset() + viewsDescriptorTableSize * i;

                        DX_ShaderResourceGroupCompiledData& compiledData = m_compiledData[i];
                        compiledData.m_gpuViewsDescriptorHandle = m_descriptorContext->GetGpuPlatformHandleForTable(
                            DX_DescriptorTable(descriptorHandle, static_cast<uint16_t>(viewsDescriptorTableSize)));
                    }
                }

                // Now its time to update the descriptors in the table from the views in ShaderResourceGroupData. (Copy the CPU only visible ones to the table location)
                // First get a handle to the table based on the frame number.
                const DX_DescriptorTable descriptorTable(m_viewsDescriptorTable.GetOffset() + m_compiledDataIndex * viewsDescriptorTableSize,
                    static_cast<uint16_t>(viewsDescriptorTableSize));
                const RHI::ShaderResourceGroupLayout& groupLayout = *GetLayout();
                uint32_t shaderInputIndex = 0;

                // Update the buffer views
                if (IsResourceTypeEnabledForCompilation(static_cast<uint32_t>(ResourceMask::BufferViewMask)))
                {
                    for (const RHI::ShaderInputBufferDescriptor& shaderInputBuffer : groupLayout.GetShaderInputListForBuffers())
                    {
                        // Gather all the cpu handles based on the layouts need.
                        const RHI::ShaderInputBufferIndex bufferInputIndex(shaderInputIndex);

                        // The resource view array in the srg data is a flat array we get a handle to a range based on the layouts at the specific index.
                        std::span<const RHI::ConstPtr<RHI::BufferView>> bufferViews = GetData().GetBufferViewArray(bufferInputIndex);
                        D3D12_DESCRIPTOR_RANGE_TYPE descriptorRangeType = ConvertShaderInputBufferAccess(shaderInputBuffer.m_access);
                        std::vector<DX_DescriptorHandle> descriptorHandles;
                        switch (descriptorRangeType)
                        {
                        case D3D12_DESCRIPTOR_RANGE_TYPE_SRV:
                        {
                            descriptorHandles = GetSRVsFromImageViews<RHI::BufferView, DX_BufferView>(bufferViews, D3D12_SRV_DIMENSION_BUFFER);
                            break;
                        }
                        case D3D12_DESCRIPTOR_RANGE_TYPE_UAV:
                        {
                            descriptorHandles = GetUAVsFromImageViews<RHI::BufferView, DX_BufferView>(bufferViews, D3D12_UAV_DIMENSION_BUFFER);
                            break;
                        }
                        case D3D12_DESCRIPTOR_RANGE_TYPE_CBV:
                        {
                            descriptorHandles = GetCBVsFromBufferViews(bufferViews);
                            break;
                        }
                        default:
                            assert(false, "Unhandled D3D12_DESCRIPTOR_RANGE_TYPE enumeration");
                            break;
                        }

                        // Finally map to the table based on the layout interval and copy the collected descriptors.
                        const RHI::Interval interval = GetLayout()->GetGroupInterval(bufferInputIndex);
                        const DX_DescriptorHandle startHandle = descriptorTable[m_descriptorTableBufferOffset + interval.m_min];
                        const DX_DescriptorTable gpuDestinationTable(startHandle, interval.m_max - interval.m_min);
                        m_descriptorContext->UpdateDescriptorTableRange(gpuDestinationTable, descriptorHandles.data(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
                        ++shaderInputIndex;
                    }
                }

                // Update the image views
                if (IsResourceTypeEnabledForCompilation(static_cast<uint32_t>(ResourceMask::ImageViewMask)))
                {
                    shaderInputIndex = 0;
                    for (const RHI::ShaderInputImageDescriptor& shaderInputImage : groupLayout.GetShaderInputListForImages())
                    {
                        // Gather all the cpu handles based on the layouts need.
                        const RHI::ShaderInputImageIndex imageInputIndex(shaderInputIndex);

                        // The resource view array in the srg data is a flat array we get a handle to a range based on the layouts at the specific index.
                        std::span<const RHI::ConstPtr<RHI::ImageView>> imageViews = GetData().GetImageViewArray(imageInputIndex);
                        D3D12_DESCRIPTOR_RANGE_TYPE descriptorRangeType = ConvertShaderInputImageAccess(shaderInputImage.m_access);
                        std::vector<DX_DescriptorHandle> descriptorHandles;

                        switch (descriptorRangeType)
                        {
                        case D3D12_DESCRIPTOR_RANGE_TYPE_SRV:
                        {
                            descriptorHandles =
                                GetSRVsFromImageViews<RHI::ImageView, DX_ImageView>(imageViews, ConvertSRVDimension(shaderInputImage.m_type));
                            break;
                        }
                        case D3D12_DESCRIPTOR_RANGE_TYPE_UAV:
                        {
                            descriptorHandles =
                                GetUAVsFromImageViews<RHI::ImageView, DX_ImageView>(imageViews, ConvertUAVDimension(shaderInputImage.m_type));
                            break;
                        }
                        default:
                            assert(false, "Unhandled D3D12_DESCRIPTOR_RANGE_TYPE enumeration");
                            break;
                        }

                        // Finally map to the table based on the layout interval and copy the collected descriptors.
                        const RHI::Interval interval = GetLayout()->GetGroupInterval(imageInputIndex);
                        const DX_DescriptorHandle startHandle = descriptorTable[m_descriptorTableImageOffset + interval.m_min];
                        const DX_DescriptorTable gpuDestinationTable(startHandle, interval.m_max - interval.m_min);
                        m_descriptorContext->UpdateDescriptorTableRange(gpuDestinationTable, descriptorHandles.data(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
                        ++shaderInputIndex;
                    }
                }
            }
        }

        template<typename T, typename U>
        std::vector<DX_DescriptorHandle> DX_ShaderResourceGroup::GetSRVsFromImageViews(const std::span<const RHI::ConstPtr<T>>& imageViews, D3D12_SRV_DIMENSION dimension)
        {
            std::vector<DX_DescriptorHandle> cpuSourceDescriptors(imageViews.size(), m_descriptorContext->GetNullHandleSRV(dimension));
            for (size_t i = 0; i < cpuSourceDescriptors.size(); ++i)
            {
                if (imageViews[i])
                {
                    cpuSourceDescriptors[i] = static_cast<const U*>(imageViews[i].get())->GetReadDescriptor();
                }
            }
            return cpuSourceDescriptors;
        }

        template<typename T, typename U>
        std::vector<DX_DescriptorHandle> DX_ShaderResourceGroup::GetUAVsFromImageViews(const std::span<const RHI::ConstPtr<T>>& imageViews, D3D12_UAV_DIMENSION dimension)
        {
            std::vector<DX_DescriptorHandle> cpuSourceDescriptors(imageViews.size(), m_descriptorContext->GetNullHandleUAV(dimension));
            for (size_t i = 0; i < cpuSourceDescriptors.size(); ++i)
            {
                if (imageViews[i])
                {
                    cpuSourceDescriptors[i] = static_cast<const U*>(imageViews[i].get())->GetReadWriteDescriptor();
                }
            }
            return cpuSourceDescriptors;
        }

        std::vector<DX_DescriptorHandle> DX_ShaderResourceGroup::GetCBVsFromBufferViews(const std::span<const RHI::ConstPtr<RHI::BufferView>>& bufferViews)
        {
            std::vector<DX_DescriptorHandle> cpuSourceDescriptors(bufferViews.size(), m_descriptorContext->GetNullHandleCBV());
            for (size_t i = 0; i < bufferViews.size(); ++i)
            {
                if (bufferViews[i])
                {
                    cpuSourceDescriptors[i] = static_cast<const DX_BufferView*>(bufferViews[i].get())->GetConstantDescriptor();
                }
            }
            return cpuSourceDescriptors;
        }
    }
}