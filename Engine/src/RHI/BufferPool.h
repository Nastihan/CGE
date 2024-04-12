#pragma once

#include "Buffer.h"
#include "ResourcePool.h"
#include "BufferPoolDescriptor.h"

namespace CGE
{
	namespace RHI
	{
        class Fence;

        // Used in BufferPool::InitBuffer.
        struct BufferInitRequest
        {
            BufferInitRequest() = default;
            BufferInitRequest(Buffer& buffer, const BufferDescriptor& descriptor, const void* initialData = nullptr);

            // Buffer to initilize. Must be in uninitialized state.
            Buffer* m_buffer = nullptr;
            // The descriptor used to initialize the buffer.
            BufferDescriptor m_descriptor;
            // Initial data used to initialize the buffer.
            const void* m_initialData = nullptr;
        };

        // used in BufferPool::MapBuffer. Has to be host visible.
        struct BufferMapRequest
        {
            BufferMapRequest() = default;
            BufferMapRequest(Buffer& buffer, size_t byteOffset, size_t byteCount);

            Buffer* m_buffer = nullptr;

            // After map was called to will map to the begining of the buffer.
            // We move the pointer if offset is set. (check DX_BufferPool::MapBufferInternal)
            size_t m_byteOffset = 0;
            size_t m_byteCount = 0;
        };

        // Returned from BufferPool::MapBuffer.
        struct BufferMapResponse
        {
            void* m_data = nullptr;
        };

        // Used in platfrom specific upload queue
        struct BufferStreamRequest
        {
            // Fence to signal when upload is finished and buffer is unmaped
            Fence* m_fenceToSignal = nullptr;
            Buffer* m_buffer = nullptr;
            size_t m_byteOffset = 0;
            size_t m_byteCount = 0;
            const void* m_sourceData = nullptr;
        };


        class BufferPool : public ResourcePool
        {
        public:
            virtual ~BufferPool() override = default;

            // To re-initialize call shutdown first.
            ResultCode Init(Device& device, const BufferPoolDescriptor& descriptor);

            // Buffer must be in an uninitialized state. Call Shutdown first to reinitilize.
            ResultCode InitBuffer(const BufferInitRequest& request);
            ResultCode OrphanBuffer(Buffer& buffer);
            ResultCode MapBuffer(const BufferMapRequest& request, BufferMapResponse& response);
            void UnmapBuffer(Buffer& buffer);

            // Uses Async upload queue.
            ResultCode StreamBuffer(const BufferStreamRequest& request);
            const BufferPoolDescriptor& GetDescriptor() const override final;

        protected:
            BufferPool() = default;
            bool ValidateNotProcessingFrame() const;
            void OnFrameBegin() override;

        private:
            using ResourcePool::Init;

            bool ValidatePoolDescriptor(const BufferPoolDescriptor& descriptor) const;
            bool ValidateInitRequest(const BufferInitRequest& initRequest) const;
            bool ValidateIsHostHeap() const;
            bool ValidateMapRequest(const BufferMapRequest& request) const;
            void ValidateBufferMap(Buffer& buffer, bool isDataValid);
            bool ValidateBufferUnmap(Buffer& buffer);
            
            virtual ResultCode InitInternal(Device& device, const RHI::BufferPoolDescriptor& descriptor) = 0;
            virtual ResultCode InitBufferInternal(Buffer& buffer, const BufferDescriptor& descriptor) = 0;
            virtual ResultCode OrphanBufferInternal(Buffer& buffer) = 0;
            virtual ResultCode MapBufferInternal(const BufferMapRequest& request, BufferMapResponse& response) = 0;
            virtual void UnmapBufferInternal(Buffer& buffer) = 0;
            virtual ResultCode StreamBufferInternal(const BufferStreamRequest& request);

            // Does a memcopy
            virtual void BufferCopy(void* destination, const void* source, size_t num);

        private:
            BufferPoolDescriptor m_descriptor;

            // This will be accross all buffers registered on this pool.
            std::atomic_uint m_mapRefCount = { 0 };
        };
	}
}