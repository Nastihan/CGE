#pragma once
#include <vector>
#include <mutex>
#include "../RHI/RHI_Common.h"
#include "DX_CommonHeaders.h"
#include "../RHI/Device.h"

namespace CGE
{
	namespace DX12
	{
		// [todo] might change from being a singleton
		class DX_Device final : public RHI::Device
		{
			// new ===================================================================================
		public:
			static RHI::Ptr<RHI::Device> Create();
		private:
			RHI::ResultCode InitInternal(RHI::PhysicalDevice& physicalDevice) override;
			void ShutdownInternal() override;
			//========================================================================================
		public:
			~DX_Device() = default;
		private:
			DX_Device() = default;
			REMOVE_COPY_AND_MOVE(DX_Device);
		public:
			const wrl::ComPtr<ID3D12DeviceX>& GetDevice() const;
			const wrl::ComPtr<IDXGIFactoryX>& GetDxgiFactory() const;
			std::string GetDeviceRemovedReason() const;
			void OnDeviceRemoved();
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
		};
	}
}