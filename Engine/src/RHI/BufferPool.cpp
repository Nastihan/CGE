
// RHI
#include "BufferPool.h"

namespace CGE
{
	namespace RHI
	{
        BufferInitRequest::BufferInitRequest(Buffer& buffer, const BufferDescriptor& descriptor, const void* initialData)
            : m_buffer{ &buffer }
            , m_descriptor{ descriptor }
            , m_initialData{ initialData } {}

        BufferMapRequest::BufferMapRequest(Buffer& buffer, size_t byteOffset, size_t byteCount)
            : m_buffer{ &buffer }
            , m_byteOffset{ byteOffset }
            , m_byteCount{ byteCount } {}

        ResultCode BufferPool::Init(Device& device, const BufferPoolDescriptor& descriptor)
        {
            return ResourcePool::Init(device, descriptor,
                [this, &device, &descriptor]()
                {
                    if (!ValidatePoolDescriptor(descriptor))
                    {
                        return ResultCode::InvalidArgument;
                    }

                    m_descriptor = descriptor;
                    return InitInternal(device, descriptor);
                });
        }

        ResultCode BufferPool::InitBuffer(const BufferInitRequest& request)
        {
            if (!ValidateInitRequest(request))
            {
                return ResultCode::InvalidArgument;
            }

            request.m_buffer->SetDescriptor(request.m_descriptor);
            ResultCode resultCode = ResourcePool::InitResource(request.m_buffer, [this, &request]() { return InitBufferInternal(*request.m_buffer, request.m_descriptor); });

            if (resultCode == ResultCode::Success && request.m_initialData)
            {
                BufferMapRequest mapRequest;
                mapRequest.m_buffer = request.m_buffer;
                mapRequest.m_byteCount = request.m_descriptor.m_byteCount;
                mapRequest.m_byteOffset = 0;

                BufferMapResponse mapResponse;
                resultCode = MapBufferInternal(mapRequest, mapResponse);
                if (resultCode == ResultCode::Success)
                {
                    BufferCopy(mapResponse.m_data, request.m_initialData, request.m_descriptor.m_byteCount);
                    UnmapBufferInternal(*request.m_buffer);
                }
            }

            return resultCode;
        }

        ResultCode BufferPool::OrphanBuffer(Buffer& buffer)
        {
            if (!ValidateIsInitialized() || !ValidateIsHostHeap() || !ValidateNotProcessingFrame())
            {
                return ResultCode::InvalidOperation;
            }

            if (!ValidateIsRegistered(&buffer))
            {
                return ResultCode::InvalidArgument;
            }

            return OrphanBufferInternal(buffer);
        }

        ResultCode BufferPool::MapBuffer(const BufferMapRequest& request, BufferMapResponse& response)
        {
            if (!ValidateIsInitialized() || !ValidateNotProcessingFrame())
            {
                return ResultCode::InvalidOperation;
            }

            if (!ValidateIsRegistered(request.m_buffer))
            {
                return ResultCode::InvalidArgument;
            }

            if (!ValidateMapRequest(request))
            {
                return ResultCode::InvalidArgument;
            }

            ResultCode resultCode = MapBufferInternal(request, response);
            ValidateBufferMap(*request.m_buffer, response.m_data != nullptr);
            return resultCode;
        }

        void BufferPool::UnmapBuffer(Buffer& buffer)
        {
            if (ValidateIsInitialized() && ValidateNotProcessingFrame() && ValidateIsRegistered(&buffer) && ValidateBufferUnmap(buffer))
            {
                UnmapBufferInternal(buffer);
            }
        }

        ResultCode BufferPool::StreamBuffer(const BufferStreamRequest& request)
        {
            if (!ValidateIsInitialized())
            {
                return ResultCode::InvalidOperation;
            }

            if (!ValidateIsRegistered(request.m_buffer))
            {
                return ResultCode::InvalidArgument;
            }

            return StreamBufferInternal(request);
        }

        const BufferPoolDescriptor& BufferPool::GetDescriptor() const
        {
            return m_descriptor;
        }

        bool BufferPool::ValidateNotProcessingFrame() const
        {
            return GetDescriptor().m_heapMemoryLevel != HeapMemoryLevel::Device || ResourcePool::ValidateNotProcessingFrame();
        }

        bool BufferPool::ValidatePoolDescriptor(const BufferPoolDescriptor& descriptor) const
        {
            // When HeapMemoryLevel::Device is specified, m_hostMemoryAccess must be HostMemoryAccess::Write
            assert((descriptor.m_heapMemoryLevel != RHI::HeapMemoryLevel::Device) || (descriptor.m_hostMemoryAccess == RHI::HostMemoryAccess::Write));
            return true;
        }

        bool BufferPool::ValidateInitRequest(const BufferInitRequest& initRequest) const
        {
            const BufferPoolDescriptor& poolDescriptor = GetDescriptor();
            assert(initRequest.m_descriptor.m_bindFlags == poolDescriptor.m_bindFlags);
            // Initial data is not allowed for read-only heaps
            assert((!initRequest.m_initialData) || (poolDescriptor.m_hostMemoryAccess != RHI::HostMemoryAccess::Read));
            return true;
        }

        bool BufferPool::ValidateIsHostHeap() const
        {
            assert(GetDescriptor().m_heapMemoryLevel == HeapMemoryLevel::Host);
            return true;
        }

        bool BufferPool::ValidateMapRequest(const BufferMapRequest& request) const
        {
            assert(request.m_buffer);
            assert(request.m_byteCount != 0);
            assert(request.m_byteOffset + request.m_byteCount <= request.m_buffer->GetDescriptor().m_byteCount);
            return true;
        }

        ResultCode BufferPool::StreamBufferInternal(const BufferStreamRequest& request)
        {
            return ResultCode::Unimplemented;
        }

        void BufferPool::BufferCopy(void* destination, const void* source, size_t num)
        {
            memcpy(destination, source, num);
        }

        void BufferPool::ValidateBufferMap(Buffer& buffer, bool isDataValid)
        {
            assert(isDataValid);
            m_mapRefCount++;
        }

        bool BufferPool::ValidateBufferUnmap(Buffer& buffer)
        {
            m_mapRefCount--;
            return true;
        }
	}
}