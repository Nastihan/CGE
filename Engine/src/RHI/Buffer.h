#pragma once

#include "Resource.h"
#include "BufferDescriptor.h"
#include "TypeHash.h"

namespace CGE
{
	namespace RHI
	{
        struct BufferViewDescriptor;

        // region of linear memory and used for rendering operations
        // manage the lifecycle of a buffer through a BufferPool
        class Buffer : public Resource
        {
            using Base = Resource;
            friend class BufferPool;

        public:
            virtual ~Buffer() = default;

            const BufferDescriptor& GetDescriptor() const;
            Ptr<BufferView> GetBufferView(const BufferViewDescriptor& bufferViewDescriptor);
            const HashValue64 GetHash() const;

        protected:
            Buffer() = default;
            void SetDescriptor(const BufferDescriptor& descriptor);

        private:
            BufferDescriptor m_descriptor;
        };
	}
}