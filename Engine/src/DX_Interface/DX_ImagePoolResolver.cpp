
// DX12
#include "DX_ImagePoolResolver.h"
#include "DX_Conversions.h"
#include "DX_CommandList.h"

#include <d3dx12.h>

namespace CGE
{
	namespace DX12
	{
		DX_ImagePoolResolver::DX_ImagePoolResolver(DX_Device& device, DX_ImagePool* imagePool) : m_device{ &device }, m_pool{ imagePool } {}

		RHI::ResultCode DX_ImagePoolResolver::UpdateImage(const RHI::ImageUpdateRequest& request, size_t& bytesTransferred)
		{
            std::lock_guard<std::mutex> lock(m_imagePacketMutex);

            DX_Image* image = static_cast<DX_Image*>(request.m_image);
            DX_Memory* imageMemory = image->GetMemoryView().GetMemory();

            // Allocate an entry from the Image packets vector. Keep the set unique.
            {
                size_t imagePacketIndex = 0;
                for (; imagePacketIndex < m_imagePackets.size(); ++imagePacketIndex)
                {
                    if (m_imagePackets[imagePacketIndex].m_image == image)
                    {
                        break;
                    }
                }

                if (imagePacketIndex == m_imagePackets.size())
                {
                    m_imagePackets.emplace_back();
                    ImagePacket& imagePacket = m_imagePackets.back();
                    imagePacket.m_image = image;
                    imagePacket.m_imageMemory = image->GetMemoryView().GetMemory();
                }
            }

            // Build a subresource packet which contains the staging data and target image location to copy into.
            const RHI::ImageDescriptor& imageDescriptor = image->GetDescriptor();
            const RHI::ImageSubresourceLayout& sourceSubresourceLayout = request.m_sourceSubresourceLayout;
            const uint32_t stagingRowPitch = RHI::AlignUp(sourceSubresourceLayout.m_bytesPerRow, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);
            const uint32_t stagingSlicePitch = stagingRowPitch * sourceSubresourceLayout.m_rowCount;
            DX_MemoryView stagingMemory = m_device->AcquireStagingMemory(stagingSlicePitch, D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT);

            D3D12_PLACED_SUBRESOURCE_FOOTPRINT stagingFootprint;
            stagingFootprint.Offset = stagingMemory.GetOffset();
            stagingFootprint.Footprint.Width = sourceSubresourceLayout.m_size.m_width;
            stagingFootprint.Footprint.Height = sourceSubresourceLayout.m_size.m_height;
            stagingFootprint.Footprint.Depth = sourceSubresourceLayout.m_size.m_depth;
            stagingFootprint.Footprint.Format = GetBaseFormat(ConvertFormat(imageDescriptor.m_format));
            stagingFootprint.Footprint.RowPitch = stagingRowPitch;

            m_imageSubresourcePackets.emplace_back();
            ImageSubresourcePacket& imageSubresourcePacket = m_imageSubresourcePackets.back();

            // Copy to the requested image subresource with the requested pixel offset.
            imageSubresourcePacket.m_imageLocation = CD3DX12_TEXTURE_COPY_LOCATION(imageMemory, RHI::GetImageSubresourceIndex(request.m_imageSubresource, imageDescriptor.m_mipLevels));
            imageSubresourcePacket.m_imageSubresourcePixelOffset = request.m_imageSubresourcePixelOffset;

            // Copy from the staging data using the allocated staging memory and the computed footprint.
            imageSubresourcePacket.m_stagingLocation = CD3DX12_TEXTURE_COPY_LOCATION(stagingMemory.GetMemory(), stagingFootprint);

            // Copy CPU data into the staging memory.
            {
                DX_CpuVirtualAddress stagingMemoryPtr = stagingMemory.Map(RHI::HostMemoryAccess::Write);

                D3D12_MEMCPY_DEST destData;
                destData.pData = stagingMemoryPtr;
                destData.RowPitch = stagingRowPitch;
                destData.SlicePitch = stagingSlicePitch;

                D3D12_SUBRESOURCE_DATA srcData;
                srcData.pData = request.m_sourceData;
                srcData.RowPitch = sourceSubresourceLayout.m_bytesPerRow;
                srcData.SlicePitch = sourceSubresourceLayout.m_bytesPerImage;

                MemcpySubresource(&destData, &srcData, sourceSubresourceLayout.m_bytesPerRow, sourceSubresourceLayout.m_rowCount, sourceSubresourceLayout.m_size.m_depth);

                stagingMemory.Unmap(RHI::HostMemoryAccess::Write);
            }

            image->m_pendingResolves++;
            bytesTransferred = stagingMemory.GetSize();
			return RHI::ResultCode::Success;
		}

        // [todo - FrameGraph]
        void DX_ImagePoolResolver::Compile() {}
		void DX_ImagePoolResolver::OnResourceShutdown(const RHI::Resource& resource) {}
        void DX_ImagePoolResolver::QueuePrologueTransitionBarriers(DX_CommandList& commandList) {}
        void DX_ImagePoolResolver::QueueEpilogueTransitionBarriers(DX_CommandList& commandList) const {}
        
        void DX_ImagePoolResolver::Deactivate()
        {

        }

        void DX_ImagePoolResolver::Resolve(DX_CommandList& commandList) const
        {
            for (const ImageSubresourcePacket& imageSubresourcePacket : m_imageSubresourcePackets)
            {
                commandList.GetCommandList()->CopyTextureRegion(
                    &imageSubresourcePacket.m_imageLocation,
                    imageSubresourcePacket.m_imageSubresourcePixelOffset.m_left,
                    imageSubresourcePacket.m_imageSubresourcePixelOffset.m_top,
                    imageSubresourcePacket.m_imageSubresourcePixelOffset.m_front,
                    &imageSubresourcePacket.m_stagingLocation,
                    nullptr);
            }
        }
	}
}