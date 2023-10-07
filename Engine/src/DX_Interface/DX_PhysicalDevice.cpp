#include "DX_PhysicalDevice.h"

namespace CGE
{
	namespace DX12
	{
		std::shared_ptr<RHI::PhysicalDevice> DX_PhysicalDevice::EnumerateHighPerformanceDevice()
		{
			LOCAL_HR;
			DX_PhysicalDevice* ptr = new DX_PhysicalDevice();
			std::shared_ptr<DX_PhysicalDevice> physicalDevice = std::make_shared<DX_PhysicalDevice>(ptr);

			wrl::ComPtr<IDXGIFactoryX> dxgiFactory;
			DX_THROW_FAILED(CreateDXGIFactory2(0, __uuidof(IDXGIFactoryX), (void**)&dxgiFactory));

			wrl::ComPtr<IDXGIAdapterX> dxgiAdapter;
			for (uint32_t i{ 0 }; dxgiFactory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&dxgiAdapter)) != DXGI_ERROR_NOT_FOUND; i++)
			{
				physicalDevice->Init(dxgiFactory.Get(), dxgiAdapter.Get());
				break;
			}
			return physicalDevice;
		}
		void DX_PhysicalDevice::Init(IDXGIFactoryX* factory, IDXGIAdapterX* adapter)
		{
			m_dxgiFactory = std::make_shared<IDXGIFactoryX>(factory);
			m_dxgiAdapter = std::make_shared<IDXGIAdapterX>(adapter);

			DXGI_ADAPTER_DESC adapterDesc;
			adapter->GetDesc(&adapterDesc);


			std::wstring wDesc = adapterDesc.Description;
			std::string description(wDesc.begin(), wDesc.end());

			m_descriptor.m_cardName = description;
			//[todo] add driver version ...
		}
		void DX_PhysicalDevice::Shutdown()
		{
			m_dxgiAdapter = nullptr;
			m_dxgiFactory = nullptr;
		}
		IDXGIFactoryX* DX_PhysicalDevice::GetFactory() const
		{
			return m_dxgiFactory.get();
		}
		IDXGIAdapterX* DX_PhysicalDevice::GetAdapter() const
		{
			return m_dxgiAdapter.get();
		}
	}
}