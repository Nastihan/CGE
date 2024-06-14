#pragma once

// DX12
#include "DX_ImagePool.h"
#include "DX_Image.h"
#include "DX_ResourcePoolResolver.h"
#include "DX_Device.h"

// RHI
#include "../RHI/ImagePool.h"

namespace CGE
{
	namespace DX12
	{
        class DX_ImagePoolResolver : public DX_ResourcePoolResolver
        {
        public:
            DX_ImagePoolResolver(DX_Device& device, DX_ImagePool* imagePool);
            RHI::ResultCode UpdateImage(const RHI::ImageUpdateRequest& request, size_t& bytesTransferred);
            
            // DX_ResourcePoolResolver
            void Compile() override;
            void QueuePrologueTransitionBarriers(DX_CommandList& commandList) override;
            void Resolve(DX_CommandList& commandList) const override;
            void QueueEpilogueTransitionBarriers(DX_CommandList& commandList) const override;
            void Deactivate() override;
            void OnResourceShutdown(const RHI::Resource& resource) override;
        
        public:
            DX_ImagePool* m_pool = nullptr;
        
        private:
            struct ImagePacket
            {
                DX_Image* m_image;
                DX_Memory* m_imageMemory;
            };

            // This structure will be used to describe the location of a texture resource copy operation.
            struct ImageSubresourcePacket
            {
                RHI::Origin m_imageSubresourcePixelOffset;
                D3D12_TEXTURE_COPY_LOCATION m_imageLocation;
                D3D12_TEXTURE_COPY_LOCATION m_stagingLocation;
            };

        private:
            DX_Device* m_device = nullptr;
            std::mutex m_imagePacketMutex;
            std::vector<ImagePacket> m_imagePackets;
            std::vector<ImageSubresourcePacket> m_imageSubresourcePackets;
            std::vector<D3D12_RESOURCE_TRANSITION_BARRIER> m_prologueBarriers;
            std::vector<D3D12_RESOURCE_TRANSITION_BARRIER> m_epilogueBarriers;
        };
	}
}