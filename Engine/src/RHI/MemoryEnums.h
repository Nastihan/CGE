#pragma once

// RHI
#include "RHI_Common.h"

namespace CGE
{
	namespace RHI
	{
		// These enums will result in choosing the heap properties.
		// Check ConvertHeapType function in DX_Conversions.h for example.

		enum class HostMemoryAccess : uint32_t
		{
			Write = 0,
			Read
		};

		enum class HeapMemoryLevel : uint32_t
		{
			// Host memory local to CPU
			Host = 0,
			// Device memory local to discrete GPU
			Device
		};
	}
}