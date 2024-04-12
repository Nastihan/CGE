
// DX12
#include <d3dx12.h>
#include "DX_Device.h"
#include "DX_UploadQueue.h"
#include "DX_Buffer.h"

namespace CGE
{
	namespace DX12
	{
		DX_UploadQueue::Descriptor::Descriptor(size_t stagingSizeInBytes)
		{
			m_stagingSizeInBytes = stagingSizeInBytes;
		}

		void DX_UploadQueue::Init(RHI::Device& device, const Descriptor& descriptor)
		{
            Base::Init(device);
            auto& dxDevice = static_cast<DX_Device&>(device);
            ID3D12DeviceX* dx12Device = dxDevice.GetDevice();

            m_copyQueue = &dxDevice.GetCommandQueueContext().GetCommandQueue(RHI::HardwareQueueClass::Copy);
            m_uploadFence.Init(dx12Device, RHI::FenceState::Signaled);

            for (size_t i = 0; i < descriptor.m_frameCount; ++i)
            {
                m_framePackets.emplace_back();
                FramePacket& framePacket = m_framePackets.back();
                framePacket.m_fence.Init(dx12Device, RHI::FenceState::Signaled);

                CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_UPLOAD);
                CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(descriptor.m_stagingSizeInBytes);

                Microsoft::WRL::ComPtr<ID3D12Resource> stagingResource;
                DXAssertSuccess(dx12Device->CreateCommittedResource(
                    &heapProperties,
                    D3D12_HEAP_FLAG_NONE,
                    &bufferDesc,
                    D3D12_RESOURCE_STATE_GENERIC_READ,
                    nullptr,
                    IID_PPV_ARGS(stagingResource.GetAddressOf())));

                framePacket.m_stagingResource = stagingResource.Get();
                CD3DX12_RANGE readRange(0, 0);
                framePacket.m_stagingResource->Map(0, &readRange, reinterpret_cast<void**>(&framePacket.m_stagingResourceData));


                Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
                DXAssertSuccess(dx12Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COPY, IID_PPV_ARGS(commandAllocator.GetAddressOf())));
                framePacket.m_commandAllocator = commandAllocator.Get();

                Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;
                DXAssertSuccess(dx12Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_COPY, framePacket.m_commandAllocator.get(), nullptr, IID_PPV_ARGS(commandList.GetAddressOf())));
                framePacket.m_commandList = commandList.Get();
                DXAssertSuccess(framePacket.m_commandList->Close());
            }
		}

        void DX_UploadQueue::Shutdown()
        {
            if (m_copyQueue)
            {
                m_copyQueue->Shutdown();
                m_copyQueue = nullptr;
            }
            for (auto& framePacket : m_framePackets)
            {
                framePacket.m_fence.Shutdown();
                framePacket.m_commandList = nullptr;
                framePacket.m_commandAllocator = nullptr;
            }
            m_framePackets.clear();
            m_uploadFence.Shutdown();
            Base::Shutdown();
        }

        uint64_t DX_UploadQueue::QueueUpload(const RHI::BufferStreamRequest& request)
        {
            DX_Buffer& dxBuffer = static_cast<DX_Buffer&>(*request.m_buffer);
            RHI::BufferPool& bufferPool = static_cast<RHI::BufferPool&>(*dxBuffer.GetPool());

            // No need to use staging buffers since it's host memory.
            if (bufferPool.GetDescriptor().m_heapMemoryLevel == RHI::HeapMemoryLevel::Host)
            {
                RHI::BufferMapRequest mapRequest;
                mapRequest.m_buffer = request.m_buffer;
                mapRequest.m_byteCount = request.m_byteCount;
                mapRequest.m_byteOffset = request.m_byteOffset;
                RHI::BufferMapResponse mapResponse;
                bufferPool.MapBuffer(mapRequest, mapResponse);
                ::memcpy(mapResponse.m_data, request.m_sourceData, request.m_byteCount);
                bufferPool.UnmapBuffer(*request.m_buffer);
                if (request.m_fenceToSignal)
                {
                    request.m_fenceToSignal->SignalOnCpu();
                }
                return m_uploadFence.GetPendingValue();
            }

            // Destination buffer
            const DX_MemoryView& memoryView = dxBuffer.GetMemoryView();
            RHI::Ptr<ID3D12Resource> dx12Buffer = memoryView.GetMemory();

            RHI::Ptr<ID3D12Fence> dx12FenceToSignal;
            uint64_t dx12FenceToSignalValue = 0;

            size_t byteCount = request.m_byteCount;
            size_t byteOffset = memoryView.GetOffset() + request.m_byteOffset;
            const uint8_t* sourceData = reinterpret_cast<const uint8_t*>(request.m_sourceData);

            // Get the request fence for signaling after upload finished
            if (request.m_fenceToSignal)
            {
                DX_Fence& fence = static_cast<DX_FenceImpl&>(*request.m_fenceToSignal).Get();
                dx12FenceToSignal = fence.Get();
                dx12FenceToSignalValue = fence.GetPendingValue();
            }

            uint64_t queueValue = m_uploadFence.Increment();
            m_copyQueue->QueueCommand([=](void* commandQueue)
                {
                    size_t pendingByteOffset = 0;
                    size_t pendingByteCount = byteCount;
                    ID3D12CommandQueue* dx12CommandQueue = static_cast<ID3D12CommandQueue*>(commandQueue);

                    while (pendingByteCount > 0)
                    {
                        // Wait on the previous frame uploads to finish.
                        FramePacket* framePacket = BeginFramePacket();
                        const size_t bytesToCopy = std::min(pendingByteCount, m_descriptor.m_stagingSizeInBytes);
                        {
                            memcpy(framePacket->m_stagingResourceData, sourceData + pendingByteOffset, bytesToCopy);
                        }
                        framePacket->m_commandList->CopyBufferRegion(dx12Buffer.get(), byteOffset + pendingByteOffset, framePacket->m_stagingResource.get(), 0, bytesToCopy);
                        pendingByteOffset += bytesToCopy;
                        pendingByteCount -= bytesToCopy;

                        EndFramePacket(dx12CommandQueue);
                    }

                    if (dx12FenceToSignal)
                    {
                        dx12CommandQueue->Signal(dx12FenceToSignal.get(), dx12FenceToSignalValue);
                    }
                    dx12CommandQueue->Signal(m_uploadFence.Get(), queueValue);
                });

            return queueValue;
        }

        void DX_UploadQueue::QueueWaitFence(const DX_Fence& fence, uint64_t fenceValue)
        {
            m_copyQueue->QueueCommand([=](void* commandQueue)
                {
                    ID3D12CommandQueue* dx12CommandQueue = static_cast<ID3D12CommandQueue*>(commandQueue);
                    dx12CommandQueue->Wait(fence.Get(), fenceValue);
                });
        }

        bool DX_UploadQueue::IsUploadFinished(uint64_t fenceValue)
        {
            return m_uploadFence.GetCompletedValue() >= fenceValue;
        }

        void DX_UploadQueue::WaitForUpload(uint64_t fenceValue)
        {
            if (!IsUploadFinished(fenceValue))
            {
                assert(m_uploadFence.GetPendingValue() >= fenceValue, "Attempting to wait for work that has not been encoded.");
                m_uploadFence.Wait(m_uploadFenceEvent, fenceValue);
            }
            ProcessCallbacks(fenceValue);
        }

        void DX_UploadQueue::ProcessCallbacks(uint64_t fenceValue)
        {
            std::lock_guard<std::mutex> lock(m_callbackMutex);
            uint64_t completedValue = m_uploadFence.GetCompletedValue();
            fenceValue = std::min(completedValue, fenceValue);

            while (m_callbacks.size() > 0 && m_callbacks.front().second <= fenceValue)
            {
                std::function<void()> callback = m_callbacks.front().first;
                m_callbacks.pop();
                callback();
            }
        }

        DX_UploadQueue::FramePacket* DX_UploadQueue::BeginFramePacket()
        {
            assert(!m_recordingFrame, "The previous frame packet isn't ended");

            FramePacket* framePacket = &m_framePackets[m_frameIndex];
            framePacket->m_fence.Wait(m_fenceEvent);
            framePacket->m_fence.Increment();
            framePacket->m_dataOffset = 0;

            DXAssertSuccess(framePacket->m_commandAllocator->Reset());
            DXAssertSuccess(framePacket->m_commandList->Reset(framePacket->m_commandAllocator.get(), nullptr));

            m_recordingFrame = true;
            return framePacket;
        }

        void DX_UploadQueue::EndFramePacket(ID3D12CommandQueue* commandQueue)
        {
            assert(m_recordingFrame, "The frame packet wasn't started. You need to call StartFramePacket first.");

            FramePacket& framePacket = m_framePackets[m_frameIndex];

            DXAssertSuccess(framePacket.m_commandList->Close());
            ID3D12CommandList* commandLists[] = { framePacket.m_commandList.get() };
            commandQueue->ExecuteCommandLists(1, commandLists);
            commandQueue->Signal(framePacket.m_fence.Get(), framePacket.m_fence.GetPendingValue());

            m_frameIndex = (m_frameIndex + 1) % m_descriptor.m_frameCount;
            m_recordingFrame = false;
        }
	}
}