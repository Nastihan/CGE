
// RHI
#include "BufferViewDescriptor.h"

namespace CGE
{
	namespace RHI
	{
		BufferViewDescriptor BufferViewDescriptor::CreateStructured(uint32_t elementOffset, uint32_t elementCount, uint32_t elementSize)
		{
			BufferViewDescriptor descriptor;
			descriptor.m_elementOffset = elementOffset;
			descriptor.m_elementCount = elementCount;
			descriptor.m_elementSize = elementSize;
			descriptor.m_elementFormat = Format::Unknown;
			return descriptor;
		}

		BufferViewDescriptor BufferViewDescriptor::CreateRaw(uint32_t byteOffset, uint32_t byteCount)
		{
			BufferViewDescriptor descriptor;

			/*
			* shifting considering each element is 4-bytes
			* if we shift two bits to the left is like dividing by 4.
			* example: byteOffset = 8, byteCount = 16
			* m_elementOffset = 8 / 4 = 2, m_elementCount = 16 / 4 = 4
			*/
			descriptor.m_elementOffset = byteOffset >> 2;
			descriptor.m_elementCount = byteCount >> 2;
			descriptor.m_elementSize = 4;
			descriptor.m_elementFormat = Format::R32_UINT;
			return descriptor;
		}

		BufferViewDescriptor BufferViewDescriptor::CreateTyped(uint32_t elementOffset, uint32_t elementCount, Format elementFormat)
		{
			BufferViewDescriptor descriptor;
			descriptor.m_elementOffset = elementOffset;
			descriptor.m_elementCount = elementCount;
			descriptor.m_elementSize = GetFormatSize(elementFormat);
			descriptor.m_elementFormat = elementFormat;
			return descriptor;
		}

		HashValue64 BufferViewDescriptor::GetHash(HashValue64 seed) const
		{
			return TypeHash64(*this, seed);
		}

		bool BufferViewDescriptor::operator==(const BufferViewDescriptor& other) const
		{
			return m_elementOffset == other.m_elementOffset &&
				m_elementCount == other.m_elementCount &&
				m_elementSize == other.m_elementSize &&
				m_elementFormat == other.m_elementFormat &&
				m_overrideBindFlags == other.m_overrideBindFlags;
		}
	}
}