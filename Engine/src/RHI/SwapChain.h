#pragma once

// RHI
#include "RHI_Common.h"
#include "Device.h"
#include "SwapChainDescriptor.h"
#include "DeviceObject.h"

namespace CGE
{
	namespace RHI
	{
		class SwapChain : public DeviceObject
		{
		public:
			virtual ~SwapChain();
			ResultCode Init(RHI::Device& device, const SwapChainDescriptor& descriptor);
			
			void Present();

			// Sets the vertical sync interval for the swap chain.
			//      0 - No vsync.
			//      N - Sync to every N vertical refresh.
			// A value of 1 syncs to the refresh rate of the monitor.
			void SetVerticalSyncInterval(uint32_t verticalSyncInterval);
			ResultCode Resize(const SwapChainDimensions& dimensions);
			uint32_t GetImageCount() const;
			uint32_t GetCurrentImageIndex() const;
			const RHI::SwapChainDescriptor& GetDescriptor() const;

		protected:
			SwapChain();

		private:
			bool ValidateDescriptor(const SwapChainDescriptor& descriptor) const;

			virtual ResultCode InitInternal(RHI::Device& device, const SwapChainDescriptor& descriptor, SwapChainDimensions* nativeDimensions) = 0;
			virtual ResultCode ResizeInternal(const SwapChainDimensions& dimensions, SwapChainDimensions* nativeDimensions) = 0;
			virtual uint32_t PresentInternal() = 0;
			virtual void SetVerticalSyncIntervalInternal(uint32_t previousVerticalSyncInterval) = 0;

		private:
			SwapChainDescriptor m_descriptor;
			uint32_t m_currentImageIndex = 0;
		};
	}
}