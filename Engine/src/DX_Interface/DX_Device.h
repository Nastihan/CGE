#pragma once

//DX12
#include "DX_CommonHeaders.h"
#include "DX_CommandListPool.h"
#include "DX_CommandQueueContext.h"

// RHI
#include "../RHI/RHI_Common.h"
#include "../RHI/Device.h"

// std
#include <vector>
#include <mutex>

namespace CGE
{
	namespace DX12
	{
		// [todo] might change from being a singleton
		class DX_Device final : public RHI::Device
		{
		public:
			static RHI::Ptr<RHI::Device> Create();
		private:
			// RHI
			RHI::ResultCode InitInternal(RHI::PhysicalDevice& physicalDevice) override;
			void ShutdownInternal() override;
			RHI::ResultCode BeginFrameInternal() override;
			void EndFrameInternal() override;
			RHI::ResultCode InitializeLimits() override;
		public:
			~DX_Device() = default;
		private:
			DX_Device() = default;
			REMOVE_COPY_AND_MOVE(DX_Device);
		public:
			ID3D12DeviceX* GetDevice() const;
			const wrl::ComPtr<IDXGIFactoryX>& GetDxgiFactory() const;
			std::string GetDeviceRemovedReason() const;
			void OnDeviceRemoved();
			bool DXAssertSuccess(HRESULT hr);
			DX_CommandQueueContext& GetCommandQueueContext();
		private:
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

			DX_CommandListSubAllocator m_commandListSubAllocator;
			DX_CommandQueueContext m_commandQueueContext;
		};
	}
}