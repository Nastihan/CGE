
// DX12
#include "DX_StagingMemoryAllocator.h"
#include "DX_Device.h"

namespace CGE
{
	namespace DX12
	{
		DX_StagingMemoryAllocator::DX_StagingMemoryAllocator() {}

		void DX_StagingMemoryAllocator::Init(const Descriptor& descriptor)
		{
            m_device = descriptor.m_device;

            {
                DX_MemoryPageAllocator::Descriptor poolDesc;
                poolDesc.m_device = descriptor.m_device;
                poolDesc.m_pageSizeInBytes = descriptor.m_mediumPageSizeInBytes;
                poolDesc.m_collectLatency = descriptor.m_collectLatency;
                poolDesc.m_getHeapMemoryUsageFunction = [this]() { return &m_memoryUsage; };
                poolDesc.m_recycleOnCollect = true;
                m_mediumPageAllocator.Init(poolDesc);
            }

            {
                DX_MemoryPageAllocator::Descriptor poolDesc;
                poolDesc.m_device = descriptor.m_device;
                poolDesc.m_pageSizeInBytes = descriptor.m_largePageSizeInBytes;
                poolDesc.m_collectLatency = descriptor.m_collectLatency;
                poolDesc.m_getHeapMemoryUsageFunction = [this]() { return &m_memoryUsage; };
                poolDesc.m_recycleOnCollect = true;
                m_largePageAllocator.Init(poolDesc);

                m_largeBlockAllocator.Init(m_largePageAllocator);
            }
		}

        void DX_StagingMemoryAllocator::Shutdown()
        {
            m_mediumBlockAllocators.Shutdown();
            m_mediumPageAllocator.Shutdown();
            m_largeBlockAllocator.Shutdown();
            m_largePageAllocator.Shutdown();
        }

        void DX_StagingMemoryAllocator::GarbageCollect()
        {
            m_mediumBlockAllocators.GarbageCollect();
            m_mediumPageAllocator.Collect();

            m_largeBlockAllocator.GarbageCollect();
            m_largePageAllocator.Collect();
        }

        DX_MemoryView DX_StagingMemoryAllocator::Allocate(size_t sizeInBytes, size_t alignmentInBytes)
        {
            const size_t mediumPageSize = m_mediumPageAllocator.GetPageSize();
            const size_t mediumBlockThreshold = mediumPageSize / 2; // [todo] Any larger than half the medium page should use the large page. (Why? I guess less new pages)

            if (sizeInBytes <= mediumBlockThreshold)
            {
                return DX_MemoryView(m_mediumBlockAllocators.Allocate(sizeInBytes, alignmentInBytes), DX_MemoryViewType::Buffer);
            }
            else if (sizeInBytes <= m_largePageAllocator.GetPageSize())
            {
                return DX_MemoryView(m_largeBlockAllocator.Allocate(sizeInBytes, alignmentInBytes), DX_MemoryViewType::Buffer);
            }
            else
            {
                // Very large allocation. Make a committed resource.
                return AllocateUnique(sizeInBytes);
            }
        }

        DX_MemoryPageAllocator& DX_StagingMemoryAllocator::GetMediumPageAllocator()
        {
            return m_mediumPageAllocator;
        }

        DX_MemoryView DX_StagingMemoryAllocator::AllocateUnique(size_t sizeInBytes)
        {
            RHI::BufferDescriptor descriptor;
            descriptor.m_byteCount = sizeInBytes;
            DX_MemoryView memoryView = m_device->CreateBufferCommitted(descriptor, D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_HEAP_TYPE_UPLOAD);
            memoryView.GetMemory()->SetName(L"Large Upload Buffer");

            // Queue the memory or deferred release immediately.
            m_device->QueueForRelease(memoryView);

            return memoryView;
        }
	}
}