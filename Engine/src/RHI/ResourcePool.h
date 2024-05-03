#pragma once

// RHI
#include "DeviceObject.h"
#include "MemoryUsage.h"
#include "MemoryEnums.h"
#include "ResourcePoolDescriptor.h"

// std
#include <shared_mutex>
#include <unordered_set>

namespace CGE
{
	namespace RHI
	{
        class CommandList;
        class Resource;

        // Resource Pool Resolvers will perform resolve operations on the pool.
        // This class will queue resolve operations so we dont do any operations on the resources while the gpu is using them.
        class ResourcePoolResolver
        {
        public:
            virtual ~ResourcePoolResolver() = default;
        };

        // Resource pools will manage the backing memory and allocations for Buffers and Images.
        // For buffers a page allocator and suballocator will be used.
        // Images are simpler and we will just create commited resources.
        // For transient attachments we will use a dedicated heap which the size will be calculated after we compile the frame graph.
        // This pool will use the Placed Resource api. (Check TransientAttachmentPool)
        // Also for big texures check StreamingImagePool this will use the update tile mappings api.
        class ResourcePool : public DeviceObject
        {
            friend class Resource;
        public:
            virtual ~ResourcePool();

            void Shutdown() override final;

            template <typename ResourceType>
            void ForEach(std::function<void(ResourceType&)> callback);
            template <typename ResourceType>
            void ForEach(std::function<void(const ResourceType&)> callback) const;

            uint32_t GetResourceCount() const;
            ResourcePoolResolver* GetResolver();
            const ResourcePoolResolver* GetResolver() const;
            virtual const ResourcePoolDescriptor& GetDescriptor() const = 0;
            const HeapMemoryUsage& GetHeapMemoryUsage(HeapMemoryLevel heapMemoryLevel) const;
            const PoolMemoryUsage& GetMemoryUsage() const;

        protected:
            ResourcePool() = default;
            void SetResolver(std::unique_ptr<ResourcePoolResolver>&& resolvePolicy);
            virtual void ShutdownInternal();
            virtual void ShutdownResourceInternal(Resource& resource);

            using PlatformMethod = std::function<RHI::ResultCode()>;
            ResultCode Init(Device& device, const ResourcePoolDescriptor& descriptor, const PlatformMethod& initMethod);
            ResultCode InitResource(Resource* resource, const PlatformMethod& initResourceMethod);
            bool ValidateIsRegistered(const Resource* resource) const;
            bool ValidateIsUnregistered(const Resource* resource) const;
            bool ValidateIsInitialized() const;
            bool ValidateNotProcessingFrame() const;

        protected:
            PoolMemoryUsage m_memoryUsage;
            virtual void OnFrameBegin();

        private:
            void ShutdownResource(Resource* resource);
            void Register(Resource& resource);
            void Unregister(Resource& resource);

            void OnFrameCompile();
            void OnFrameEnd();

        private:
            mutable std::shared_mutex m_registryMutex;
            std::unordered_set<Resource*> m_registry;

            // Drived class will assign this.
            std::unique_ptr<ResourcePoolResolver> m_resolver;
            std::atomic_bool m_isProcessingFrame = { false };
        };

        template <typename ResourceType>
        void ResourcePool::ForEach(std::function<void(ResourceType&)> callback)
        {
            std::shared_lock<std::shared_mutex> lock(m_registryMutex);
            for (Resource* resourceBase : m_registry)
            {
                ResourceType* resourceType = dynamic_cast<ResourceType*>(resourceBase);
                if (resourceType)
                {
                    callback(*resourceType);
                }
            }
        }

        template <typename ResourceType>
        void ResourcePool::ForEach(std::function<void(const ResourceType&)> callback) const
        {
            std::shared_lock<std::shared_mutex> lock(m_registryMutex);
            for (const Resource* resourceBase : m_registry)
            {
                const ResourceType* resourceType = dynamic_cast<const ResourceType*>(resourceBase);
                if (resourceType)
                {
                    callback(*resourceType);
                }
            }
        }
	}
}