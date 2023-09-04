#pragma once
#include "DX_CommonHeaders.h"

namespace CGE
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
		const wrl::ComPtr< ID3D12Device8>& GetDevice() const;
		const wrl::ComPtr< IDXGIFactory7>& GetDxgiFactory() const;
	private:
		void InitDxgi();
		void InitAdapter();
		IDXGIAdapter4* FindAdapter();
		D3D_FEATURE_LEVEL GetAdaptersMaxFeatureLevel(IDXGIAdapter4* adapter);
	private:
		wrl::ComPtr< ID3D12Device8> device;
		wrl::ComPtr< IDXGIFactory7> dxgiFactory;
		static constexpr D3D_FEATURE_LEVEL MINIMUM_FEATURE_LEVEL{ D3D_FEATURE_LEVEL_11_0 };
	};
}