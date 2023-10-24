#include "DX_PhysicalDevice.h"

namespace CGE
{
	namespace DX12
	{
		RHI::Ptr<RHI::PhysicalDevice> DX_PhysicalDevice::Create()
		{
			LOCAL_HR;
			RHI::Ptr<PhysicalDevice> physicalDevice;
			RHI::Ptr<DX_PhysicalDevice> dxPhysicalDevice = new DX_PhysicalDevice();

			wrl::ComPtr<IDXGIFactoryX> dxgiFactory;
			DX_THROW_FAILED(CreateDXGIFactory2(0, __uuidof(IDXGIFactoryX), (void**)&dxgiFactory));

			wrl::ComPtr<IDXGIAdapterX> dxgiAdapter;
			for (uint32_t i{ 0 }; dxgiFactory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&dxgiAdapter)) != DXGI_ERROR_NOT_FOUND; i++)
			{
				dxPhysicalDevice->Init(dxgiFactory.Get(), dxgiAdapter.Get());
				break;
			}
			physicalDevice = dxPhysicalDevice;
			return physicalDevice;
		}
		void DX_PhysicalDevice::Init(IDXGIFactoryX* factory, IDXGIAdapterX* adapter)
		{
			m_dxgiFactory = factory;
			m_dxgiAdapter = adapter;

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