#pragma once

// RHI
#include "ResourceView.h"
#include "BufferViewDescriptor.h"

namespace CGE
{
	namespace RHI
	{
        class Buffer;

        // A buffer view will map to a linear sub-region of a buffer resource.
        class BufferView : public ResourceView
        {
        public:
            virtual ~BufferView() = default;

            ResultCode Init(const Buffer& buffer, const BufferViewDescriptor& viewDescriptor);
            const BufferViewDescriptor& GetDescriptor() const;
            const Buffer& GetBuffer() const;
            bool IsFullView() const override final;
            HashValue64 GetHash() const;

            static constexpr uint32_t InvalidBindlessIndex = 0xFFFFFFFF;
            virtual uint32_t GetBindlessReadIndex() const { return InvalidBindlessIndex; }
            virtual uint32_t GetBindlessReadWriteIndex() const { return InvalidBindlessIndex; }

        protected:
            HashValue64 m_hash = HashValue64{ 0 };

        private:
            bool ValidateForInit(const Buffer& buffer, const BufferViewDescriptor& viewDescriptor) const;

            /// The RHI descriptor for this view. This will contain the information needed for platform specific views.
            BufferViewDescriptor m_descriptor;
        };
	}
}