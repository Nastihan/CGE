#pragma once

// RHI
#include "RHI_Common.h"
#include "AttachmentEnums.h"
#include "TypeHash.h"

namespace CGE
{
	namespace RHI
	{
		// Used for creating descriptor views (check DX_BufferView class)
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


		inline BufferBindFlags operator|(BufferBindFlags a, BufferBindFlags b)
		{
			return static_cast<BufferBindFlags>(static_cast<int>(a) | static_cast<int>(b));
		}

		struct BufferDescriptor
		{
			HashValue64 GetHash(HashValue64 seed = HashValue64{ 0 }) const;

			u64 m_byteCount = 0;
			u64 m_alignment = 0;
			BufferBindFlags m_bindFlags = BufferBindFlags::None;
			HardwareQueueClassMask m_sharedQueueMask = HardwareQueueClassMask::All;
		};
	}
}