
// RHI
#include "RHI_Common.h"
#include "IndexBufferView.h"

namespace CGE
{
    namespace RHI
    {
        IndexBufferView::IndexBufferView(const Buffer& buffer, uint32_t byteOffset, uint32_t byteCount, IndexFormat format) : 
            m_buffer{ &buffer }, 
            m_byteOffset{ byteOffset },
            m_byteCount{ byteCount },
            m_format{ format } 
        {
            m_hash = TypeHash64(*this);
        }

        const Buffer* IndexBufferView::GetBuffer() const
        {
            return m_buffer;
        }

        uint32_t IndexBufferView::GetByteOffset() const
        {
            return m_byteOffset;
        }

        uint32_t IndexBufferView::GetByteCount() const
        {
            return m_byteCount;
        }

        IndexFormat IndexBufferView::GetIndexFormat() const
        {
            return m_format;
        }

        HashValue64 IndexBufferView::GetHash() const
        {
            return m_hash;
        }
    }
}