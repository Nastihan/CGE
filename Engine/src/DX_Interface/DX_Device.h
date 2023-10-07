#pragma once
#include <vector>
#include <mutex>
#include "DX_CommonHeaders.h"

namespace CGE
{
	namespace DX12
	{
		// [todo] might change from being a singleton
		class DX_Device
		{
		public:
			static const DX_Device& GetInstance();
			~DX_Device();
		private:
			DX_Device();
			REMOVE_COPY_AND_MOVE(DX_Device);
		public:
			const wrl::ComPtr<ID3D12DeviceX>& GetDevice() const;
			const wrl::ComPtr<IDXGIFactoryX>& GetDxgiFactory() const;
			std::string GetDeviceRemovedReason() const;
			void OnDeviceRemoved();
		private:
			void InitDxgi();
			void InitAdapter();
			IDXGIAdapter4* FindAdapter();
			D3D_FEATURE_LEVEL GetAdaptersMaxFeatureLevel(IDXGIAdapter4* adapter);
			void EnableBreakOnD3DError();
			void EnableD3DDebugLayer();
		private:
			wrl::ComPtr<IDXGIFactoryX> dxgiFactory;
			wrl::ComPtr<IDXGIAdapterX> dxgiAdapter;
			wrl::ComPtr<ID3D12DeviceX> device;
			static constexpr D3D_FEATURE_LEVEL MINIMUM_FEATURE_LEVEL{ D3D_FEATURE_LEVEL_11_0 };

			std::mutex deviceRemovedMtx;
			bool onDeviceRemoved = false;
		};
	}
}