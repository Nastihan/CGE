#pragma once

//DX12
#include "DX_CommonHeaders.h"
#include "DX_CommandListPool.h"
#include "DX_CommandQueueContext.h"
#include "DX_DescriptorContext.h"

// RHI
#include "../RHI/RHI_Common.h"
#include "../RHI/Device.h"

// std
#include <vector>
#include <mutex>

namespace CGE
{
	namespace RHI
	{
		struct BufferDescriptor;
	}

	namespace DX12
	{
		class DX_MemoryView;

		// [todo] might change from being a singleton
		class DX_Device final : public RHI::Device
		{
		public:
			~DX_Device() = default;
			static RHI::Ptr<RHI::Device> Create();

			ID3D12DeviceX* GetDevice() const;
			const wrl::ComPtr<IDXGIFactoryX>& GetDxgiFactory() const;
			std::string GetDeviceRemovedReason() const;
			void OnDeviceRemoved();
			bool DXAssertSuccess(HRESULT hr);
			DX_CommandQueueContext& GetCommandQueueContext();
			DX_DescriptorContext& GetDescriptorContext();
			// The CommandList and allocator will get collected every frame in DX_Device::EndFrameInternal (deferred release)
			DX_CommandList* AcquireCommandList(RHI::HardwareQueueClass hardwareQueueClass);

			DX_MemoryView CreateBufferCommitted(const RHI::BufferDescriptor& bufferDescriptor, D3D12_RESOURCE_STATES initialState, D3D12_HEAP_TYPE heapType);
		private:
			DX_Device() = default;
			REMOVE_COPY_AND_MOVE(DX_Device);

			// RHI
			RHI::ResultCode InitInternal(RHI::PhysicalDevice& physicalDevice) override;
			void ShutdownInternal() override;
			RHI::ResultCode BeginFrameInternal() override;
			void EndFrameInternal() override;
			RHI::ResultCode InitializeLimits() override;
			void WaitForIdleInternal() override;

			void EnableD3DDebugLayer();
			void EnableGPUBasedValidation();
			void EnableDebugDeviceFeatures();
			void EnableBreakOnD3DError();
		private:
			wrl::ComPtr<IDXGIFactoryX> m_dxgiFactory;
			wrl::ComPtr<IDXGIAdapterX> m_dxgiAdapter;
			wrl::ComPtr<ID3D12DeviceX> m_device;

			std::mutex deviceRemovedMtx;
			bool onDeviceRemoved = false;

			std::shared_ptr<DX_DescriptorContext> m_descriptorContext;
			DX_CommandListAllocator m_commandListAllocator;
			DX_CommandQueueContext m_commandQueueContext;
		};
	}
}