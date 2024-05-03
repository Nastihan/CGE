#pragma once

// RHI
#include "Resource.h"

namespace CGE
{
	namespace RHI
	{
        class Resource;

        // Base class for resource views.
        class ResourceView : public DeviceObject
        {
        public:
            friend class Resource;

            virtual ~ResourceView() = default;
            const Resource& GetResource() const;

            // This method will be called if we either manullay want to delete a view from the resources view cache
            // Or the resource is freed from the pool which all views will be released.
            ResultCode InvalidateResourceView();

            // Returns whether the view covers the entire image. (not just a sub-region)
            virtual bool IsFullView() const = 0;

            // This method will check if the view is still valid for the resource.
            bool IsStale() const;

        protected:
            ResultCode Init(const Resource& resource);

        private:
            // RHI::Object
            void Shutdown() override final;

            // Init the view using the context to the descriptor heap.
            virtual ResultCode InitInternal(Device& device, const Resource& resource) = 0;

            // The underlying platform implementation should release the descriptor from the descriptor heap in this overload.
            virtual void ShutdownInternal() = 0;
            virtual ResultCode InvalidateInternal() = 0;

        private:
            // Each new view will store a smart pointer reference to the resource to increase its refcount
            // This is a smart pointer to make sure a resource is not destroyed before all the views are destroyed first.
            ConstPtr<Resource> m_resource = nullptr;

            // This will be initilized with the version of the resource at creation time.
            // The version of the resource changes each time the backing resource is changed by the pool.
            // We can check this to see if the resource view is still valid for the resource.
            uint32_t m_version = 0;
        };
	}
}