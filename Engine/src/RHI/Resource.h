#pragma once

// RHI
#include "DeviceObject.h"

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
        class MemoryStatisticsBuilder;
        class ResourceView;
        class ImageView;
        class BufferView;
        struct ImageViewDescriptor;
        struct BufferViewDescriptor;


        // Base class for pooled resources (Image / Buffer).
        // You can retrieve a specific view for the resource and this can be used to initilize the platform specific descriptors for bindings.
        class Resource : public DeviceObject
        {
            friend class FrameAttachment;
            friend class ResourcePool;
        public:
            virtual ~Resource();

            void Shutdown() override final;
            const ResourcePool* GetPool() const;
            ResourcePool* GetPool();
            bool IsInResourceCache(const ImageViewDescriptor& imageViewDescriptor);
            bool IsInResourceCache(const BufferViewDescriptor& bufferViewDescriptor);
            void EraseResourceView(ResourceView* resourceView) const;

        protected:
            Resource() = default;

            Ptr<ImageView> GetResourceView(const ImageViewDescriptor& imageViewDescriptor) const;
            Ptr<BufferView> GetResourceView(const BufferViewDescriptor& bufferViewDescriptor) const; 

        private:
            void SetPool(ResourcePool* pool);

        private:
            ResourcePool* m_pool = nullptr;

            // Cache the resourceViews in order to avoid re-creation
            mutable std::unordered_map<size_t, ResourceView*> m_resourceViewCache;
            mutable std::mutex m_cacheMutex;
        };
    }
}