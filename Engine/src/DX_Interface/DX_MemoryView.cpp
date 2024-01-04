
// DX12
#include "DX_MemoryView.h"
#include "DX_Conversions.h"

namespace CGE
{
	namespace DX12
	{
		DX_MemoryView::DX_MemoryView(RHI::Ptr<DX_Memory> memory, size_t offset, size_t size, size_t alignment, DX_MemoryViewType viewType)
			: DX_MemoryView(DX_MemoryAllocation(memory, offset, size, alignment), viewType) {}

		DX_MemoryView::DX_MemoryView(const DX_MemoryAllocation& memoryAllocation, DX_MemoryViewType viewType) : m_memoryAllocation(memoryAllocation), m_viewType(viewType)
		{
			Construct();
		}

		bool DX_MemoryView::IsValid() const
		{
			return m_memoryAllocation.m_memory != nullptr;
		}

		size_t DX_MemoryView::GetOffset() const
		{
			return m_memoryAllocation.m_offset;
		}

		size_t DX_MemoryView::GetSize() const
		{
			return m_memoryAllocation.m_size;
		}

		size_t DX_MemoryView::GetAlignment() const
		{
			return m_memoryAllocation.m_alignment;
		}

		DX_Memory* DX_MemoryView::GetMemory() const
		{
			return m_memoryAllocation.m_memory.get();
		}

		DX_CpuVirtualAddress DX_MemoryView::Map(RHI::HostMemoryAccess hostAccess) const
		{
			DX_CpuVirtualAddress cpuAddress = nullptr;
			D3D12_RANGE readRange = {};
			if (hostAccess == RHI::HostMemoryAccess::Read)
			{
				readRange.Begin = m_memoryAllocation.m_offset;
				readRange.End = m_memoryAllocation.m_offset + m_memoryAllocation.m_size;
			}
			m_memoryAllocation.m_memory->Map(0, &readRange, reinterpret_cast<void**>(&cpuAddress));

			if (cpuAddress)
			{
				cpuAddress += m_memoryAllocation.m_offset;
			}
			return cpuAddress;
		}

		void DX_MemoryView::Unmap(RHI::HostMemoryAccess hostAccess) const
		{
			D3D12_RANGE writeRange = {};
			if (hostAccess == RHI::HostMemoryAccess::Write)
			{
				writeRange.Begin = m_memoryAllocation.m_offset;
				writeRange.End = m_memoryAllocation.m_offset + m_memoryAllocation.m_size;
			}
			m_memoryAllocation.m_memory->Unmap(0, &writeRange);
		}

		DX_GpuVirtualAddress DX_MemoryView::GetGpuAddress() const
		{
			return m_gpuAddress;
		}

		void DX_MemoryView::SetName(const std::string& name)
		{
			if (m_memoryAllocation.m_memory)
			{
				std::wstring wname = s2ws(name);
				m_memoryAllocation.m_memory->SetName(wname.c_str());
			}
		}

		void DX_MemoryView::SetName(const std::wstring& name)
		{
			if (m_memoryAllocation.m_memory)
			{
				m_memoryAllocation.m_memory->SetName(name.c_str());
			}
		}

		void DX_MemoryView::Construct()
		{
			if (m_memoryAllocation.m_memory)
			{
				if (m_viewType == DX_MemoryViewType::Image)
				{
					m_gpuAddress = 0;
				}
				else
				{
					m_gpuAddress = m_memoryAllocation.m_memory->GetGPUVirtualAddress();
				}
				m_gpuAddress += m_memoryAllocation.m_offset;
			}
		}
	}
}