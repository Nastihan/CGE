#pragma once

// RHI
#include "ResourceView.h"
#include "BufferViewDescriptor.h"

namespace CGE
{
	namespace RHI
	{
        class Buffer;

        class BufferView : public ResourceView
        {
        public:
            virtual ~BufferView() = default;

            ResultCode Init(const Buffer& buffer, const BufferViewDescriptor& viewDescriptor);
            const BufferViewDescriptor& GetDescriptor() const;
            const Buffer& GetBuffer() const;
            bool IsFullView() const override final;
            HashValue64 GetHash() const;

        protected:
            HashValue64 m_hash = HashValue64{ 0 };

        private:
            bool ValidateForInit(const Buffer& buffer, const BufferViewDescriptor& viewDescriptor) const;

            /// The RHI descriptor for this view.
            BufferViewDescriptor m_descriptor;
        };
	}
}