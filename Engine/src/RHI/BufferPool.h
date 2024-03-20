#pragma once

#include "Buffer.h"
#include "ResourcePool.h"
#include "BufferPoolDescriptor.h"

namespace CGE
{
	namespace RHI
	{
        class Fence;

        struct BufferInitRequest
        {
            BufferInitRequest() = default;
            BufferInitRequest(Buffer& buffer, const BufferDescriptor& descriptor, const void* initialData = nullptr);

            Buffer* m_buffer = nullptr;
            BufferDescriptor m_descriptor;
            const void* m_initialData = nullptr;
        };

        struct BufferMapRequest
        {
            BufferMapRequest() = default;
            BufferMapRequest(Buffer& buffer, size_t byteOffset, size_t byteCount);

            Buffer* m_buffer = nullptr;
            size_t m_byteOffset = 0;
            size_t m_byteCount = 0;
        };

        struct BufferMapResponse
        {
            void* m_data = nullptr;
        };

        struct BufferStreamRequest
        {
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
            ResultCode InitBuffer(const BufferInitRequest& request);
            ResultCode OrphanBuffer(Buffer& buffer);
            ResultCode MapBuffer(const BufferMapRequest& request, BufferMapResponse& response);
            void UnmapBuffer(Buffer& buffer);
            ResultCode StreamBuffer(const BufferStreamRequest& request);
            const BufferPoolDescriptor& GetDescriptor() const override final;

        protected:
            BufferPool() = default;
            bool ValidateNotProcessingFrame() const;

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
            virtual void BufferCopy(void* destination, const void* source, size_t num);

        private:
            BufferPoolDescriptor m_descriptor;
            std::atomic_uint m_mapRefCount = { 0 };
        };
	}
}