#pragma once

// DX12
#include "DX_MemoryPageAllocator.h"

// RHI
#include "../RHI/MemorySubAllocator.h"

namespace CGE
{
	namespace DX12
	{
		using DX_MemoryFreeListSubAllocatorTraits = RHI::MemorySubAllocatorTraits<DX_Memory, DX_MemoryPageAllocator, RHI::FreeListAllocator>;
		using DX_MemoryFreeListSubAllocator = RHI::MemorySubAllocator<DX_MemoryFreeListSubAllocatorTraits>;

		using DX_MemoryPoolSubAllocatorTraits = RHI::MemorySubAllocatorTraits<DX_Memory, DX_MemoryPageAllocator, RHI::PoolAllocator>;
		using DX_MemoryPoolSubAllocator = RHI::MemorySubAllocator<DX_MemoryPoolSubAllocatorTraits>;
	}
}