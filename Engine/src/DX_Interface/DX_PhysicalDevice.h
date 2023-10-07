#pragma once
#include "DX_CommonHeaders.h"
#include "../RHI/PhysicalDevice.h"
#include <memory>

namespace CGE
{
	namespace DX12
	{
		class DX_PhysicalDevice final : public RHI::PhysicalDevice
		{
			using Base = RHI::PhysicalDevice;
			
		public:
			~DX_PhysicalDevice() = default;

			static std::shared_ptr<RHI::PhysicalDevice> EnumerateHighPerformanceDevice();
			// static RHI::PhysicalDeviceList Enumirate();

			IDXGIFactoryX* GetFactory() const;
			IDXGIAdapterX* GetAdapter() const;
		private:
			DX_PhysicalDevice() = default;

			void Init(IDXGIFactoryX* factory, IDXGIAdapterX* adapter);
			void Shutdown() override;
		private:
			std::shared_ptr<IDXGIFactoryX> m_dxgiFactory;
			std::shared_ptr<IDXGIAdapterX> m_dxgiAdapter;
		};
	}
}