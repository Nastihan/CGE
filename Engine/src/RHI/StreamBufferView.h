#pragma once

// RHI
#include "RHI_Common.h"
#include "TypeHash.h"

namespace CGE
{
	namespace RHI
	{
        class Buffer;

		// Vertext Buffer View
		class StreamBufferView
		{
        public:
            StreamBufferView() = default;
            StreamBufferView(const Buffer& buffer, uint32_t byteOffset, uint32_t byteCount, uint32_t byteStride);
            const Buffer* GetBuffer() const;
            uint32_t GetByteOffset() const;
            uint32_t GetByteCount() const;
            uint32_t GetByteStride() const;
            HashValue64 GetHash() const;

        private:
            HashValue64 m_hash = HashValue64{ 0 };
            const Buffer* m_buffer = nullptr;
            uint32_t m_byteOffset = 0;
            uint32_t m_byteCount = 0;
            uint32_t m_byteStride = 0;
		};
	}
}