// RHI
#include "SwapChain.h"

namespace CGE
{
	namespace RHI
	{
		SwapChain::SwapChain() {}
		SwapChain::~SwapChain() {}

        ResultCode SwapChain::InitImages()
        {
            return InitImagesInternal();
        }

		bool SwapChain::ValidateDescriptor(const SwapChainDescriptor& descriptor) const
		{
			const bool isValidDescriptor = descriptor.m_dimensions.m_imageWidth != 0 && descriptor.m_dimensions.m_imageHeight != 0 && descriptor.m_dimensions.m_imageCount != 0;
			return isValidDescriptor;
		}

        ResultCode SwapChain::Init(RHI::Device& device, const SwapChainDescriptor& descriptor)
        {
            DeviceObject::Init(device);

            if (!ValidateDescriptor(descriptor))
            {
                return ResultCode::InvalidArgument;
            }

            SetName("SwapChain");
            SwapChainDimensions nativeDimensions = descriptor.m_dimensions;

            ResultCode resultCode = InitInternal(device, descriptor, &nativeDimensions);

            if (resultCode == ResultCode::Success)
            {
                m_descriptor = descriptor;
                // Overwrite descriptor dimensions with the native ones (the ones assigned by the platform) returned by InitInternal.
                m_descriptor.m_dimensions = nativeDimensions;
                
                resultCode = InitImages();
            }

            device.SetSwapChain(this);

            return resultCode;
        }

        void SwapChain::Present()
        {
            m_currentImageIndex = PresentInternal();
        }

        void SwapChain::SetVerticalSyncInterval(uint32_t verticalSyncInterval)
        {
            uint32_t previousVsyncInterval = m_descriptor.m_verticalSyncInterval;
            m_descriptor.m_verticalSyncInterval = verticalSyncInterval;
            SetVerticalSyncIntervalInternal(previousVsyncInterval);
        }

        ResultCode SwapChain::Resize(const SwapChainDimensions& dimensions)
        {
            SwapChainDimensions nativeDimensions = dimensions;
            ResultCode resultCode = ResizeInternal(dimensions, &nativeDimensions);
            if (resultCode == ResultCode::Success)
            {
                m_descriptor.m_dimensions = nativeDimensions;
            }

            return resultCode;
        }

        const RHI::SwapChainDescriptor& SwapChain::GetDescriptor() const
        {
            return m_descriptor;
        }

        uint32_t SwapChain::GetCurrentImageIndex() const
        {
            return m_currentImageIndex;
        }
	}
}