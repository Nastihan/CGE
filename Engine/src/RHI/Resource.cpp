
// RHI
#include "Resource.h"
#include "TypeHash.h"
#include "ResourcePool.h"

namespace CGE
{
	namespace RHI
	{
        Resource::~Resource()
        {
            assert(GetPool() == nullptr);
        }

        bool Resource::IsAttachment() const
        {
            return m_frameAttachment != nullptr;
        }

        void Resource::Shutdown()
        {
            if (m_pool)
            {
                assert(m_frameAttachment == nullptr);
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

        const FrameAttachment* Resource::GetFrameAttachment() const
        {
            return m_frameAttachment;
        }

        void Resource::InvalidateViews()
        {
            if (!m_isInvalidationQueued)
            {
                m_isInvalidationQueued = true;
            }
        }

        /*
        bool Resource::IsInResourceCache(const ImageViewDescriptor& imageViewDescriptor)
        {
            const HashValue64 hash = imageViewDescriptor.GetHash();
            auto it = m_resourceViewCache.find(static_cast<uint64_t>(hash));
            return it != m_resourceViewCache.end();
        }

        bool Resource::IsInResourceCache(const BufferViewDescriptor& bufferViewDescriptor)
        {
            const HashValue64 hash = bufferViewDescriptor.GetHash();
            auto it = m_resourceViewCache.find(static_cast<uint64_t>(hash));
            return it != m_resourceViewCache.end();
        }

        void Resource::EraseResourceView(ResourceView* resourceView) const
        {
            std::lock_guard<std::mutex> registryLock(m_cacheMutex);
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

        Ptr<BufferView> Resource::GetResourceView(const BufferViewDescriptor& bufferViewDescriptor) const
        {
            const HashValue64 hash = bufferViewDescriptor.GetHash();
            std::lock_guard<std::mutex> registryLock(m_cacheMutex);
            auto it = m_resourceViewCache.find(static_cast<uint64_t>(hash));
            if (it == m_resourceViewCache.end())
            {
                Ptr<BufferView> bufferViewPtr = RHI::Factory::Get().CreateBufferView();
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
        */
        void Resource::SetPool(ResourcePool* pool)
        {
            m_pool = pool;
        }

        void Resource::SetFrameAttachment(FrameAttachment* frameAttachment)
        {
            m_frameAttachment = frameAttachment;
        }
	}
}