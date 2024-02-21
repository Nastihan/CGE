#pragma once

// RHI
#include "RHI_Common.h"
#include "IndexBufferView.h"

namespace CGE
{
    namespace RHI
    {
        class Buffer;

        enum class IndexFormat : uint32_t
        {
            Uint16 = 0,
            Uint32
        };

        class IndexBufferView
        {
        public:
            IndexBufferView() = default;
            IndexBufferView(const Buffer& buffer, uint32_t byteOffset, uint32_t byteCount, IndexFormat format);
            const Buffer* GetBuffer() const;
            uint32_t GetByteOffset() const;
            uint32_t GetByteCount() const;
            IndexFormat GetIndexFormat() const;

        private:
            const Buffer* m_buffer = nullptr;
            uint32_t m_byteOffset = 0;
            uint32_t m_byteCount = 0;
            IndexFormat m_format = IndexFormat::Uint32;
        };
    }
}