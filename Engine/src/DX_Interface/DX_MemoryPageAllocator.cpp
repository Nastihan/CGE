
// DX12
#include "DX_MemoryPageAllocator.h"
#include "DX_Conversions.h"
#include "DX_Device.h"

namespace CGE
{
	namespace DX12
	{
		void DX_MemoryPageFactory::Init(const Descriptor& descriptor)
		{
			assert(descriptor.m_getHeapMemoryUsageFunction);

			m_descriptor = descriptor;
			m_descriptor.m_pageSizeInBytes = RHI::AlignUp(m_descriptor.m_pageSizeInBytes, DX_Alignment::CommittedBuffer);
			m_heapType = ConvertHeapType(descriptor.m_heapMemoryLevel, descriptor.m_hostMemoryAccess);
		}

		RHI::Ptr<DX_Memory> DX_MemoryPageFactory::CreateObject()
		{
			RHI::HeapMemoryUsage& heapMemoryUsage = *m_descriptor.m_getHeapMemoryUsageFunction();

			D3D12_RESOURCE_STATES initialResourceState = ConvertInitialResourceState(m_descriptor.m_heapMemoryLevel, m_descriptor.m_hostMemoryAccess);

			RHI::BufferDescriptor bufferDescriptor;
			bufferDescriptor.m_byteCount = m_descriptor.m_pageSizeInBytes;
			bufferDescriptor.m_bindFlags = m_descriptor.m_bindFlags;
			DX_MemoryView memoryView = m_descriptor.m_device->CreateBufferCommitted(bufferDescriptor, initialResourceState, m_heapType);

			if (memoryView.IsValid())
			{
				heapMemoryUsage.m_totalResidentInBytes += m_descriptor.m_pageSizeInBytes;
				memoryView.SetName(L"BufferPage");
			}
			
			return memoryView.GetMemory();
		}

		void DX_MemoryPageFactory::ShutdownObject(DX_Memory& memory, bool isPoolShutdown)
		{
			RHI::HeapMemoryUsage& heapMemoryUsage = *m_descriptor.m_getHeapMemoryUsageFunction();
			heapMemoryUsage.m_totalResidentInBytes -= m_descriptor.m_pageSizeInBytes;

			if (isPoolShutdown)
			{
				m_descriptor.m_device->QueueForRelease(&memory);
			}
		}

		bool DX_MemoryPageFactory::CollectObject(DX_Memory& memory)
		{
			// Why ?
			(void)memory;
			return m_descriptor.m_recycleOnCollect;
		}

		const DX_MemoryPageFactory::Descriptor& DX_MemoryPageFactory::GetDescriptor() const
		{
			return m_descriptor;
		}

		size_t DX_MemoryPageAllocator::GetPageCount() const
		{
			return GetObjectCount();
		}

		size_t DX_MemoryPageAllocator::GetPageSize() const
		{
			return GetFactory().GetDescriptor().m_pageSizeInBytes;
		}
	}
}