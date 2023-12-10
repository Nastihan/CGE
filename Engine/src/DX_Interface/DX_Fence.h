#pragma once

// DX12
#include "DX_CommonHeaders.h"
#include "DX_CommandQueue.h"

// RHI
#include "../RHI/Fence.h"
#include "../RHI/CommandQueue.h"

// std
#include <array>

namespace CGE
{
	namespace DX12
	{
		class DX_FenceEvent final
		{
		public:
			DX_FenceEvent() = delete;
			DX_FenceEvent(const char* name);
			~DX_FenceEvent();
			const char* GetName() const;
		private:
			friend class DX_Fence;
			HANDLE m_EventHandle;
			const char* m_name;
		};

		class DX_Fence final
		{
		public:
			RHI::ResultCode Init(ID3D12DeviceX* dxDevice, RHI::FenceState initialState);
			void Shutdown();
			uint64_t Increment();
			void Signal();
			void Wait(DX_FenceEvent& fenceEvent) const;
			void Wait(DX_FenceEvent& fenceEvent, uint64_t fenceValue) const;
			uint64_t GetPendingValue() const;
			uint64_t GetCompletedValue() const;
			RHI::FenceState GetFenceState() const;
			ID3D12Fence* Get() const;
			
		private:
			RHI::Ptr<ID3D12Fence> m_fence;
			// The fence will own its pending value that a queue must signal
			uint64_t m_pendingValue = 1;
		};

		// Wrapper around a set of fences, one for each command queue type.
		class DX_FenceSet final
		{
		public:
			DX_FenceSet() = default;
			void Init(ID3D12DeviceX* device, RHI::FenceState initialState);
			void Shutdown();
			void Wait(DX_FenceEvent& event) const;
			void Reset();
			DX_Fence& GetFence(RHI::HardwareQueueClass hardwareQueueClass);
			const DX_Fence& GetFence(RHI::HardwareQueueClass hardwareQueueClass) const;

		private:
			std::array<DX_Fence, RHI::HardwareQueueClassCount> m_fences;
		};

		/**
		 * The RHI fence implementation for DX12. This exists separately from Fence to decouple
		 * the RHI::Device instance from low-level queue management. This is because RHI::Fence holds
		 * a reference to the RHI device, which would create circular dependency issues if the device
		 * indirectly held a reference to one. Therefore, this implementation is only used when passing
		 * fences back and forth between the user and the RHI interface. Low-level systems will use
		 * the internal Fence instance instead.
		 */
		class DX_FenceImpl final : public RHI::Fence
		{
		public:
			static RHI::Ptr<DX_FenceImpl> Create();
			DX12::DX_Fence& Get();

		private:
			DX_FenceImpl() = default;

			// RHI::Fence
			RHI::ResultCode InitInternal(RHI::Device& device, RHI::FenceState initialState) override;
			void ShutdownInternal() override;
			void SignalOnCpuInternal() override;
			void WaitOnCpuInternal() const override;
			void ResetInternal() override;
			RHI::FenceState GetFenceStateInternal() const override;

		private:
			DX12::DX_Fence m_fence;
		};
	}
}