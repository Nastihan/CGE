#pragma once
#include "DX_CommonHeaders.h"
#include "../RHI/PhysicalDevice.h"

namespace CGE
{
	namespace DX12
	{
		class DX_PhysicalDevice final : public RHI::PhysicalDevice
		{
			using Base = RHI::PhysicalDevice;
			
		public:
			static RHI::Ptr<RHI::PhysicalDevice> Create();

		public:
			~DX_PhysicalDevice() = default;

			IDXGIFactoryX* GetFactory() const;
			IDXGIAdapterX* GetAdapter() const;
		private:
			DX_PhysicalDevice() = default;

			void Init(IDXGIFactoryX* factory, IDXGIAdapterX* adapter);
			void Shutdown() override;
		private:
			RHI::Ptr<IDXGIFactoryX> m_dxgiFactory;
			RHI::Ptr<IDXGIAdapterX> m_dxgiAdapter;
		};
	}
}