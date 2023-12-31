#include "PoolAllocator.h"
#include "RHI_Common.h"

namespace CGE
{
    namespace RHI
    {
        bool PoolAllocator::IsGarbageReady(Garbage& garbage) const
        {
            return (m_garbageCollectCycle - garbage.m_garbageCollectCycle) >= m_descriptor.m_garbageCollectLatency;
        }

        size_t PoolAllocator::GetAllocationCount() const
        {
            return m_allocationCountTotal;
        }

        size_t PoolAllocator::GetAllocatedByteCount() const
        {
            return m_allocationCountTotal * m_descriptor.m_elementSize;
        }

        const PoolAllocator::Descriptor& PoolAllocator::GetDescriptor() const
        {
            return m_descriptor;
        }

        void PoolAllocator::Init(const Descriptor& descriptor)
        {
            Shutdown();

            assert(descriptor.m_elementSize, "Element size cannot be zero");
            m_descriptor = descriptor;
            m_elementCount = descriptor.m_capacityInBytes / descriptor.m_elementSize;
            assert(m_elementCount, "Block size is larger than pool size.");

            m_freeList.resize(m_elementCount);
            for (uint32_t i = 0; i < m_elementCount; ++i)
            {
                m_freeList[i] = m_elementCount - i - 1;
            }
        }

        void PoolAllocator::Shutdown()
        {
            while (m_garbage.size())
            {
                m_garbage.pop();
            }
            m_freeList.clear();
        }

        VirtualAddress PoolAllocator::Allocate(size_t byteCount, size_t byteAlignment)
        {
            (void)byteAlignment;
            if (byteCount == 0)
            {
                return VirtualAddress::CreateNull();
            }

            assert(byteAlignment <= m_descriptor.m_elementSize, "Can't support this alignment requirement.");
            assert(byteCount <= static_cast<size_t>(m_descriptor.m_elementSize), "Can't support an allocation of this size.");

            if (m_freeList.empty())
            {
                return VirtualAddress::CreateNull();
            }

            VirtualAddress address;
            address.m_ptr = m_descriptor.m_addressBase.m_ptr + m_freeList.back() * m_descriptor.m_elementSize;
            m_freeList.pop_back();
            m_allocationCountTotal++;
            return address;
        }

        void PoolAllocator::DeAllocate(VirtualAddress allocation)
        {
            VirtualAddress normalizedAddress;
            normalizedAddress.m_ptr = allocation.m_ptr - m_descriptor.m_addressBase.m_ptr;
            assert(IsDivisible<size_t>(normalizedAddress.m_ptr, m_descriptor.m_elementSize), "Allocation is not properly aligned");

            uint32_t index = static_cast<uint32_t>(normalizedAddress.m_ptr / m_descriptor.m_elementSize);
            m_garbage.emplace(index, m_garbageCollectCycle);
        }

        void PoolAllocator::GarbageCollect()
        {
            while (m_garbage.size() && IsGarbageReady(m_garbage.front()))
            {
                m_freeList.push_back(m_garbage.front().m_index);
                m_allocationCountTotal--;
                m_garbage.pop();
            }
            m_garbageCollectCycle++;
        }

        void PoolAllocator::GarbageCollectForce()
        {
            while (m_garbage.size())
            {
                m_freeList.push_back(m_garbage.front().m_index);
                m_allocationCountTotal--;
                m_garbage.pop();
            }
        }
    }
}