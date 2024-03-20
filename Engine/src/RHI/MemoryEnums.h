#pragma once

// RHI
#include "RHI_Common.h"

namespace CGE
{
	namespace RHI
	{
		// These enums will result in choosing the heap properties.
		// Check ConvertHeapType function in DX_Conversions.h for example.

		// Used for host visible memory. if device heap is used, use HostMemoryAccess::Write.
		enum class HostMemoryAccess : uint32_t
		{
			// fast for writes CPU->GPU
			Write = 0,
			// used for reads GPU->CPU
			Read
		};

		enum class HeapMemoryLevel : uint32_t
		{
			// Host memory local to CPU
			Host = 0,
			// Device memory local to discrete GPU (require a staging transfer to GPU fast memory)
			Device
		};
	}
}