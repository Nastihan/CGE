#pragma once
#include "../ConsoleLog.h"
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
		DX_Device(const DX_Device&) = delete;
		DX_Device(DX_Device&&) = delete;
		DX_Device& operator=(const DX_Device&) = delete;
		DX_Device& operator=(DX_Device&&) = delete;
	public:
		ID3D12Device8* GetDevice() const;
		const IDXGIFactory7* GetDxgiFactory() const;
	private:
		void InitDxgi();
		void InitAdapter();
		IDXGIAdapter4* FindAdapter();
		D3D_FEATURE_LEVEL GetAdaptersMaxFeatureLevel(IDXGIAdapter4* adapter);
	private:
		static DX_Device DXDeviceInstance;
		ID3D12Device8* device{ nullptr };
		IDXGIFactory7* dxgiFactory{ nullptr };
		static constexpr D3D_FEATURE_LEVEL MINIMUM_FEATURE_LEVEL{ D3D_FEATURE_LEVEL_11_0 };
	};
}