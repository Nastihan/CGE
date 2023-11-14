#include "DX_Fence.h"
#include "DX_Conversions.h"
#include "DX_Device.h"

namespace CGE
{
	namespace DX12
	{
		FenceEvent::FenceEvent(const char* name) : m_EventHandle(nullptr), m_name(name)
		{
			std::wstring wName;
			wName = s2ws(name);
			m_EventHandle = CreateEvent(nullptr, false, false, wName.c_str());
		}
		FenceEvent::~FenceEvent()
		{
			CloseHandle(m_EventHandle);
		}
		const char* FenceEvent::GetName() const
		{
			return m_name;
		}

		RHI::ResultCode Fence::Init(ID3D12DeviceX* dxDevice, RHI::FenceState initialState)
		{
			wrl::ComPtr<ID3D12Fence> fencePtr;
			if (!DXAssertSuccess(dxDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fencePtr.GetAddressOf()))))
			{
				return RHI::ResultCode::Fail;
			}

			m_fence = fencePtr.Get();
			m_fence->Signal(0);
			m_pendingValue = (initialState == RHI::FenceState::Signaled) ? 0 : 1;
			return RHI::ResultCode::Success;
		}
		void Fence::Shutdown()
		{
			m_fence = nullptr;
		}
		uint64_t Fence::Increment()
		{
			return m_pendingValue++;
		}
		void Fence::Signal()
		{
			m_fence->Signal(m_pendingValue);
		}
		void Fence::Wait(FenceEvent& fenceEvent) const
		{
			Wait(fenceEvent, m_pendingValue);
		}
		void Fence::Wait(FenceEvent& fenceEvent, uint64_t fenceValue) const
		{
			if (fenceValue > GetCompletedValue())
			{
				m_fence->SetEventOnCompletion(fenceValue, fenceEvent.m_EventHandle);
				WaitForSingleObject(fenceEvent.m_EventHandle, INFINITE);
			}
		}
		uint64_t Fence::GetPendingValue() const
		{
			return m_pendingValue;
		}
		uint64_t Fence::GetCompletedValue() const
		{
			return m_fence->GetCompletedValue();
		}
		RHI::FenceState Fence::GetFenceState() const
		{
			const uint64_t completedValue = GetCompletedValue();
			return (m_pendingValue <= completedValue) ? RHI::FenceState::Signaled : RHI::FenceState::Reset;
		}
		ID3D12Fence* Fence::Get() const
		{
			return m_fence.get();
		}

		void FenceSet::Init(ID3D12DeviceX* dxDevice, RHI::FenceState initialState)
		{
			for (uint32_t hardwareQueueIdx = 0; hardwareQueueIdx < RHI::HardwareQueueClassCount; hardwareQueueIdx++)
			{
				m_fences[hardwareQueueIdx].Init(dxDevice, initialState);
			}
		}
		void FenceSet::Shutdown()
		{
			for (uint32_t hardwareQueueIdx = 0; hardwareQueueIdx < RHI::HardwareQueueClassCount; hardwareQueueIdx++)
			{
				m_fences[hardwareQueueIdx].Shutdown();
			}
		}
		void FenceSet::Wait(FenceEvent& event) const
		{
			for (uint32_t hardwareQueueIdx = 0; hardwareQueueIdx < RHI::HardwareQueueClassCount; ++hardwareQueueIdx)
			{
				m_fences[hardwareQueueIdx].Wait(event);
			}
		}
		void FenceSet::Reset()
		{
			for (uint32_t hardwareQueueIdx = 0; hardwareQueueIdx < RHI::HardwareQueueClassCount; ++hardwareQueueIdx)
			{
				m_fences[hardwareQueueIdx].Increment();
			}
		}
		Fence& FenceSet::GetFence(RHI::HardwareQueueClass hardwareQueueClass)
		{
			return m_fences[static_cast<uint32_t>(hardwareQueueClass)];
		}
		const Fence& FenceSet::GetFence(RHI::HardwareQueueClass hardwareQueueClass) const
		{
			return m_fences[static_cast<uint32_t>(hardwareQueueClass)];
		}

		RHI::Ptr<FenceImpl> FenceImpl::Create()
		{
			return new FenceImpl();
		}
		DX12::Fence& FenceImpl::Get()
		{
			return m_fence;
		}
		RHI::ResultCode FenceImpl::InitInternal(RHI::Device& device, RHI::FenceState initialState)
		{
			return m_fence.Init(static_cast<DX_Device&>(device).GetDevice(), initialState);
		}
		void FenceImpl::ShutdownInternal()
		{
			m_fence.Shutdown();
		}
		void FenceImpl::SignalOnCpuInternal()
		{
			m_fence.Signal();
		}
		void FenceImpl::WaitOnCpuInternal() const
		{
			FenceEvent event("WaitOnCpu");
			m_fence.Wait(event);
		}
		void FenceImpl::ResetInternal()
		{
			m_fence.Increment();
		}
		RHI::FenceState FenceImpl::GetFenceStateInternal() const
		{
			return m_fence.GetFenceState();
		}
	}
}