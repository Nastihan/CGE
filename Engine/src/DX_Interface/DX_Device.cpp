#include "DX_Device.h"
#include <iostream>

using namespace Microsoft::WRL;

namespace CGE
{
	DX_Device::DX_Device()
	{
		InitDxgi();
		InitAdapter();
	}

	DX_Device::~DX_Device()
	{
		LOCAL_HR;
		#ifdef _DEBUG
		{
			{
				ComPtr<ID3D12InfoQueue> infoQueue;
				DX_THROW_FAILED(device->QueryInterface(IID_PPV_ARGS(&infoQueue)));
				infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, false);
				infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, false);
				infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, false);
			}

			// check if all com pointers created by device are released
			ComPtr<ID3D12DebugDevice2> debugDevice;
			DX_THROW_FAILED(device->QueryInterface(IID_PPV_ARGS(&debugDevice)));
			DX_THROW_FAILED(debugDevice->ReportLiveDeviceObjects(D3D12_RLDO_SUMMARY | D3D12_RLDO_DETAIL | D3D12_RLDO_IGNORE_INTERNAL));
		}
		#endif // _DEBUG
	}

	void DX_Device::InitDxgi()
	{
		LOCAL_HR;
		// init dgxi factory
		UINT dxgiFactoryFlags{ 0 };
		#ifdef _DEBUG
		{
			ComPtr<ID3D12Debug3> debugInterface;
			DX_THROW_FAILED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface)));
			debugInterface->EnableDebugLayer();
			// enable debug factory in debug build
			dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}
		#endif // _DEBUG
		DX_THROW_FAILED(CreateDXGIFactory2(dxgiFactoryFlags, __uuidof(IDXGIFactory7), (void**)&dxgiFactory));
		LOG_CONSOLE(LogLevel::Info, L"DXGI Created");
	}

	void DX_Device::InitAdapter()
	{
		LOCAL_HR;
		ComPtr<IDXGIAdapter4> mainAdapter;
		mainAdapter.Attach(FindAdapter());
		if (!mainAdapter) throw std::exception();
		// check if max feature level isnt lower than what we need
		D3D_FEATURE_LEVEL maxFeatureLevel{ GetAdaptersMaxFeatureLevel(mainAdapter.Get()) };
		assert(maxFeatureLevel >= MINIMUM_FEATURE_LEVEL);
		if (maxFeatureLevel < MINIMUM_FEATURE_LEVEL) throw std::exception();
		DX_THROW_FAILED(D3D12CreateDevice(mainAdapter.Get(), maxFeatureLevel, IID_PPV_ARGS(&device)));

		// check adapters name
		DXGI_ADAPTER_DESC1 desc;
		DX_THROW_FAILED(mainAdapter->GetDesc1(&desc));
		LOG_CONSOLE(LogLevel::Info, desc.Description);

		NAME_D3D12_OBJECT(device, L"Main D3D12 Device");
		LOG_CONSOLE(LogLevel::Info, L"D3D12 Device Created");

		#ifdef _DEBUG
		{
			ComPtr<ID3D12InfoQueue> infoQueue;
			DX_THROW_FAILED(device->QueryInterface(IID_PPV_ARGS(&infoQueue)));
			// these flags will cause a break in the application
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		}
		#endif // _DEBUG
	}

	// [todo] expand to check for more features we will need for the engine
	// get the first most performing adapter that supports the minimum feature level
	IDXGIAdapter4* DX_Device::FindAdapter()
	{
		IDXGIAdapter4* adapter{ nullptr };
		for (UINT i{ 0 }; dxgiFactory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter)) != DXGI_ERROR_NOT_FOUND; i++)
		{
			if (SUCCEEDED(D3D12CreateDevice(adapter, MINIMUM_FEATURE_LEVEL, __uuidof(ID3D12Device), nullptr)))
			{
				return adapter;
			}
			Release(adapter);
		}
		return nullptr;
	}

	// This function can be laster used if we have multiple adapters
	D3D_FEATURE_LEVEL DX_Device::GetAdaptersMaxFeatureLevel(IDXGIAdapter4* adapter)
	{
		LOCAL_HR;
		// feature levels we want to check
		// [todo] get this from an input of the function
		constexpr D3D_FEATURE_LEVEL featureLevels[4] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_12_0, D3D_FEATURE_LEVEL_12_1 };
		D3D12_FEATURE_DATA_FEATURE_LEVELS featureLevelInfo{};
		featureLevelInfo.NumFeatureLevels = _countof(featureLevels);
		featureLevelInfo.pFeatureLevelsRequested = featureLevels;

		ComPtr<ID3D12Device> device;
		DX_THROW_FAILED(D3D12CreateDevice(adapter, MINIMUM_FEATURE_LEVEL, IID_PPV_ARGS(&device)));
		DX_THROW_FAILED(device->CheckFeatureSupport(D3D12_FEATURE_FEATURE_LEVELS, &featureLevelInfo, sizeof(featureLevelInfo)));
		return featureLevelInfo.MaxSupportedFeatureLevel;
	}

	const DX_Device& DX_Device::GetInstance()
	{
		static DX_Device DXDeviceInstance;
		return DXDeviceInstance;
	}

	const wrl::ComPtr< ID3D12Device8>& DX_Device::GetDevice() const
	{
		return device;
	}

	const wrl::ComPtr< IDXGIFactory7>& DX_Device::GetDxgiFactory() const
	{
		return dxgiFactory;
	}
}