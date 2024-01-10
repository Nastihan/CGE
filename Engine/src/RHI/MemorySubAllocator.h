#pragma once

#include "FreeListAllocator.h"
#include "PoolAllocator.h"
#include "MemoryAllocation.h"
#include "MemoryEnums.h"

namespace CGE
{
    namespace RHI
    {
        // MemorySubAllocator allocates pages from a pool, and then uses a custom allocator to sub-allocate from each page.
        template <typename MemoryType, typename MemoryPageAllocatorType, typename AllocatorType>
        struct MemorySubAllocatorTraits
        {
            using memory_type = MemoryType;
            using page_allocator_type = MemoryPageAllocatorType;
            using allocator_type = AllocatorType;
        };


        template <typename Traits>
        class MemorySubAllocator
        {
        public:
            using this_type = MemorySubAllocator<Traits>;
            using traits_type = Traits;
            using memory_type = typename Traits::memory_type;
            using memory_type_pointer = memory_type*;
            using page_allocator_type = typename Traits::page_allocator_type;
            using page_allocator_pointer = page_allocator_type*;
            using page_allocator_reference = page_allocator_type&;
            using allocator_type = typename Traits::allocator_type;
            using memory_allocation = MemoryAllocation<memory_type>;
            using const_memory_allocation_reference = const memory_allocation&;

            struct Descriptor : allocator_type::Descriptor
            {
                // The number of garbage collect cycles a page is allowed to be inactive before it is deallocated.
                // The reason it is zero is the virtual address will get removed but the comitted resource will not.
                uint32_t m_inactivePageCycles = 0;
            };

            MemorySubAllocator() = default;
            MemorySubAllocator(const MemorySubAllocator&) = delete;


            void Init(const Descriptor& descriptor, page_allocator_reference pageAllocator);
            memory_allocation Allocate(size_t sizeInBytes, size_t alignmentInBytes);
            void DeAllocate(const_memory_allocation_reference pageView);
            void GarbageCollect();
            void Shutdown();

        private:
            page_allocator_pointer m_pageAllocator = nullptr;
            Descriptor m_descriptor;
            std::vector<memory_type_pointer> m_pages;

            struct PageContext
            {
                allocator_type m_allocator;
                uint32_t m_inactiveCycleCount = 0;
            };
            // The page context will be used to sub-allocate first. 
            std::vector<PageContext> m_pageContexts;
        };

        template <class Traits>
        void MemorySubAllocator<Traits>::Init(const Descriptor& descriptor, page_allocator_reference pageAllocator)
        {
            m_pageAllocator = &pageAllocator;
            m_descriptor = descriptor;
            m_descriptor.m_addressBase = 0;
            if (m_descriptor.m_capacityInBytes == 0)
            {
                m_descriptor.m_capacityInBytes = m_pageAllocator->GetPageSize();
            }
        }

        template <class Traits>
        typename MemorySubAllocator<Traits>::memory_allocation MemorySubAllocator<Traits>::Allocate(size_t sizeInBytes, size_t alignmentInBytes)
        {
            if (RHI::AlignUp(sizeInBytes, alignmentInBytes) > m_descriptor.m_capacityInBytes)
            {
                return memory_allocation();
            }

            size_t pageIdx = 0;
            RHI::VirtualAddress address;

            // Attempt to allocate from an existing page.
            for (; pageIdx < m_pageContexts.size(); ++pageIdx)
            {
                PageContext& pageContext = m_pageContexts[pageIdx];

                address = pageContext.m_allocator.Allocate(sizeInBytes, alignmentInBytes);
                if (address.IsValid())
                {
                    pageContext.m_inactiveCycleCount = 0;
                    break;
                }
            }

            // Create a new page if none of our current ones can service the request.
            if (address.IsNull())
            {
                memory_type_pointer newPage = m_pageAllocator->Allocate();
                if (newPage == nullptr)
                {
                    return memory_allocation();
                }

                m_pages.emplace_back(newPage);
                m_pageContexts.emplace_back();

                PageContext& pageContext = m_pageContexts.back();
                pageContext.m_allocator.Init(m_descriptor);

                address = pageContext.m_allocator.Allocate(sizeInBytes, alignmentInBytes);
                assert(address.IsValid(), "Failed to allocate from fresh page.");
            }

            return memory_allocation(m_pages[pageIdx], address.m_ptr, sizeInBytes, alignmentInBytes);
        }

        template <class Traits>
        void MemorySubAllocator<Traits>::DeAllocate(const_memory_allocation_reference allocation)
        {
            for (size_t i = 0; i < m_pages.size(); ++i)
            {
                if (allocation.m_memory == m_pages[i])
                {
                    m_pageContexts[i].m_allocator.DeAllocate(VirtualAddress::CreateFromOffset(allocation.m_offset));
                    break;
                }
            }
        }

        template <class Traits>
        void MemorySubAllocator<Traits>::GarbageCollect()
        {
            for (PageContext& pageContext : m_pageContexts)
            {
                pageContext.m_allocator.GarbageCollect();
            }

            // Release old pages that aren't in use anymore.
            size_t currentIdx = 0;
            while (currentIdx < m_pageContexts.size())
            {
                PageContext& pageContext = m_pageContexts[currentIdx];

                if (pageContext.m_allocator.GetAllocationCount() == 0)
                {
                    if (++pageContext.m_inactiveCycleCount > m_descriptor.m_inactivePageCycles)
                    {
                        pageContext.m_allocator.Shutdown();
                        pageContext = std::move(m_pageContexts.back());
                        m_pageContexts.pop_back();

                        m_pageAllocator->DeAllocate(m_pages[currentIdx]);
                        m_pages[currentIdx] = std::move(m_pages.back());
                        m_pages.pop_back();
                    }
                }
                currentIdx++;
            }
        }

        template <class Traits>
        void MemorySubAllocator<Traits>::Shutdown()
        {
            for (PageContext& pageContext : m_pageContexts)
            {
                pageContext.m_allocator.Shutdown();
            }
            m_pageContexts.clear();

            m_pageAllocator->DeAllocate(m_pages.data(), m_pages.size());
            m_pages.clear();
        }
    }
}