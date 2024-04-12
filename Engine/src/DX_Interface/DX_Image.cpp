
// DX12
#include "DX_Image.h"
#include "DX_Conversions.h"
#include <d3dx12.h>

namespace CGE
{
	namespace DX12
	{
        RHI::Ptr<DX_Image> DX_Image::Create()
        {
            return new DX_Image();
        }

        const DX_MemoryView& DX_Image::GetMemoryView() const
        {
            return m_memoryView;
        }

        DX_MemoryView& DX_Image::GetMemoryView()
        {
            return m_memoryView;
        }

        void DX_Image::FinalizeAsyncUpload(uint32_t newStreamedMipLevels)
        {
            assert(newStreamedMipLevels <= m_streamedMipLevel, "Expanded mip levels can't be more than streamed mip level");

            if (newStreamedMipLevels < m_streamedMipLevel)
            {
                m_streamedMipLevel = newStreamedMipLevels;
                InvalidateViews();
            }
        }

        uint32_t DX_Image::GetStreamedMipLevel() const
        {
            return m_streamedMipLevel;
        }

        void DX_Image::SetStreamedMipLevel(uint32_t streamedMipLevel)
        {
            if (m_streamedMipLevel != streamedMipLevel)
            {
                m_streamedMipLevel = streamedMipLevel;
                InvalidateViews();
            }
        }

        void DX_Image::SetUploadFenceValue(uint64_t fenceValue)
        {
            assert(fenceValue > m_uploadFenceValue, "New fence value should always larger than previous fence value");
            m_uploadFenceValue = fenceValue;
        }

        uint64_t DX_Image::GetUploadFenceValue() const
        {
            return m_uploadFenceValue;
        }

        std::vector<DX_Image::SubresourceRangeAttachmentState> DX_Image::GetAttachmentStateByRange(const RHI::ImageSubresourceRange* range) const
        {
            return m_attachmentState.Get(range ? *range : RHI::ImageSubresourceRange(GetDescriptor()));
        }

        void DX_Image::SetAttachmentState(D3D12_RESOURCE_STATES state, const RHI::ImageSubresourceRange* range)
        {
            m_attachmentState.Set(range ? *range : RHI::ImageSubresourceRange(GetDescriptor()), state);
        }

        std::vector<DX_Image::SubresourceAttachmentState> DX_Image::GetAttachmentStateByIndex(const RHI::ImageSubresourceRange* range) const
        {
            std::vector<SubresourceAttachmentState> result;
            auto initialStatesRange = GetAttachmentStateByRange(range);
            if (initialStatesRange.empty())
            {
                return result;
            }

            // First check if we can use the D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES index by counting
            // the subresources that have the same attachment state.
            // This flag is used in D3D12_RESOURCE_TRANSITION_BARRIER::Subresource member and setting it will transition all the subresources.
            uint32_t subresourcesCount = 0;
            D3D12_RESOURCE_STATES lastState = initialStatesRange.front().m_property;
            for (const auto& initialState : initialStatesRange)
            {
                // Check if the new subresource range has the same attachment state as all previous subresources.
                // If it's different, then we just exit because we can't use the D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES index.
                if (lastState != initialState.m_property)
                {
                    break;
                }

                uint32_t mipCount = initialState.m_range.m_mipSliceMax - initialState.m_range.m_mipSliceMin + 1;
                uint32_t arraySize = initialState.m_range.m_arraySliceMax - initialState.m_range.m_arraySliceMin + 1;
                uint32_t planeCount = RHI::CountBitsSet(static_cast<uint32_t>(GetAspectFlags() & initialState.m_range.m_aspectFlags));
                subresourcesCount += planeCount * mipCount * arraySize;
            }

            // Compare the subresource count to the total subresources of the image to see if we can use the D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES special index.
            const auto& descriptor = GetDescriptor();
            uint32_t totalSubresources = descriptor.m_arraySize * descriptor.m_mipLevels * RHI::CountBitsSet(static_cast<uint32_t>(GetAspectFlags()));
            if (totalSubresources == subresourcesCount)
            {
                SubresourceAttachmentState state{};
                state.m_state = lastState;
                state.m_subresourceIndex = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
                return { state };
            }

            // Since not all subresources have the same attachment state, we need to add each subresource as a separate entry.
            for (const auto& initialState : initialStatesRange)
            {
                const RHI::ImageSubresourceRange& subresourceRange = initialState.m_range;
                for (uint16_t aspectIndex = 0; aspectIndex < RHI::ImageAspectCount; ++aspectIndex)
                {
                    if (RHI::CheckBitsAll(static_cast<uint32_t>(subresourceRange.m_aspectFlags), static_cast<uint32_t>(static_cast<RHI::ImageAspectFlags>(1u << aspectIndex))))
                    {
                        uint16_t planeSlice = ConvertImageAspectToPlaneSlice(static_cast<RHI::ImageAspect>(aspectIndex));
                        for (uint16_t mipLevel = subresourceRange.m_mipSliceMin; mipLevel < subresourceRange.m_mipSliceMax + 1; ++mipLevel)
                        {
                            for (uint16_t arraySlice = subresourceRange.m_arraySliceMin; arraySlice < subresourceRange.m_arraySliceMax + 1; ++arraySlice)
                            {
                                result.emplace_back();
                                SubresourceAttachmentState& subresourceState = result.back();
                                subresourceState.m_state = initialState.m_property;
                                // A buffer is an unstructured resourceand is therefore defined as containing a single subresource.
                                // APIs that take buffers do not need a subresource index.A texture on the other hand is highly structured.
                                // Each texture object may contain one or more subresources depending on the size of the array and the number of mipmap levels.
                                subresourceState.m_subresourceIndex = D3D12CalcSubresource(mipLevel, arraySlice, planeSlice, descriptor.m_mipLevels, descriptor.m_arraySize);
                            }
                        }
                    }
                }
            }
            return result;
        }

        D3D12_RESOURCE_STATES DX_Image::GetInitialResourceState() const
        {
            return m_initialResourceState;
        }

        void DX_Image::SetNameInternal(const std::string& name)
        {
            m_memoryView.SetName(name);
        }

        void DX_Image::GetSubresourceLayoutsInternal(const RHI::ImageSubresourceRange& subresourceRange, RHI::ImageSubresourceLayout* subresourceLayouts, size_t* totalSizeInBytes) const
        {
            const RHI::ImageDescriptor& imageDescriptor = GetDescriptor();
            uint32_t byteOffset = 0;

            if (subresourceLayouts)
            {
                for (uint16_t arraySlice = subresourceRange.m_arraySliceMin; arraySlice <= subresourceRange.m_arraySliceMax; ++arraySlice)
                {
                    for (uint16_t mipSlice = subresourceRange.m_mipSliceMin; mipSlice <= subresourceRange.m_mipSliceMax; ++mipSlice)
                    {
                        const RHI::ImageSubresourceLayout& subresourceLayout = m_subresourceLayoutsPerMipChain[mipSlice];
                        const uint32_t subresourceIndex = RHI::GetImageSubresourceIndex(mipSlice, arraySlice, imageDescriptor.m_mipLevels);
                        subresourceLayouts[subresourceIndex] = subresourceLayout;
                        subresourceLayouts[subresourceIndex].m_offset = byteOffset;
                        byteOffset = RHI::AlignUp(byteOffset + subresourceLayout.m_bytesPerImage * subresourceLayout.m_size.m_depth, D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT);
                    }
                }
            }
            else
            {
                for (uint16_t arraySlice = subresourceRange.m_arraySliceMin; arraySlice <= subresourceRange.m_arraySliceMax; ++arraySlice)
                {
                    for (uint16_t mipSlice = subresourceRange.m_mipSliceMin; mipSlice <= subresourceRange.m_mipSliceMax; ++mipSlice)
                    {
                        const RHI::ImageSubresourceLayout& subresourceLayout = m_subresourceLayoutsPerMipChain[mipSlice];
                        byteOffset = RHI::AlignUp(byteOffset + subresourceLayout.m_bytesPerImage * subresourceLayout.m_size.m_depth, D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT);
                    }
                }
            }

            if (totalSizeInBytes)
            {
                *totalSizeInBytes = byteOffset;
            }
        }

        bool DX_Image::IsStreamableInternal() const
        {
            // [todo] Impelemnt the tile map API's
            return false;
        }

        void DX_Image::SetDescriptor(const RHI::ImageDescriptor& descriptor)
        {
            RHI::Image::SetDescriptor(descriptor);
            m_initialResourceState = D3D12_RESOURCE_STATE_COMMON;
            const RHI::ImageBindFlags bindFlags = descriptor.m_bindFlags;

            // Write only states
            const bool renderTarget = RHI::CheckBitsAny(static_cast<uint32_t>(bindFlags), static_cast<uint32_t>(RHI::ImageBindFlags::Color));
            const bool copyDest = RHI::CheckBitsAny(static_cast<uint32_t>(bindFlags), static_cast<uint32_t>(RHI::ImageBindFlags::CopyWrite));
            const bool depthTarget = RHI::CheckBitsAny(static_cast<uint32_t>(bindFlags), static_cast<uint32_t>(RHI::ImageBindFlags::DepthStencil));

            // Read Only States
            const bool shaderResource = RHI::CheckBitsAny(static_cast<uint32_t>(bindFlags), static_cast<uint32_t>(RHI::ImageBindFlags::ShaderRead));
            const bool copySource = RHI::CheckBitsAny(static_cast<uint32_t>(bindFlags), static_cast<uint32_t>(RHI::ImageBindFlags::CopyRead));

            const bool writeState = renderTarget || copyDest || depthTarget;
            const bool readState = shaderResource || copySource;

            // If any write only state is set, only write only resource states can be applied
            if (writeState)
            {
                if (renderTarget)
                {
                    m_initialResourceState |= D3D12_RESOURCE_STATE_RENDER_TARGET;
                }
                else if (copyDest)
                {
                    m_initialResourceState |= D3D12_RESOURCE_STATE_COPY_DEST;
                }
                else if (depthTarget)
                {
                    m_initialResourceState |= D3D12_RESOURCE_STATE_DEPTH_WRITE;
                }
            }
            // If any read only state is set, only read only resource states can be applied
            else if (readState)
            {
                if (shaderResource)
                {
                    if (RHI::CheckBitsAny(static_cast<uint32_t>(descriptor.m_sharedQueueMask), static_cast<uint32_t>(RHI::HardwareQueueClassMask::Graphics)))
                    {
                        m_initialResourceState |= D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
                    }
                    if (RHI::CheckBitsAny(static_cast<uint32_t>(descriptor.m_sharedQueueMask), static_cast<uint32_t>(RHI::HardwareQueueClassMask::Compute)))
                    {
                        m_initialResourceState |= D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
                    }
                }

                if (copySource)
                {
                    m_initialResourceState |= D3D12_RESOURCE_STATE_COPY_SOURCE;
                }
            }
            // If neither a read only or write only state is set, we can set a read/write state
            else
            {
                if (RHI::CheckBitsAny(static_cast<uint32_t>(bindFlags), static_cast<uint32_t>(RHI::ImageBindFlags::ShaderWrite)))
                {
                    m_initialResourceState |= D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
                }
            }

            m_attachmentState.Init(descriptor);
            SetAttachmentState(m_initialResourceState);
        }

        void DX_Image::GenerateSubresourceLayouts()
        {
            for (uint16_t mipSlice = 0; mipSlice < GetDescriptor().m_mipLevels; ++mipSlice)
            {
                RHI::ImageSubresourceLayout& subresourceLayout = m_subresourceLayoutsPerMipChain[mipSlice];

                RHI::ImageSubresource subresource;
                subresource.m_mipSlice = mipSlice;
                subresourceLayout = RHI::GetImageSubresourceLayout(GetDescriptor(), subresource);

                // Align the row size to match the DX12 row pitch alignment.
                subresourceLayout.m_bytesPerRow = RHI::AlignUp(subresourceLayout.m_bytesPerRow, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);
                subresourceLayout.m_bytesPerImage = subresourceLayout.m_rowCount * subresourceLayout.m_bytesPerRow;
            }
        }
	}
}