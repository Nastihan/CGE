#pragma once

// DX12
#include "DX_CommonHeaders.h"
#include "DX_DescriptorPool.h"

// RHI
#include "../RHI/SwapChain.h"
#include "../RHI/Device.h"

namespace CGE
{
	namespace DX12
	{
		class DX_Device;

		class DX_SwapChain : public RHI::SwapChain
		{
		public:
			static RHI::Ptr<DX_SwapChain> Create();
			DX_Device& GetDevice() const;

		private:
			DX_SwapChain() = default;
			RHI::ResultCode InitInternal(RHI::Device& device, const RHI::SwapChainDescriptor& descriptor, RHI::SwapChainDimensions* nativeDimensions) override;
			RHI::ResultCode ResizeInternal(const RHI::SwapChainDimensions& dimensions, RHI::SwapChainDimensions* nativeDimensions) override;
			uint32_t PresentInternal() override;
			void SetVerticalSyncIntervalInternal(uint32_t previousVerticalSyncInterval) override;

		private:
			wrl::ComPtr<IDXGISwapChainX> m_swapChain;
			bool m_isTearingSupported = false;

			// [todo] remove later
			DX_DescriptorPool m_swapchainRTVDescriptorPool;
		};
	}
}