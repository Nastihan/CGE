#pragma once

// DX12
#include "DX_MemoryView.h"

// RHI
#include "../RHI/ObjectPool.h"
#include "../RHI/BufferDescriptor.h"
#include "../RHI/MemoryUsage.h"

namespace CGE
{
	namespace DX12
	{
		class DX_Device;

		class DX_MemoryPageFactory : public RHI::ObjectFactoryBase<DX_Memory>
		{
		public:
			using GetHeapMemoryUsageFunction = std::function<RHI::HeapMemoryUsage* ()>;

			struct Descriptor
			{
				DX_Device* m_device = nullptr;
				uint32_t m_pageSizeInBytes = 0;
				RHI::BufferBindFlags m_bindFlags = RHI::BufferBindFlags::None;
				RHI::HeapMemoryLevel m_heapMemoryLevel = RHI::HeapMemoryLevel::Host;
				RHI::HostMemoryAccess m_hostMemoryAccess = RHI::HostMemoryAccess::Write;
				GetHeapMemoryUsageFunction m_getHeapMemoryUsageFunction;
				bool m_recycleOnCollect = false;
			};

			void Init(const Descriptor& descriptor);
			RHI::Ptr<DX_Memory> CreateObject();
			void ShutdownObject(DX_Memory& memory, bool isPoolShutdown);
			bool CollectObject(DX_Memory& memory);
			const Descriptor& GetDescriptor() const;

		private:
			Descriptor m_descriptor;
			D3D12_HEAP_TYPE m_heapType = D3D12_HEAP_TYPE_DEFAULT;
		};

		class DX_MemoryPageAllocatorTraits : RHI::ObjectPoolTraits
		{
		public:
			using ObjectType = DX_Memory;
			using ObjectFactoryType = DX_MemoryPageFactory;
			using MutexType = std::mutex;
		};

		class DX_MemoryPageAllocator : public RHI::ObjectPool<DX_MemoryPageAllocatorTraits>
		{
		public:
			size_t GetPageCount() const;
			size_t GetPageSize() const;
		};
	}
}