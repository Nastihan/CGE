
// RHI
#include "Resource.h"
#include "TypeHash.h"
#include "ResourcePool.h"
#include "BufferViewDescriptor.h"
#include "BufferView.h"
#include "Buffer.h"
#include "Graphics.h"

namespace CGE
{
	namespace RHI
	{
        Resource::~Resource()
        {
            assert(GetPool() == nullptr);
        }

        void Resource::Shutdown()
        {
            if (m_pool)
            {
                m_pool->ShutdownResource(this);
            }
            DeviceObject::Shutdown();
        }

        const ResourcePool* Resource::GetPool() const
        {
            return m_pool;
        }

        ResourcePool* Resource::GetPool()
        {
            return m_pool;
        }

        void Resource::EraseResourceView(ResourceView* resourceView) const
        {
            auto itr = m_resourceViewCache.begin();
            while (itr != m_resourceViewCache.end())
            {
                if (itr->second == resourceView)
                {
                    m_resourceViewCache.erase(itr->first);
                    break;
                }
                itr++;
            }
        }

        /*
        Ptr<ImageView> Resource::GetResourceView(const ImageViewDescriptor& imageViewDescriptor) const
        {
            const HashValue64 hash = imageViewDescriptor.GetHash();
            std::lock_guard<std::mutex> registryLock(m_cacheMutex);
            auto it = m_resourceViewCache.find(static_cast<uint64_t>(hash));
            if (it == m_resourceViewCache.end())
            {
                Ptr<ImageView> imageViewPtr = RHI::Factory::Get().CreateImageView();
                RHI::ResultCode resultCode = imageViewPtr->Init(static_cast<const Image&>(*this), imageViewDescriptor);
                if (resultCode == RHI::ResultCode::Success)
                {
                    m_resourceViewCache[static_cast<uint64_t>(hash)] = static_cast<ResourceView*>(imageViewPtr.get());
                    return imageViewPtr;
                }
                else
                {
                    return nullptr;
                }
            }
            else
            {
                return static_cast<ImageView*>(it->second);
            }
        }
        */

        Ptr<BufferView> Resource::GetResourceView(const BufferViewDescriptor& bufferViewDescriptor) const
        {
            const HashValue64 hash = bufferViewDescriptor.GetHash();
            auto it = m_resourceViewCache.find(static_cast<uint64_t>(hash));
            if (it == m_resourceViewCache.end())
            {
                Ptr<BufferView> bufferViewPtr = RHI::Graphics::GetFactory().CreateBufferView();
                RHI::ResultCode resultCode = bufferViewPtr->Init(static_cast<const Buffer&>(*this), bufferViewDescriptor);
                if (resultCode == RHI::ResultCode::Success)
                {
                    m_resourceViewCache[static_cast<uint64_t>(hash)] = static_cast<ResourceView*>(bufferViewPtr.get());
                    return bufferViewPtr;
                }
                else
                {
                    return nullptr;
                }
            }
            else
            {
                return static_cast<BufferView*>(it->second);
            }
        }

        void Resource::SetPool(ResourcePool* pool)
        {
            m_pool = pool;
        }
	}
}