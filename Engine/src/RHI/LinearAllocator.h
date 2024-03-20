#pragma once

#include "Allocator.h"

namespace CGE
{
	namespace RHI
	{
        // A linear allocator where each allocation is a simple increment to the cursor. Garbage collection
        // controls when the cursor resets back to the beginning.
        class LinearAllocator final : public Allocator
        {
        public:
            struct Descriptor : Allocator::Descriptor
            {
                // No additional members.
            };

            LinearAllocator() = default;
            void Init(const Descriptor& descriptor);

            // Allocator
            void Shutdown() override;
            VirtualAddress Allocate(size_t byteCount, size_t byteAlignment) override;
            void DeAllocate(VirtualAddress allocation) override;
            void GarbageCollect() override;
            void GarbageCollectForce() override;
            size_t GetAllocatedByteCount() const override;
            const Descriptor& GetDescriptor() const override;

            inline void SetBaseAddress(VirtualAddress address)
            {
                m_descriptor.m_addressBase = address;
            }

        private:
            Descriptor m_descriptor;
            size_t m_byteOffsetCurrent = 0;
            size_t m_garbageCollectIteration = 0;
        };
	}
}