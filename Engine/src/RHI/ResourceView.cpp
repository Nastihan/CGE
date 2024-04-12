
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

		ResultCode ResourceView::InvalidateResourceView()
		{
			ResultCode resultCode = InvalidateInternal();
			if (resultCode == ResultCode::Success)
			{
				// Set the new version to the new resources version.
				// This version will get increment evey time the underlying resource gets reinitialized.
				m_version = m_resource->GetVersion();
			}
			return resultCode;
		}

		bool ResourceView::IsStale() const
		{
			// The resource is not valid if the underying resource has been reinitialized.
			return m_resource && m_resource->GetVersion() != m_version;
		}
	}
}