
// RHI
#include "ResourceView.h"

namespace CGE
{
	namespace RHI
	{
		const Resource& ResourceView::GetResource() const
		{
			return *m_resource;
		}

		ResultCode ResourceView::Init(const Resource& resource)
		{
			RHI::Device& device = resource.GetDevice();

			m_resource = &resource;
			ResultCode resultCode = InitInternal(device, resource);
			if (resultCode != ResultCode::Success)
			{
				m_resource = nullptr;
				return resultCode;
			}

			DeviceObject::Init(device);
			return ResultCode::Success;
		}

		void ResourceView::Shutdown()
		{
			if (IsInitialized())
			{
				ShutdownInternal();

				m_resource->EraseResourceView(this);
				m_resource = nullptr;
				DeviceObject::Shutdown();
			}
		}
	}
}