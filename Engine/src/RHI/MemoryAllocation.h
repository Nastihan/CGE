#pragma once

#include "RHI_Common.h"

namespace CGE
{
	namespace RHI
	{
		// Check DX_Device::CreateImageCommitted and DX_Device::CreateBufferCommitted
		template<typename MemoryType>
		struct MemoryAllocation
		{
			MemoryAllocation() = default;
			MemoryAllocation(Ptr<MemoryType> memory, size_t offset, size_t size, size_t alignment);

			Ptr<MemoryType> m_memory;
			size_t m_offset = 0;
			size_t m_size = 0;
			size_t m_alignment = 0;
		};

		template<typename MemoryType>
		MemoryAllocation<MemoryType>::MemoryAllocation(Ptr<MemoryType> memory, size_t offset, size_t size, size_t alignment) 
			: m_memory(memory), m_offset(offset), m_size(size), m_alignment(alignment) {}
	}
}