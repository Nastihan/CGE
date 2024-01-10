
// DX12
#include "DX_BufferMemoryView.h"

namespace CGE
{
	namespace DX12
	{
        DX_BufferMemoryView::DX_BufferMemoryView(const DX_MemoryView& memoryView, DX_BufferMemoryType memoryType) : DX_MemoryView(memoryView) , m_type{ memoryType } {}

        DX_BufferMemoryView::DX_BufferMemoryView(DX_MemoryView&& memoryView, DX_BufferMemoryType memoryType) : DX_MemoryView(std::move(memoryView)) , m_type{ memoryType } {}

        DX_BufferMemoryType DX_BufferMemoryView::GetType() const
        {
            return m_type;
        }
	}
}