#pragma once

// DX12
#include "DX_MemoryView.h"

namespace CGE
{
	namespace DX12
	{
        enum class DX_BufferMemoryType : uint32_t
        {
            // New commited buffer
            Unique,
            // SubAllocated in a existing page in the pool
            SubAllocated
        };

        class DX_BufferMemoryView : public DX_MemoryView
        {
        public:
            DX_BufferMemoryView() = default;
            DX_BufferMemoryView(const DX_MemoryView& memoryView, DX_BufferMemoryType memoryType);
            DX_BufferMemoryView(DX_MemoryView&& memoryView, DX_BufferMemoryType memoryType);

            DX_BufferMemoryView(const DX_BufferMemoryView& rhs) = default;
            DX_BufferMemoryView(DX_BufferMemoryView&& rhs) = default;
            DX_BufferMemoryView& operator=(const DX_BufferMemoryView& rhs) = default;
            DX_BufferMemoryView& operator=(DX_BufferMemoryView&& rhs) = default;

            DX_BufferMemoryType GetType() const;

        private:
            DX_BufferMemoryType m_type = DX_BufferMemoryType::Unique;
        };
	}
}