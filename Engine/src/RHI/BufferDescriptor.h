#pragma once

// RHI
#include "RHI_Common.h"
#include "AttachmentEnums.h"

namespace CGE
{
	namespace RHI
	{
		enum class BufferBindFlags : uint32_t
		{
			None = 0,
			InputAssembly = (1u << 0u),
			DynamicInputAssembly = (1u << 1u),
			Constant = (1u << 2u),
			ShaderRead = (1u << 3u),
			ShaderWrite = (1u << 4u),
			ShaderReadWrite = (ShaderRead | ShaderWrite),
			CopyRead = (1u << 5u),
			CopyWrite = (1u << 6u),
			Prediction = (1u << 7u),
			Indirect = (1u << 8u)
		};

		struct BufferDescriptor
		{
			u64 m_byteCount = 0;
			u64 m_alignment = 0;
			BufferBindFlags m_bindFlags = BufferBindFlags::None;
			HardwareQueueClassMask m_sharedQueueMask = HardwareQueueClassMask::All;
		};
	}
}