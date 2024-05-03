
// RHI
#include "ImagePool.h"

namespace CGE
{
	namespace RHI
	{
        ImageInitRequest::ImageInitRequest(Image& image, const ImageDescriptor& descriptor, const ClearValue* optimizedClearValue)
            : m_image{ &image }
            , m_descriptor{ descriptor }
            , m_optimizedClearValue{ optimizedClearValue } {}

        ResultCode ImagePool::Init(Device& device, const ImagePoolDescriptor& descriptor)
        {
            return ResourcePool::Init(device, descriptor,
                [this, &device, &descriptor]()
                {
                    m_descriptor = descriptor;
                    return InitInternal(device, descriptor);
                });
        }

        ResultCode ImagePool::InitImage(const ImageInitRequest& initRequest)
        {
            initRequest.m_image->SetDescriptor(initRequest.m_descriptor);
            return ResourcePool::InitResource(initRequest.m_image, [this, &initRequest]() { return InitImageInternal(initRequest); });
        }

        ResultCode ImagePool::UpdateImageContents(const ImageUpdateRequest& request)
        {
            if (!ValidateIsInitialized() || !ValidateNotProcessingFrame())
            {
                return ResultCode::InvalidOperation;
            }

            if (!ValidateIsRegistered(request.m_image))
            {
                return ResultCode::InvalidArgument;
            }

            if (!ValidateUpdateRequest(request))
            {
                return ResultCode::InvalidArgument;
            }

            return UpdateImageContentsInternal(request);
        }

        const ImagePoolDescriptor& ImagePool::GetDescriptor() const
        {
            return m_descriptor;
        }

        bool ImagePool::ValidateUpdateRequest(const ImageUpdateRequest& updateRequest) const
        {
            const ImageDescriptor& imageDescriptor = updateRequest.m_image->GetDescriptor();
            assert(!(updateRequest.m_imageSubresource.m_mipSlice >= imageDescriptor.m_mipLevels || updateRequest.m_imageSubresource.m_arraySlice >= imageDescriptor.m_arraySize));
            return true;
        }
	}
}