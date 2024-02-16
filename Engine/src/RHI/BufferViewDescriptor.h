#pragma once

// RHI
#include "RHI_Common.h"
#include "Format.h"
#include "TypeHash.h"
#include "BufferDescriptor.h"

namespace CGE
{
	namespace RHI
	{
        // Buffer views describe how to interpret a region of memory in a buffer.
        struct BufferViewDescriptor
        {
            BufferViewDescriptor() = default;
            bool operator==(const BufferViewDescriptor& other) const;

            // Creates a structured buffer view.
            static BufferViewDescriptor CreateStructured(uint32_t elementOffset, uint32_t elementCount, uint32_t elementSize);

            // Creates a raw (unsigned 32bit integral) buffer view.
            static BufferViewDescriptor CreateRaw(uint32_t byteOffset, uint32_t byteCount);

            // Creates a buffer with a fundamental type.
            static BufferViewDescriptor CreateTyped(uint32_t elementOffset, uint32_t elementCount, Format elementFormat);

            HashValue64 GetHash(HashValue64 seed = HashValue64{ 0 }) const;

            // Number of elements from the start of the buffer to offset.
            uint32_t m_elementOffset = 0;

            // The number of elements in the view.
            uint32_t m_elementCount = 0;

            // The size in bytes of each element.
            uint32_t m_elementSize = 0;

            // The format of each element. Should be Unknown for structured buffers, or R32 for raw buffers.
            Format m_elementFormat = Format::Unknown;

            BufferBindFlags m_overrideBindFlags = BufferBindFlags::None;
        };
	}
}