#pragma once

// RHI
#include "Resource.h"

namespace CGE
{
	namespace RHI
	{
        class Resource;

        // ResourceView is a base class for views which are dependent on a Resource instance.
        class ResourceView : public DeviceObject
        {
        public:
            friend class Resource;

            virtual ~ResourceView() = default;

            const Resource& GetResource() const;

            // Returns whether the view covers the entire image (i.e. isn't just a subset).
            virtual bool IsFullView() const = 0;

        protected:
            ResultCode Init(const Resource& resource);

        private:
            // RHI::Object
            void Shutdown() final;


            virtual ResultCode InitInternal(Device& device, const Resource& resource) = 0;
            virtual void ShutdownInternal() = 0;
            virtual ResultCode InvalidateInternal() = 0;

        private:
            //This is a smart pointer to make sure a Resource is not destroyed before all the views are destroyed first.
            ConstPtr<Resource> m_resource = nullptr;
        };
	}
}