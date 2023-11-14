#pragma once

#include "../RHI/Fence.h"
#include "DX_CommonHeaders.h"
#include "DX_CommandQueue.h"

#include <array>

namespace CGE
{
	namespace DX12
	{
		class FenceEvent final
		{
		public:
			FenceEvent() = delete;
			FenceEvent(const char* name);
			~FenceEvent();
			const char* GetName() const;
		private:
			friend class Fence;
			HANDLE m_EventHandle;
			const char* m_name;
		};

		class Fence final
		{
		public:
			RHI::ResultCode Init(ID3D12DeviceX* dxDevice, RHI::FenceState initialState);
			void Shutdown();
			uint64_t Increment();
			void Signal();
			void Wait(FenceEvent& fenceEvent) const;
			void Wait(FenceEvent& fenceEvent, uint64_t fenceValue) const;
			uint64_t GetPendingValue() const;
			uint64_t GetCompletedValue() const;
			RHI::FenceState GetFenceState() const;
			ID3D12Fence* Get() const;
			
		private:
			RHI::Ptr<ID3D12Fence> m_fence;
			uint64_t m_pendingValue = 1;
		};

		class FenceSet final
		{
		public:
			FenceSet() = default;
			void Init(ID3D12DeviceX* device, RHI::FenceState initialState);
			void Shutdown();
			void Wait(FenceEvent& event) const;
			void Reset();
			Fence& GetFence(RHI::HardwareQueueClass hardwareQueueClass);
			const Fence& GetFence(RHI::HardwareQueueClass hardwareQueueClass) const;

		private:
			std::array<Fence, RHI::HardwareQueueClassCount> m_fences;
		};

		class FenceImpl final : public RHI::Fence
		{
		public:
			static RHI::Ptr<FenceImpl> Create();
			DX12::Fence& Get();

		private:
			FenceImpl() = default;

			// RHI::Fence
			RHI::ResultCode InitInternal(RHI::Device& device, RHI::FenceState initialState) override;
			void ShutdownInternal() override;
			void SignalOnCpuInternal() override;
			void WaitOnCpuInternal() const override;
			void ResetInternal() override;
			RHI::FenceState GetFenceStateInternal() const override;

		private:
			DX12::Fence m_fence;
		};
	}
}