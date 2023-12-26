// DX12
#include "DX_CommandListPool.h"
#include "DX_Conversions.h"
#include "DX_CommandList.h"

namespace CGE
{
	namespace DX12
	{
		void DX_CommandAllocatorFactory::Init(const Descriptor& descriptor)
		{
			m_descriptor = descriptor;
		}
		RHI::Ptr<ID3D12CommandAllocator> DX_CommandAllocatorFactory::CreateObject()
		{
			wrl::ComPtr<ID3D12CommandAllocator> allocator;
			DXAssertSuccess(m_descriptor.m_dxDevice->CreateCommandAllocator(ConvertHardwareQueueClass(m_descriptor.m_hardwareQueueClass), IID_PPV_ARGS(allocator.GetAddressOf())));
			return allocator.Get();
		}
		void DX_CommandAllocatorFactory::ResetObject(ID3D12CommandAllocator& allocator)
		{
			allocator.Reset();
		}

		void DX_CommandListFactory::Init(const Descriptor& descriptor)
		{
			m_descriptor = descriptor;
		}
		RHI::Ptr<DX_CommandList> DX_CommandListFactory::CreateObject(ID3D12CommandAllocator* commandAllocator)
		{
			RHI::Ptr<DX_CommandList> commandList = DX_CommandList::Create();
			commandList->Init(*m_descriptor.m_dxDevice, m_descriptor.m_hardwareQueueClass, commandAllocator, m_descriptor.m_descriptorContext);
			return commandList;
		}
		void DX_CommandListFactory::ResetObject(DX_CommandList& commandList, ID3D12CommandAllocator* commandAllocator)
		{
			commandList.Reset(commandAllocator);
		}
		void DX_CommandListFactory::ShutdownObject(DX_CommandList& commandList, bool isPoolShutdown)
		{
			commandList.Shutdown();
		}
		bool DX_CommandListFactory::CollectObject(DX_CommandList& commandList)
		{
			return true;
		}

		void DX_CommandListSubAllocator::Init(DX_CommandAllocatorPool& commandAllocatorPool, DX_CommandListPool& commandListPool)
		{
			m_commandAllocatorPool = &commandAllocatorPool;
			m_commandListPool = &commandListPool;
		}
		DX_CommandList* DX_CommandListSubAllocator::Allocate()
		{
			if (!m_currentAllocator)
			{
				m_currentAllocator = m_commandAllocatorPool->Allocate();
			}
			DX_CommandList* commandList = m_commandListPool->Allocate(m_currentAllocator);
			m_activeLists.push_back(commandList);
			return commandList;
		}
		void DX_CommandListSubAllocator::Reset()
		{
			if (m_currentAllocator)
			{
				m_commandListPool->DeAllocate(m_activeLists.data(), (uint32_t)m_activeLists.size());
				m_activeLists.clear();

				m_commandAllocatorPool->DeAllocate(m_currentAllocator);
				m_currentAllocator = nullptr;
			}
		}

		void DX_CommandListAllocator::Init(const Descriptor& descriptor)
		{
			if (m_isInitialized)
			{
				return;
			}
			for (uint32_t queueIdx = 0; queueIdx < RHI::HardwareQueueClassCount; queueIdx++)
			{
				DX_CommandListPool& commandListPool = m_commandListPools[queueIdx];
				DX_CommandAllocatorPool& commandAllocatorPool = m_commandAllocatorPools[queueIdx];

				DX_CommandListPool::Descriptor commandListPoolDescriptor;
				commandListPoolDescriptor.m_dxDevice = descriptor.m_dxDevice;
				commandListPoolDescriptor.m_hardwareQueueClass = static_cast<RHI::HardwareQueueClass>(queueIdx);
				commandListPoolDescriptor.m_collectLatency = descriptor.m_frameCountMax;
				commandListPool.Init(commandListPoolDescriptor);

				DX_CommandAllocatorPool::Descriptor commandAllocatorPoolDescriptor;
				commandAllocatorPoolDescriptor.m_hardwareQueueClass = static_cast<RHI::HardwareQueueClass>(queueIdx);
				commandAllocatorPoolDescriptor.m_dxDevice = descriptor.m_dxDevice->GetDevice();
				commandAllocatorPoolDescriptor.m_collectLatency = descriptor.m_frameCountMax;
				commandAllocatorPool.Init(commandAllocatorPoolDescriptor);

				m_commandListSubAllocators[queueIdx].Init(commandAllocatorPool, commandListPool);
			}
			m_isInitialized = true;
		}
		void DX_CommandListAllocator::Shutdown()
		{
			if (m_isInitialized)
			{
				for (uint32_t queueIdx = 0; queueIdx < RHI::HardwareQueueClassCount; queueIdx++)
				{
					m_commandListSubAllocators[queueIdx].Reset();
					m_commandListPools[queueIdx].Shutdown();
					m_commandAllocatorPools[queueIdx].Shutdown();
				}
				m_isInitialized = false;
			}
		}
		DX_CommandList* DX_CommandListAllocator::Allocate(RHI::HardwareQueueClass hardwareQueueClass)
		{
			return m_commandListSubAllocators[static_cast<uint32_t>(hardwareQueueClass)].Allocate();
		}
		void DX_CommandListAllocator::Collect()
		{
			for (uint32_t queueIdx = 0; queueIdx < RHI::HardwareQueueClassCount; ++queueIdx)
			{
				m_commandListSubAllocators[queueIdx].Reset();
				m_commandListPools[queueIdx].Collect();
				m_commandAllocatorPools[queueIdx].Collect();
			}
		}
	}
}