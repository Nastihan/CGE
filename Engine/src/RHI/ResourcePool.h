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

        class ResourcePoolResolver
        {
        public:
            virtual ~ResourcePoolResolver() = default;
        };

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
            virtual void ShutdownInternal() = 0;
            virtual void ShutdownResourceInternal(Resource& resource) = 0;

            using PlatformMethod = std::function<RHI::ResultCode()>;
            ResultCode Init(Device& device, const ResourcePoolDescriptor& descriptor, const PlatformMethod& initMethod);
            ResultCode InitResource(Resource* resource, const PlatformMethod& initResourceMethod);
            bool ValidateIsRegistered(const Resource* resource) const;
            bool ValidateIsUnregistered(const Resource* resource) const;
            bool ValidateIsInitialized() const;
            bool ValidateNotProcessingFrame() const;

        protected:
            PoolMemoryUsage m_memoryUsage;

        private:
            void ShutdownResource(Resource* resource);
            void Register(Resource& resource);
            void Unregister(Resource& resource);

        private:
            mutable std::shared_mutex m_registryMutex;
            std::unordered_set<Resource*> m_registry;
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