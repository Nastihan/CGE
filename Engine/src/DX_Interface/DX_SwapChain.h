#pragma once

// DX12
#include "DX_CommonHeaders.h"
#include "DX_DescriptorPool.h"

// RHI
#include "../RHI/SwapChain.h"
#include "../RHI/Device.h"
#include "../RHI/Limits.h"

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

			ID3D12Resource* GetBackBuffer();
			DX_DescriptorHandle& GetBackBufferDescriptorHandle();

		private:
			DX_SwapChain() = default;
			RHI::ResultCode InitInternal(RHI::Device& device, const RHI::SwapChainDescriptor& descriptor, RHI::SwapChainDimensions* nativeDimensions) override;
			RHI::ResultCode ResizeInternal(const RHI::SwapChainDimensions& dimensions, RHI::SwapChainDimensions* nativeDimensions) override;
			uint32_t PresentInternal() override;
			void SetVerticalSyncIntervalInternal(uint32_t previousVerticalSyncInterval) override;
			RHI::ResultCode InitImagesInternal() override;

		private:
			wrl::ComPtr<IDXGISwapChainX> m_swapChain;
			bool m_isTearingSupported = false;

			// [todo] Replace with proper image view later
			wrl::ComPtr<ID3D12Resource> m_backBuffers[RHI::Limits::Device::FrameCountMax];
			DX_DescriptorHandle m_swapChainDescriptorHandles[RHI::Limits::Device::FrameCountMax];
		};
	}
}