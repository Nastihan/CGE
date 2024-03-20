#pragma once

// DX12
#include "DX_MemoryPageAllocator.h"

// RHI
#include "../RHI/MemorySubAllocator.h"
#include "../RHI//MemoryLinearSubAllocator.h"

namespace CGE
{
	namespace DX12
	{
		using DX_MemoryFreeListSubAllocatorTraits = RHI::MemorySubAllocatorTraits<DX_Memory, DX_MemoryPageAllocator, RHI::FreeListAllocator>;
		using DX_MemoryFreeListSubAllocator = RHI::MemorySubAllocator<DX_MemoryFreeListSubAllocatorTraits>;

		using DX_MemoryPoolSubAllocatorTraits = RHI::MemorySubAllocatorTraits<DX_Memory, DX_MemoryPageAllocator, RHI::PoolAllocator>;
		using DX_MemoryPoolSubAllocator = RHI::MemorySubAllocator<DX_MemoryPoolSubAllocatorTraits>;

		using DX_MemoryLinearSubAllocatorTraits = RHI::MemoryLinearSubAllocatorTraits<DX_Memory, DX_MemoryPageAllocator>;
		using DX_MemoryLinearSubAllocator = RHI::MemorySubAllocator<DX_MemoryLinearSubAllocatorTraits>;
	}
}