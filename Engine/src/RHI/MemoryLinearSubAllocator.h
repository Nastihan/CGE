#pragma once

// RHI
#include "LinearAllocator.h"
#include "MemorySubAllocator.h"

namespace CGE
{
	namespace RHI
	{
        template <typename MemoryType, typename MemoryPageAllocatorType>
        struct MemoryLinearSubAllocatorTraits : MemorySubAllocatorTraits<MemoryType, MemoryPageAllocatorType, LinearAllocator> {};


        // MemorySubAllocator specialization
        template <typename MemoryType, typename MemoryPageAllocatorType>
        class MemorySubAllocator<MemoryLinearSubAllocatorTraits<MemoryType, MemoryPageAllocatorType>>
        {
        public:
            using traits_type = MemoryLinearSubAllocatorTraits<MemoryType, MemoryPageAllocatorType>;
            using this_type = MemorySubAllocator<traits_type>;
            using memory_type = typename traits_type::memory_type;
            using memory_type_pointer = memory_type*;
            using page_allocator_type = typename traits_type::page_allocator_type;
            using page_allocator_pointer = page_allocator_type*;
            using page_allocator_reference = page_allocator_type&;
            using allocator_type = typename traits_type::allocator_type;
            using memory_allocation = MemoryAllocation<memory_type>;
            using const_memory_allocation_reference = const memory_allocation&;

            MemorySubAllocator() = default;
            MemorySubAllocator(MemorySubAllocator&&) = default;
            MemorySubAllocator(const MemorySubAllocator&) = delete;
            MemorySubAllocator& operator = (MemorySubAllocator&&) = default;
            ~MemorySubAllocator()
            {
                Shutdown();
            }

            void Init(page_allocator_reference pageAllocator)
            {
                m_pageAllocator = &pageAllocator;
                RHI::LinearAllocator::Descriptor allocatorDescriptor;
                allocatorDescriptor.m_capacityInBytes = m_pageAllocator->GetPageSize();
                m_allocator.Init(allocatorDescriptor);
            }

            memory_allocation Allocate(size_t sizeInBytes, size_t alignmentInBytes)
            {
                if (RHI::AlignUp(sizeInBytes, alignmentInBytes) > m_allocator.GetDescriptor().m_capacityInBytes)
                {
                    return memory_allocation();
                }

                RHI::VirtualAddress address;
                if (m_currentPage)
                {
                    address = m_allocator.Allocate(sizeInBytes, alignmentInBytes);
                }

                if (address.IsNull())
                {
                    if (m_currentPage)
                    {
                        m_retiredPages.push_back(m_currentPage);
                    }

                    m_allocator.GarbageCollect();
                    m_currentPage = m_pageAllocator->Allocate();

                    address = m_allocator.Allocate(sizeInBytes, alignmentInBytes);
                    assert(address.IsValid());
                }

                return memory_allocation(m_currentPage, address.m_ptr, sizeInBytes, alignmentInBytes);
            }

            void GarbageCollect()
            {
                if (m_currentPage)
                {
                    m_retiredPages.push_back(m_currentPage);
                    m_allocator.GarbageCollect();
                    m_currentPage = nullptr;
                }

                m_pageAllocator->DeAllocate(m_retiredPages.data(), m_retiredPages.size());
                m_retiredPages.clear();
            }

            void Shutdown()
            {
                if (m_pageAllocator)
                {
                    GarbageCollect();
                    m_allocator.Shutdown();
                    m_pageAllocator = nullptr;
                }
            }

        private:
            page_allocator_pointer m_pageAllocator = nullptr;
            memory_type_pointer m_currentPage = nullptr;
            allocator_type m_allocator;
            std::vector<memory_type_pointer> m_retiredPages;
        };
	}
}