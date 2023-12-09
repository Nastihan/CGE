#pragma once

// DX12
#include "DX_CommonHeaders.h"

// RHI
#include "../RHI/ObjectPool.h"
#include "../RHI/CommandQueue.h"

//std
#include <array>

namespace CGE
{
	namespace DX12
	{
		class DX_CommandList;
		class DX_Device;

		// CommandAllocatorPool
		class DX_CommandAllocatorFactory final : public RHI::ObjectFactoryBase<ID3D12CommandAllocator>
		{
		public:
			struct Descriptor
			{
				RHI::HardwareQueueClass m_hardwareQueueClass = RHI::HardwareQueueClass::Graphics;
				RHI::Ptr<ID3D12DeviceX> m_dxDevice;
			};

			void Init(const Descriptor& descriptor);
			RHI::Ptr<ID3D12CommandAllocator> CreateObjct();
			void ResetObject(ID3D12CommandAllocator& allocator);

		private:
			Descriptor m_descriptor;
		};
		struct DX_CommandAllocatorPoolTraits : public RHI::ObjectPoolTraits
		{
			using ObjectType = ID3D12CommandAllocator;
			using ObjectFactoryType = DX_CommandAllocatorFactory;
			using MutexType = std::mutex;
		};
		using DX_CommandAllocatorPool = RHI::ObjectPool<DX_CommandAllocatorPoolTraits>;

		// CommandListPool
		class DX_CommandListFactory final : public RHI::ObjectFactoryBase<DX_CommandList>
		{
			using Base = RHI::ObjectFactoryBase<DX_CommandList>;
		public:
			struct Descriptor
			{
				DX_Device* m_dxDevice;
				RHI::HardwareQueueClass m_hardwareQueueClass = RHI::HardwareQueueClass::Graphics;
			};

			void Init(const Descriptor& descriptor);
			RHI::Ptr<DX_CommandList> CreateObject(ID3D12CommandAllocator* commandAllocator);
			void ResetObject(DX_CommandList& commandList, ID3D12CommandAllocator* commandAllocator);
			void ShutdownObject(DX_CommandList& commandList, bool isPoolShutdown);
			bool CollectObject(DX_CommandList& commandList);

		private:
			Descriptor m_descriptor;
		};
		struct DX_CommandListPoolTraits : public RHI::ObjectPoolTraits
		{
			using ObjectType = DX_CommandList;
			using ObjectFactoryType = DX_CommandListFactory;
			using MutexType = std::recursive_mutex;
		};
		using DX_CommandListPool = RHI::ObjectPool<DX_CommandListPoolTraits>;


		class DX_CommandListSubAllocator final
		{
		public:
			DX_CommandListSubAllocator() = default;
			DX_CommandListSubAllocator(const DX_CommandListSubAllocator&) = delete;

			void Init(DX_CommandAllocatorPool& commandAllocatorPool, DX_CommandListPool& commandListPool);
			DX_CommandList* Allocate();
			void Reset();

		private:
			ID3D12CommandAllocator* m_currentAllocator = nullptr;
			DX_CommandAllocatorPool* m_commandAllocatorPool = nullptr;
			DX_CommandListPool* m_commandListPool = nullptr;
			std::vector<DX_CommandList*> m_activeLists;
		};

		class DX_CommandListAllocator final
		{
		public:
			DX_CommandListAllocator() = default;

			struct Descriptor
			{
				DX_Device* m_dxDevice = nullptr;
				uint32_t m_frameCountMax = RHI::Limits::Device::FrameCountMax;
			};

			void Init(const Descriptor& descriptor);
			void Shutdown();
			DX_CommandList* Allocate(RHI::HardwareQueueClass hardwareQueueClass);
			void Collect();

		private:
			bool m_isInitialized = false;

			std::array<DX_CommandListPool, RHI::HardwareQueueClassCount> m_commandListPools;
			std::array<DX_CommandAllocatorPool, RHI::HardwareQueueClassCount> m_commandAllocatorPools;

			// Graphics, Compute and Copy => idx 0, 1, 2
			std::array<DX_CommandListSubAllocator, RHI::HardwareQueueClassCount> m_commandListSubAllocators;
		};
	}
}