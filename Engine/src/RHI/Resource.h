#pragma once

// RHI
#include "DeviceObject.h"
#include "TypeHash.h"

// std
#include <unordered_set>
#include <unordered_map>
#include <mutex>


namespace CGE
{
    namespace RHI
    {
        class ResourcePool;
        class FrameAttachment;
        class ResourceView;
        class ImageView;
        class BufferView;
        struct ImageViewDescriptor;
        struct BufferViewDescriptor;


        // Base class for pooled resources (Image / Buffer).
        // The backing memory for resources is associated at initialization time on a specific pool.
        class Resource : public DeviceObject
        {
            friend class ResourcePool;
        public:
            virtual ~Resource();

            // Shutdown will happen by detaching it from the parent pool.
            // This way the allocators used for the pool will be updated and the space can be used.
            void Shutdown() override final;
            const ResourcePool* GetPool() const;
            ResourcePool* GetPool();
            void EraseResourceView(ResourceView* resourceView) const;
            uint32_t GetVersion() const;
            void InvalidateViews();

        protected:
            Resource() = default;

            Ptr<ImageView> GetResourceView(const ImageViewDescriptor& imageViewDescriptor) const;
            Ptr<BufferView> GetResourceView(const BufferViewDescriptor& bufferViewDescriptor) const;

        private:
            void SetPool(ResourcePool* pool);
            bool IsFirstVersion() const;

        private:
            ResourcePool* m_pool = nullptr;
            mutable std::unordered_map<size_t, ResourceView*> m_resourceViewCache;

            // The version is monotonically incremented any time new platform memory is assigned to the resource by a backing pool.
            // This happens when SetPool function is called. This finction will be called by the parent pool at initialization time.
            uint32_t m_version = 0;
        };
    }
}