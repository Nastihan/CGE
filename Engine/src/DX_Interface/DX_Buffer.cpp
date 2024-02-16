
// DX12
#include "DX_Buffer.h"

namespace CGE
{
	namespace DX12
	{
		RHI::Ptr<DX_Buffer> DX_Buffer::Create()
		{
			return new DX_Buffer();
		}

		const DX_MemoryView& DX_Buffer::GetMemoryView() const
		{
			return m_memoryView;
		}

		DX_MemoryView& DX_Buffer::GetMemoryView()
		{
			return m_memoryView;
		}

		void DX_Buffer::SetNameInternal(const std::string& name)
		{
			if (m_memoryView.GetType() == DX_BufferMemoryType::Unique)
			{
				m_memoryView.SetName(name);
			}
		}
	}
}