#pragma once

// RHI
#include "RHI_Common.h"

namespace CGE
{
	namespace RHI
	{
		enum class HardwareQueueClass : uint32_t
		{
			Graphics = 0,
			Compute,
			Copy,
			Count
		};

		enum class HardwareQueueClassMask : uint32_t
		{
			None = 0,
			Graphics = (1u << static_cast<uint32_t>(HardwareQueueClass::Graphics)),
			Compute = (1u << static_cast<uint32_t>(HardwareQueueClass::Compute)),
			Copy = (1u << static_cast<uint32_t>(HardwareQueueClass::Copy)),
			All = (Graphics | Compute | Copy)
		};
	}
}