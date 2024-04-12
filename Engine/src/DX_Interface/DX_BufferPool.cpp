
// DX12
#include "DX_BufferPool.h"
#include "DX_BufferPoolDescriptor.h"
#include "DX_BufferPoolResolver.h"
#include "DX_Conversions.h"

namespace CGE
{
	namespace DX12
	{
		RHI::Ptr<DX_BufferPool> DX_BufferPool::Create()
		{
            return new DX_BufferPool();
		}

        DX_Device& DX_BufferPool::GetDevice() const
        {
            return static_cast<DX_Device&>(Base::GetDevice());
        }

        RHI::ResultCode DX_BufferPool::InitInternal(RHI::Device& device, const RHI::BufferPoolDescriptor& descriptor)
        {
            DX_Device& dxDevice = static_cast<DX_Device&>(device);
            RHI::HeapMemoryUsage& heapMemoryUsage = m_memoryUsage.GetHeapMemoryUsage(descriptor.m_heapMemoryLevel);
            uint32_t bufferPageSize = static_cast<uint32_t>(device.GetPlatformLimitsDescriptor().m_platformDefaultValues.m_bufferPoolPageSizeInBytes);

            // The DX12 descriptor provides an explicit buffer page size override.
            if (const DX_BufferPoolDescriptor* dxDescriptor = dynamic_cast<const DX_BufferPoolDescriptor*>(&descriptor))
            {
                bufferPageSize = dxDescriptor->m_bufferPoolPageSizeInBytes;
            }

            if (descriptor.m_largestPooledAllocationSizeInBytes > 0)
            {
                bufferPageSize = std::max<uint32_t>(bufferPageSize, static_cast<uint32_t>(descriptor.m_largestPooledAllocationSizeInBytes));
            }

            DX_BufferMemoryAllocator::Descriptor allocatorDescriptor;
            allocatorDescriptor.m_device = &dxDevice;
            allocatorDescriptor.m_pageSizeInBytes = bufferPageSize;
            allocatorDescriptor.m_bindFlags = descriptor.m_bindFlags;
            allocatorDescriptor.m_heapMemoryLevel = descriptor.m_heapMemoryLevel;
            allocatorDescriptor.m_hostMemoryAccess = descriptor.m_hostMemoryAccess;
            allocatorDescriptor.m_getHeapMemoryUsageFunction = [&]() { return &heapMemoryUsage; };
            allocatorDescriptor.m_recycleOnCollect = false;
            m_allocator.Init(allocatorDescriptor);

            if (descriptor.m_heapMemoryLevel == RHI::HeapMemoryLevel::Device)
            {
                SetResolver(std::make_unique<DX_BufferPoolResolver>(dxDevice, descriptor));
            }

            return RHI::ResultCode::Success;
        }

        void DX_BufferPool::ShutdownInternal()
        {
            m_allocator.Shutdown();
        }

        RHI::ResultCode DX_BufferPool::InitBufferInternal(RHI::Buffer& buffer, const RHI::BufferDescriptor& bufferDescriptor)
        {
            // We need respect the buffer's alignment if the buffer is used for SRV or UAV
            // If not by setting to zero the default alignment will be used.
            bool useBufferAlignment = RHI::CheckBitsAny(static_cast<uint32_t>(bufferDescriptor.m_bindFlags),
                static_cast<uint32_t>(RHI::BufferBindFlags::ShaderRead) | static_cast<uint32_t>(RHI::BufferBindFlags::ShaderWrite));

            size_t overrideAlignment = useBufferAlignment ? bufferDescriptor.m_alignment : 0;

            DX_BufferMemoryView memoryView = m_allocator.Allocate(bufferDescriptor.m_byteCount, overrideAlignment);
            if (memoryView.IsValid())
            {
                // Unique memoryView can inherit the name of the buffer.
                if (memoryView.GetType() == DX_BufferMemoryType::Unique && !buffer.GetName().empty())
                {
                    memoryView.SetName(buffer.GetName());
                }

                DX_Buffer& dxBuffer = static_cast<DX_Buffer&>(buffer);
                dxBuffer.m_memoryView = std::move(memoryView);
                dxBuffer.m_initialAttachmentState = ConvertInitialResourceState(GetDescriptor().m_heapMemoryLevel, GetDescriptor().m_hostMemoryAccess);
                return RHI::ResultCode::Success;
            }
            return RHI::ResultCode::OutOfMemory;
        }

        void DX_BufferPool::ShutdownResourceInternal(RHI::Resource& resource)
        {
            if (auto* resolver = GetResolver())
            {
                resolver->OnResourceShutdown(resource);
            }

            DX_Buffer& dxBuffer = static_cast<DX_Buffer&>(resource);
            m_allocator.DeAllocate(dxBuffer.m_memoryView);
            dxBuffer.m_memoryView = {};
            dxBuffer.m_initialAttachmentState = D3D12_RESOURCE_STATE_COMMON;
        }

        RHI::ResultCode DX_BufferPool::OrphanBufferInternal(RHI::Buffer& buffer)
        {
            DX_Buffer& dxBuffer = static_cast<DX_Buffer&>(buffer);

            DX_BufferMemoryView newMemoryView = m_allocator.Allocate(buffer.GetDescriptor().m_byteCount);
            if (newMemoryView.IsValid())
            {
                if (newMemoryView.GetType() == DX_BufferMemoryType::Unique && !buffer.GetName().empty())
                {
                    newMemoryView.SetName(buffer.GetName());
                }
                m_allocator.DeAllocate(dxBuffer.m_memoryView);
                dxBuffer.m_memoryView = std::move(newMemoryView);
                dxBuffer.InvalidateViews();
                return RHI::ResultCode::Success;
            }
            return RHI::ResultCode::OutOfMemory;
        }

        RHI::ResultCode DX_BufferPool::MapBufferInternal(const RHI::BufferMapRequest& mapRequest, RHI::BufferMapResponse& response)
        {
            const RHI::BufferPoolDescriptor& poolDescriptor = GetDescriptor();
            DX_Buffer& buffer = *static_cast<DX_Buffer*>(mapRequest.m_buffer);
            DX_CpuVirtualAddress mappedData = nullptr;

            if (poolDescriptor.m_heapMemoryLevel == RHI::HeapMemoryLevel::Host)
            {
                mappedData = buffer.GetMemoryView().Map(poolDescriptor.m_hostMemoryAccess);
                if (!mappedData)
                {
                    return RHI::ResultCode::Fail;
                }
                mappedData += mapRequest.m_byteOffset;
            }
            else
            {
                // Does the staging transfer via the staging memory allocator
                // The pointer returned is from the staging memory for cpu fast reads
                mappedData = GetResolver()->MapBuffer(mapRequest);
                if (!mappedData)
                {
                    return RHI::ResultCode::OutOfMemory;
                }
            }

            response.m_data = mappedData;
            return RHI::ResultCode::Success;
        }

        void DX_BufferPool::UnmapBufferInternal(RHI::Buffer& buffer)
        {
            const RHI::BufferPoolDescriptor& poolDescriptor = GetDescriptor();
            DX_Buffer& dxBuffer = static_cast<DX_Buffer&>(buffer);

            if (poolDescriptor.m_heapMemoryLevel == RHI::HeapMemoryLevel::Host)
            {
                dxBuffer.GetMemoryView().Unmap(poolDescriptor.m_hostMemoryAccess);
            }
        }

        RHI::ResultCode DX_BufferPool::StreamBufferInternal(const RHI::BufferStreamRequest& request)
        {
            return RHI::ResultCode::Success;
        }

        DX_BufferPoolResolver* DX_BufferPool::GetResolver()
        {
            return static_cast<DX_BufferPoolResolver*>(Base::GetResolver());
        }
	}
}