
// RHI
#include "ResourcePool.h"
#include "Resource.h"

namespace CGE
{
	namespace RHI
	{
		ResourcePool::~ResourcePool()
		{
			assert(m_registry.empty());
		}

		void ResourcePool::Shutdown()
		{
			assert(!ValidateNotProcessingFrame());

			if (IsInitialized())
			{
				for (Resource* resource : m_registry)
				{
					resource->SetPool(nullptr);
					ShutdownResourceInternal(*resource);
					resource->Shutdown();
				}
				m_registry.clear();
				m_resolver.reset();
				ShutdownInternal();
				DeviceObject::Shutdown();
			}
		}

		ResourcePoolResolver* ResourcePool::GetResolver()
		{
			return m_resolver.get();
		}

		const ResourcePoolResolver* ResourcePool::GetResolver() const
		{
			return m_resolver.get();
		}

		const HeapMemoryUsage& ResourcePool::GetHeapMemoryUsage(HeapMemoryLevel heapMemoryLevel) const
		{
			return m_memoryUsage.GetHeapMemoryUsage(heapMemoryLevel);
		}

		const PoolMemoryUsage& ResourcePool::GetMemoryUsage() const
		{
			return m_memoryUsage;
		}

		void ResourcePool::SetResolver(std::unique_ptr<ResourcePoolResolver>&& resolvePolicy)
		{
			m_resolver = std::move(resolvePolicy);
		}

		uint32_t ResourcePool::GetResourceCount() const
		{
			std::shared_lock<std::shared_mutex> lock(m_registryMutex);
			return static_cast<uint32_t>(m_registry.size());
		}

		ResultCode ResourcePool::Init(Device& device, const ResourcePoolDescriptor& descriptor, const PlatformMethod& initMethod)
		{
			for (size_t heapMemoryLevel = 0; heapMemoryLevel < 2; ++heapMemoryLevel)
			{
				m_memoryUsage.m_memoryUsagePerLevel[heapMemoryLevel].m_budgetInBytes = descriptor.m_budgetInBytes;
			}

			ResultCode resultCode = initMethod();
			if (resultCode == ResultCode::Success)
			{
				DeviceObject::Init(device);
			}
			return resultCode;
		}

		ResultCode ResourcePool::InitResource(Resource* resource, const PlatformMethod& initResourceMethod)
		{
			if (!ValidateIsInitialized())
			{
				return ResultCode::InvalidOperation;
			}

			if (!ValidateIsUnregistered(resource))
			{
				return ResultCode::InvalidArgument;
			}

			const ResultCode resultCode = initResourceMethod();
			if (resultCode == ResultCode::Success)
			{
				resource->Init(GetDevice());
				Register(*resource);
			}
			return resultCode;
		}

		bool ResourcePool::ValidateIsRegistered(const Resource* resource) const
		{
			return resource && (resource->GetPool() == this);
		}

		bool ResourcePool::ValidateIsUnregistered(const Resource* resource) const
		{
			return !resource && (resource->GetPool() != this);
		}

		bool ResourcePool::ValidateIsInitialized() const
		{
			return IsInitialized();
		}

		bool ResourcePool::ValidateNotProcessingFrame() const
		{
			return m_isProcessingFrame;
		}

		void ResourcePool::ShutdownResource(Resource* resource)
		{
			if (ValidateIsInitialized() && ValidateIsRegistered(resource))
			{
				Unregister(*resource);
				ShutdownResourceInternal(*resource);
			}
		}

		void ResourcePool::Register(Resource& resource)
		{
			resource.SetPool(this);
			std::unique_lock<std::shared_mutex> lock(m_registryMutex);
			m_registry.emplace(&resource);
		}

		void ResourcePool::Unregister(Resource& resource)
		{
			resource.SetPool(nullptr);
			std::unique_lock<std::shared_mutex> lock(m_registryMutex);
			m_registry.erase(&resource);
		}
	}
}