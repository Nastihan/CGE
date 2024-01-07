
// DX12
#include "DX_BufferMemoryAllocator.h"
#include "DX_Conversions.h"
#include "DX_Device.h"

// RHI
#include "../RHI/Limits.h"

namespace CGE
{
	namespace DX12
	{
        void DX_BufferMemoryAllocator::Init(const Descriptor& descriptor)
        {
            m_descriptor = descriptor;

            // Buffers that can be attachments can independently transition state, which precludes them from
            // being sub-allocated from the same ID3D12Resource.
            m_usePageAllocator = false;

            if (!RHI::CheckBitsAny(static_cast<uint32_t>(descriptor.m_bindFlags), 
                static_cast<uint32_t>(RHI::BufferBindFlags::ShaderWrite) | 
                static_cast<uint32_t>(RHI::BufferBindFlags::CopyWrite) | 
                static_cast<uint32_t>(RHI::BufferBindFlags::InputAssembly) | 
                static_cast<uint32_t>(RHI::BufferBindFlags::DynamicInputAssembly)))
            {
                m_usePageAllocator = true;

                const RHI::HeapMemoryUsage* heapMemoryUsage = descriptor.m_getHeapMemoryUsageFunction();

                const size_t budgetInBytes = heapMemoryUsage->m_budgetInBytes;
                if (budgetInBytes)
                {
                    // The buffer page size should not exceed the budget.
                    m_descriptor.m_pageSizeInBytes = static_cast<uint32_t>(std::min<size_t>(budgetInBytes, m_descriptor.m_pageSizeInBytes));
                }

                m_pageAllocator.Init(m_descriptor);

                m_subAllocationAlignment = RHI::CheckBitsAny(static_cast<uint32_t>(m_descriptor.m_bindFlags), static_cast<uint32_t>(RHI::BufferBindFlags::Constant)) ? DX_Alignment::Constant : DX_Alignment::Buffer;

                DX_MemoryFreeListSubAllocator::Descriptor pageAllocatorDescriptor;
                pageAllocatorDescriptor.m_alignmentInBytes = m_subAllocationAlignment;
                pageAllocatorDescriptor.m_garbageCollectLatency = RHI::Limits::Device::FrameCountMax;
                pageAllocatorDescriptor.m_inactivePageCycles = 1;
                m_subAllocator.Init(pageAllocatorDescriptor, m_pageAllocator);
            }
        }

        void DX_BufferMemoryAllocator::Shutdown()
        {
            if (m_usePageAllocator)
            {
                m_subAllocator.Shutdown();
                m_pageAllocator.Shutdown();
            }
        }

        void DX_BufferMemoryAllocator::GarbageCollect()
        {
            m_subAllocatorMutex.lock();
            m_subAllocator.GarbageCollect();
            m_subAllocatorMutex.unlock();

            m_pageAllocator.Collect();
        }

        DX_BufferMemoryView DX_BufferMemoryAllocator::Allocate(size_t sizeInBytes, size_t overrideSubAllocAlignment)
        {
            DX_BufferMemoryView bufferMemoryView;

            // First attempt to sub-allocate a buffer from the sub-allocator.
            if (m_usePageAllocator)
            {
                m_subAllocatorMutex.lock();

                size_t subAllocationAlignment = m_subAllocationAlignment;
                if (overrideSubAllocAlignment > 0)
                {
                    if ((overrideSubAllocAlignment % m_subAllocationAlignment) != 0 && (m_subAllocationAlignment % overrideSubAllocAlignment) != 0)
                    {
                        assert(false, "The buffer alignment should be either an integer multiple or a factor of default alignment");
                    }

                    subAllocationAlignment = std::max(overrideSubAllocAlignment, m_subAllocationAlignment);
                }

                bufferMemoryView = DX_BufferMemoryView(DX_MemoryView(m_subAllocator.Allocate(sizeInBytes, subAllocationAlignment), DX_MemoryViewType::Buffer), DX_BufferMemoryType::SubAllocated);
                m_subAllocatorMutex.unlock();
            }

            // Next, try a unique buffer allocation.
            if (!bufferMemoryView.IsValid())
            {
                RHI::BufferDescriptor bufferDescriptor;
                bufferDescriptor.m_byteCount = sizeInBytes;
                bufferDescriptor.m_bindFlags = m_descriptor.m_bindFlags;
                bufferMemoryView = AllocateUnique(bufferDescriptor);
            }

            return bufferMemoryView;
        }

        void DX_BufferMemoryAllocator::DeAllocate(const DX_BufferMemoryView& memoryView)
        {
            switch(memoryView.GetType())
            {
            case DX_BufferMemoryType::SubAllocated:
                m_subAllocatorMutex.lock();
                m_subAllocator.DeAllocate(memoryView.m_memoryAllocation);
                m_subAllocatorMutex.unlock();
                break;

            case DX_BufferMemoryType::Unique:
                DeAllocateUnique(memoryView);
                break;
            }
        }

        DX_BufferMemoryView DX_BufferMemoryAllocator::AllocateUnique(const RHI::BufferDescriptor& bufferDescriptor)
        {
            const size_t alignedSize = RHI::AlignUp(bufferDescriptor.m_byteCount, DX_Alignment::CommittedBuffer);
            RHI::HeapMemoryUsage& heapMemoryUsage = *m_descriptor.m_getHeapMemoryUsageFunction();
            if (!heapMemoryUsage.TryReserveMemory(alignedSize))
            {
                return DX_BufferMemoryView();
            }

            D3D12_RESOURCE_STATES initialResourceState = ConvertInitialResourceState(m_descriptor.m_heapMemoryLevel, m_descriptor.m_hostMemoryAccess);
            const D3D12_HEAP_TYPE heapType = ConvertHeapType(m_descriptor.m_heapMemoryLevel, m_descriptor.m_hostMemoryAccess);
            DX_MemoryView memoryView = m_descriptor.m_device->CreateBufferCommitted(bufferDescriptor, initialResourceState, heapType);
            if (memoryView.IsValid())
            {
                heapMemoryUsage.m_residentInBytes += alignedSize;
            }
            else
            {
                heapMemoryUsage.m_reservedInBytes -= alignedSize;
            }

            return DX_BufferMemoryView(std::move(memoryView), DX_BufferMemoryType::Unique);
        }

        void DX_BufferMemoryAllocator::DeAllocateUnique(const DX_BufferMemoryView& memoryView)
        {
            assert(memoryView.GetType() == DX_BufferMemoryType::Unique, "This call only supports unique BufferMemoryView allocations.");
            const size_t sizeInBytes = memoryView.GetSize();

            RHI::HeapMemoryUsage& heapMemoryUsage = *m_descriptor.m_getHeapMemoryUsageFunction();
            heapMemoryUsage.m_residentInBytes -= sizeInBytes;
            heapMemoryUsage.m_reservedInBytes -= sizeInBytes;

            m_descriptor.m_device->QueueForRelease(memoryView);
        }
	}
}