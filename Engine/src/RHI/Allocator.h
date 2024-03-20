#pragma once

#include "RHI_Common.h"

namespace CGE
{
	namespace RHI
	{
        // Virtual address that can be relative to a base resource address
        // Returned by allocation calls on the allocators
		class VirtualAddress
		{
			typedef unsigned __int64 uintptr_t;
            static const VirtualAddress Null;
        public:
            VirtualAddress();
            VirtualAddress(uintptr_t ptr);

            static VirtualAddress CreateNull();
            static VirtualAddress CreateZero();
            static VirtualAddress CreateFromPointer(void* ptr);
            static VirtualAddress CreateFromOffset(uint64_t offset);

            inline bool IsValid() const
            {
                return m_ptr != Null.m_ptr;
            }

            inline bool IsNull() const
            {
                return m_ptr == Null.m_ptr;
            }

            uintptr_t m_ptr;
		};

        // An allocator interface used for external GPU allocations with deferred-released based on GPU timeline.
        // This class does not own or manage any GPU objects internally.
        class Allocator
        {
        public:
            struct Descriptor
            {
                static const size_t DefaultAlignment = 256;

                Descriptor();

                // The base address added to every allocation (defaults to 0).
                VirtualAddress m_addressBase;

                // The minimum allocation size (and subsequent data alignment).
                size_t m_alignmentInBytes = DefaultAlignment;

                // The total size of the allocation region.
                size_t m_capacityInBytes = 0;

                // The number of GC cycles to wait before recycling a freed block.
                size_t m_garbageCollectLatency = 0;
            };

            virtual ~Allocator() {}

            virtual void Shutdown() = 0;

            // Allocates a virtual address relative to the base address provided at initialization time.
            virtual VirtualAddress Allocate(size_t byteCount, size_t byteAlignment) = 0;

            // Deallocates an allocation. The memory is not reclaimed until garbage collect is called.
            // Depending on the garbage collection latency, it may take several garbage collection cycles
            // before the memory is reclaimed.
            virtual void DeAllocate(VirtualAddress offset) = 0;
            virtual void GarbageCollect() = 0;
            virtual void GarbageCollectForce() = 0;

            // Includes allocations that are pending garbage collection.
            virtual size_t GetAllocationCount() const { return 0; }
            virtual size_t GetAllocatedByteCount() const { return 0; }
            virtual const Descriptor& GetDescriptor() const = 0;

            // Compute the fragmentation present in the allocator (possibly an estimate). The returned value is expected to be a measure
            // between 0.f and 1.f inclusive, with 0.f indicating zero fragmentation.
            virtual float ComputeFragmentation() const { return 0.f; }

            // Clone the current allocator to the new allocator passed in
            virtual void Clone(RHI::Allocator* newAllocator) {};

            // Helper for converting agnostic VirtualAddress type to pointer type. Will convert
            // VirtualAddress::Null to nullptr.
            template <typename T>
            T* AllocateAs(size_t byteCount, size_t byteAlignment)
            {
                VirtualAddress address = Allocate(byteCount, byteAlignment);
                return (address.IsValid()) ? reinterpret_cast<T*>(address.m_ptr) : nullptr;
            }
        };
	}
}