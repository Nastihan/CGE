
// DX12
#include "DX_ImagePool.h"
#include "DX_Device.h"
#include "DX_Image.h"

namespace CGE
{
	namespace DX12
	{
        RHI::Ptr<DX_ImagePool> DX_ImagePool::Create()
        {
            return new DX_ImagePool();
        }

        DX_Device& DX_ImagePool::GetDevice() const
        {
            return static_cast<DX_Device&>(Base::GetDevice());
        }

        DX_ImagePoolResolver* DX_ImagePool::GetResolver()
        {
            return static_cast<DX_ImagePoolResolver*>(Base::GetResolver());
        }

        RHI::ResultCode DX_ImagePool::InitInternal(RHI::Device& device, const RHI::ImagePoolDescriptor&)
        {
            SetResolver(std::make_unique<DX_ImagePoolResolver>(static_cast<DX_Device&>(device), this));
            return RHI::ResultCode::Success;
        }

        RHI::ResultCode DX_ImagePool::InitImageInternal(const RHI::ImageInitRequest& request)
        {
            DX_Device& device = GetDevice();
            DX_Image* image = static_cast<DX_Image*>(request.m_image);

            D3D12_RESOURCE_ALLOCATION_INFO allocationInfo;
            device.GetImageAllocationInfo(request.m_descriptor, allocationInfo);

            RHI::HeapMemoryUsage& memoryUsage = m_memoryUsage.GetHeapMemoryUsage(RHI::HeapMemoryLevel::Device);
            if (!memoryUsage.CanAllocate(allocationInfo.SizeInBytes))
            {
                return RHI::ResultCode::OutOfMemory;
            }

            // Just creates a committed resource for the image.
            DX_MemoryView memoryView = device.CreateImageCommitted(request.m_descriptor, request.m_optimizedClearValue, image->GetInitialResourceState(), D3D12_HEAP_TYPE_DEFAULT);
            if (memoryView.IsValid())
            {
                image->m_residentSizeInBytes = memoryView.GetSize();
                image->m_memoryView = std::move(memoryView);
                image->GenerateSubresourceLayouts();
                image->m_memoryView.SetName(image->GetName());
                image->m_streamedMipLevel = image->GetResidentMipLevel();

                memoryUsage.m_totalResidentInBytes += allocationInfo.SizeInBytes;
                memoryUsage.m_usedResidentInBytes += allocationInfo.SizeInBytes;
                return RHI::ResultCode::Success;
            }
            else
            {
                return RHI::ResultCode::OutOfMemory;
            }
        }

        RHI::ResultCode DX_ImagePool::UpdateImageContentsInternal(const RHI::ImageUpdateRequest& request)
        {
            size_t bytesTransferred = 0;
            RHI::ResultCode resultCode = GetResolver()->UpdateImage(request, bytesTransferred);
            return resultCode;
        }

        void DX_ImagePool::ShutdownResourceInternal(RHI::Resource& resourceBase)
        {
            if (auto* resolver = GetResolver())
            {
                resolver->OnResourceShutdown(resourceBase);
            }

            DX_Image& image = static_cast<DX_Image&>(resourceBase);
            RHI::HeapMemoryUsage& memoryUsage = m_memoryUsage.GetHeapMemoryUsage(RHI::HeapMemoryLevel::Device);
            memoryUsage.m_totalResidentInBytes -= image.m_residentSizeInBytes;
            memoryUsage.m_usedResidentInBytes -= image.m_residentSizeInBytes;

            GetDevice().QueueForRelease(image.m_memoryView);
            image.m_memoryView = {};
            image.m_pendingResolves = 0;
        }
	}
}