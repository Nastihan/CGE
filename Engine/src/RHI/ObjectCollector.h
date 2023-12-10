#pragma once

#include "Object.h"
#include "intrusive_ptr.h"

#include <mutex>
#include <vector>
#include <functional>

namespace CGE
{
    namespace RHI
    {
        struct NullMutex
        {
            inline void lock() {}
            inline bool try_lock() { return true; }
            inline void unlock() {}
        };

        struct ObjectCollectorTraits
        {
            /// The type of object serviced by the object collector. This must either derive from Object
            /// or share its interface.
            using ObjectType = Object;

            /// The mutex used to guard the object collector data.
            using MutexType = NullMutex;
        };

        using ObjectCollectorNotifyFunction = std::function<void()>;

        //! Deferred-releases reference-counted objects at a specific latency. Example: Use to batch-release
        //! objects that exist on the GPU timeline at the end of the frame after syncing the oldest GPU frame.
        template <typename Traits = ObjectCollectorTraits>
        class ObjectCollector
        {
        public:
            using ObjectType = typename Traits::ObjectType;
            using ObjectPtrType = Ptr<ObjectType>;

            ObjectCollector() = default;
            ~ObjectCollector();

            //! This method is called when the collection pass is releasing an object. The user
            //! can provide their own "collector" function to do something other than release
            //! (e.g. reuse from a pool).
            using CollectFunction = std::function<void(ObjectType&)>;

            struct Descriptor
            {
                /// Number of collect calls before an object is collected.
                size_t m_collectLatency = 0;

                /// The collector function called when an object is collected.
                CollectFunction m_collectFunction = nullptr;
            };

            void Init(const Descriptor& descriptor);
            void Shutdown();

            //! Queues a single pointer for collection.
            void QueueForCollect(ObjectPtrType object);

            //! Queues an array of objects for collection.
            void QueueForCollect(ObjectType* objects, size_t objectCount);

            //! Queues an array of pointers to the object for collection.
            void QueueForCollect(ObjectType** objects, size_t objectCount);

            //! Runs a collection cycle. All objects scheduled for collection (according
            //! to the collection latency) are provided to the collect function (if it exists),
            //! and the references are released.
            void Collect(bool forceFlush = false);

            //! Returns the number of objects pending to be collected.
            //! Must not be called at collection time.
            size_t GetObjectCount() const;

            //! Notifies after the current set of pending objects is released.
            void Notify(ObjectCollectorNotifyFunction notifyFunction);

        private:
            void QueueForCollectInternal(ObjectPtrType object);

            struct Garbage
            {
                std::vector<ObjectPtrType> m_objects;
                uint64_t m_collectIteration;
                std::vector<ObjectCollectorNotifyFunction> m_notifies;
            };

            inline bool IsGarbageReady(size_t collectIteration)
            {
                return m_currentIteration - collectIteration >= m_descriptor.m_collectLatency;
            }

            Descriptor m_descriptor;

            size_t m_currentIteration = 0;

            mutable typename Traits::MutexType m_mutex;
            std::vector<ObjectPtrType> m_pendingObjects;
            std::vector<Garbage> m_pendingGarbage;
            std::vector<ObjectCollectorNotifyFunction> m_pendingNotifies;
        };

        template <typename Traits>
        ObjectCollector<Traits>::~ObjectCollector()
        {
            assert(m_pendingGarbage.empty(), "There is garbage that wasn't collected");
        }

        template <typename Traits>
        void ObjectCollector<Traits>::Init(const Descriptor& descriptor)
        {
            m_descriptor = descriptor;
        }

        template <typename Traits>
        void ObjectCollector<Traits>::Shutdown()
        {
            Collect(true);
        }

        template <typename Traits>
        void ObjectCollector<Traits>::QueueForCollect(ObjectPtrType object)
        {
            m_mutex.lock();
            QueueForCollectInternal(std::move(object));
            m_mutex.unlock();
        }

        template <typename Traits>
        void ObjectCollector<Traits>::QueueForCollect(ObjectType* objects, size_t objectCount)
        {
            m_mutex.lock();
            for (size_t i = 0; i < objectCount; ++i)
            {
                QueueForCollectInternal(&objects[i]);
            }
            m_mutex.unlock();
        }

        template <typename Traits>
        void ObjectCollector<Traits>::QueueForCollect(ObjectType** objects, size_t objectCount)
        {
            m_mutex.lock();
            for (size_t i = 0; i < objectCount; ++i)
            {
                QueueForCollectInternal(static_cast<ObjectType*>(objects[i]));
            }
            m_mutex.unlock();
        }

        template <typename Traits>
        void ObjectCollector<Traits>::QueueForCollectInternal(ObjectPtrType object)
        {
            assert(object, "Queued a null object");
            m_pendingObjects.emplace_back(std::move(object));
        }

        template <typename Traits>
        void ObjectCollector<Traits>::Collect(bool forceFlush)
        {
            m_mutex.lock();
            if (m_pendingObjects.size())
            {
                m_pendingGarbage.push_back({ std::move(m_pendingObjects), m_currentIteration });
            }

            if (!m_pendingNotifies.empty())
            {
                if (!m_pendingGarbage.empty())
                {
                    // find the newest garbage entry and add any pending notifies
                    Garbage& latestGarbage = m_pendingGarbage.front();
                    size_t latestGarbageAge = m_currentIteration - latestGarbage.m_collectIteration;

                    // check the rest of the entries to see if they are newer
                    for (size_t i = 1; i < m_pendingGarbage.size(); ++i)
                    {
                        size_t age = m_currentIteration - m_pendingGarbage[i].m_collectIteration;
                        if (age < latestGarbageAge)
                        {
                            latestGarbage = m_pendingGarbage[i];
                            latestGarbageAge = age;
                        }
                    }

                    latestGarbage.m_notifies.insert(latestGarbage.m_notifies.end(), m_pendingNotifies.begin(), m_pendingNotifies.end());
                }
                else
                {
                    // garbage queue is empty, notify now
                    for (auto& notifyFunction : m_pendingNotifies)
                    {
                        notifyFunction();
                    }
                }

                m_pendingNotifies.clear();
            }
            m_mutex.unlock();

            size_t objectCount = 0;
            size_t i = 0;
            while (i < m_pendingGarbage.size())
            {
                Garbage& garbage = m_pendingGarbage[i];
                if (IsGarbageReady(garbage.m_collectIteration) || forceFlush)
                {
                    if (m_descriptor.m_collectFunction)
                    {
                        for (ObjectPtrType& object : garbage.m_objects)
                        {
                            m_descriptor.m_collectFunction(*object);
                        }
                    }
                    objectCount += garbage.m_objects.size();

                    for (auto& notifyFunction : garbage.m_notifies)
                    {
                        notifyFunction();
                    }

                    garbage = std::move(m_pendingGarbage.back());
                    m_pendingGarbage.pop_back();
                }
                else
                {
                    ++i;
                }
            }
            m_currentIteration++;
        }

        template <typename Traits>
        size_t ObjectCollector<Traits>::GetObjectCount() const
        {
            size_t objectCount = 0;
            m_mutex.lock();
            objectCount += m_pendingObjects.size();
            m_mutex.unlock();

            for (const Garbage& garbage : m_pendingGarbage)
            {
                objectCount += garbage.m_objects.size();
            }

            return objectCount;
        }

        template <typename Traits>
        void ObjectCollector<Traits>::Notify(ObjectCollectorNotifyFunction notifyFunction)
        {
            m_mutex.lock();
            m_pendingNotifies.push_back(notifyFunction);
            m_mutex.unlock();
        }
    }
}