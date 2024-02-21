
// RHI
#include "StreamBufferView.h"
#include "Buffer.h"

namespace CGE
{
	namespace RHI
	{
        StreamBufferView::StreamBufferView(const Buffer& buffer, uint32_t byteOffset, uint32_t byteCount, uint32_t byteStride) : 
            m_buffer{ &buffer }, 
            m_byteOffset{ byteOffset },
            m_byteCount{ byteCount },
            m_byteStride{ byteStride } {}

        const Buffer* StreamBufferView::GetBuffer() const
        {
            return m_buffer;
        }

        uint32_t StreamBufferView::GetByteOffset() const
        {
            return m_byteOffset;
        }

        uint32_t StreamBufferView::GetByteCount() const
        {
            return m_byteCount;
        }

        uint32_t StreamBufferView::GetByteStride() const
        {
            return m_byteStride;
        }
	}
}