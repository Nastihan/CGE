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

		enum class ScopeAttachmentAccess : uint32_t
		{
			Unknown = 0,
			Read = (1u << 0),
			Write = (1u << 1),
			ReadWrite = Read | Write
		};

        // Describes the action the hardware should use when loading an attachment prior to a scope.
        enum class AttachmentLoadAction : uint8_t
        {
            // The attachment contents should be preserved (loaded from memory).
            Load = 0,

            // The attachment contents should be cleared (using the provided clear value).
            Clear,

            // The attachment contents are undefined. Use when writing to entire contents of view.
            DontCare
        };

        // Describes the action the hardware should use when storing an attachment after a scope.
        enum class AttachmentStoreAction : uint8_t
        {
            // The attachment contents must be preserved after the current scope.
            Store = 0,

            // The attachment contents can be undefined after the current scope.
            DontCare
        };

        // Describes the type of data the attachment represents
        enum class AttachmentType : uint8_t
        {
            Image = 0,
            Buffer,
            Resolve,
            Uninitialized
        };
	}
}