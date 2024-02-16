
// RHI
#include "LinearAllocator.h"

namespace CGE
{
	namespace RHI
	{
        void LinearAllocator::Init(const Descriptor& descriptor)
        {
            Shutdown();
            m_descriptor = descriptor;
        }

        void LinearAllocator::Shutdown()
        {
            GarbageCollectForce();
        }

        VirtualAddress LinearAllocator::Allocate(size_t byteCount, size_t byteAlignment)
        {
            if (byteCount == 0)
            {
                return VirtualAddress::CreateNull();
            }

            VirtualAddress addressCurrentAligned{ RHI::AlignUp(m_descriptor.m_addressBase.m_ptr + m_byteOffsetCurrent, byteAlignment) };
            size_t byteCountAligned = RHI::AlignUp(byteCount, byteAlignment);
            size_t byteOffsetAligned = addressCurrentAligned.m_ptr - m_descriptor.m_addressBase.m_ptr;
            size_t nextByteAddress = byteOffsetAligned + byteCountAligned;

            if (nextByteAddress > m_descriptor.m_capacityInBytes)
            {
                return VirtualAddress::CreateNull();
            }

            m_byteOffsetCurrent = nextByteAddress;
            return addressCurrentAligned;
        }

        void LinearAllocator::DeAllocate(VirtualAddress offset)
        {
            (void)offset;
        }

        void LinearAllocator::GarbageCollect()
        {
            if (m_garbageCollectIteration == m_descriptor.m_garbageCollectLatency)
            {
                GarbageCollectForce();
                m_garbageCollectIteration = 0;
            }
            else
            {
                ++m_garbageCollectIteration;
            }
        }

        void LinearAllocator::GarbageCollectForce()
        {
            m_byteOffsetCurrent = 0;
            m_garbageCollectIteration = 0;
        }

        size_t LinearAllocator::GetAllocatedByteCount() const
        {
            return m_byteOffsetCurrent;
        }

        const LinearAllocator::Descriptor& LinearAllocator::GetDescriptor() const
        {
            return m_descriptor;
        }
	}
}