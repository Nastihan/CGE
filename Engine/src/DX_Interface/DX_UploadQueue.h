#pragma once

// DX12
#include "DX_CommandQueue.h"
#include "DX_Fence.h"

// RHI
#include "../RHI/DeviceObject.h"
#include "../RHI/Limits.h"
#include "../RHI/BufferPool.h"

namespace CGE
{
	namespace DX12
	{
		class DX_UploadQueue final : public RHI::DeviceObject
		{
            using Base = RHI::DeviceObject;
        public:
            struct Descriptor
            {
                size_t m_stagingSizeInBytes = RHI::Limits::DefaultValues::Memory::UploadQueueStagingBufferSizeInBytes;
                size_t m_frameCount = RHI::Limits::Device::FrameCountMax;
                Descriptor() = default;
                Descriptor(size_t stagingSizeInBytes);
            };

        public:
            REMOVE_COPY_AND_MOVE(DX_UploadQueue);

            DX_UploadQueue() = default;
            void Init(RHI::Device& device, const Descriptor& descriptor);
            void Shutdown();

            // Queue copy commands to upload buffer resource
            uint64_t QueueUpload(const RHI::BufferStreamRequest& request);

            // Queue copy commands to upload image subresources.
            // uint64_t QueueUpload(const RHI::StreamingImageExpandRequest& request, uint32_t residentMip);

            // GPU side wait.
            void QueueWaitFence(const DX_Fence& fence, uint64_t fenceValue);
            bool IsUploadFinished(uint64_t fenceValue);

            // Waits for the upload to get signaled and calls all the callbacks after.
            void WaitForUpload(uint64_t fenceValue);

        private:
            // We will have 3 frame packets for our triple buffer frame system.
            struct FramePacket
            {
                // Advanced Usage Mode in ID3D12Resource::Map api document (map called once and no unmap after)
                RHI::Ptr<ID3D12Resource> m_stagingResource;
                uint8_t* m_stagingResourceData = nullptr;
                uint32_t m_dataOffset = 0;

                RHI::Ptr<ID3D12CommandAllocator> m_commandAllocator;
                RHI::Ptr<ID3D12GraphicsCommandList> m_commandList;

                // Current frames fence to wait on and signal (used in BeginFramePacket and EndFramePacket)
                DX_Fence m_fence;
            };

        private:
            void ProcessCallbacks(uint64_t fenceValue);

            // This function waits on previous frame packed to finish, reset the command allocator and command list and set m_recordingFrame to true.
            FramePacket* BeginFramePacket();

            // Close and execute command list, signal the fence and increment the frame.
            void EndFramePacket(ID3D12CommandQueue* commandQueue);

        private:
            Descriptor m_descriptor;
            RHI::Ptr<DX_CommandQueue> m_copyQueue;
            std::vector<FramePacket> m_framePackets;

            // Used for external upload request to check if any upload currently queued.
            DX_Fence m_uploadFence;
            DX_FenceEvent m_uploadFenceEvent{ "Wait For Upload" };
            DX_FenceEvent m_fenceEvent{ "Wait for Frame" };
            size_t m_frameIndex = 0;
            bool m_recordingFrame = false;

            // pending upload callbacks and their corresponding fence values
            std::queue<std::pair<std::function<void()>, uint64_t>> m_callbacks;
            std::mutex m_callbackMutex;
        };
	}
}