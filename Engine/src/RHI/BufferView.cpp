
// RHI
#include "BufferView.h"
#include "Buffer.h"

namespace CGE
{
	namespace RHI
	{
        ResultCode BufferView::Init(const Buffer& buffer, const BufferViewDescriptor& viewDescriptor)
        {
            if (!ValidateForInit(buffer, viewDescriptor))
            {
                return ResultCode::InvalidOperation;
            }
            if (buffer.GetDescriptor().m_byteCount < (viewDescriptor.m_elementOffset + viewDescriptor.m_elementCount) * viewDescriptor.m_elementSize)
            {
                return ResultCode::OutOfMemory;
            }

            m_descriptor = viewDescriptor;
            m_hash = buffer.GetHash();
            m_hash = TypeHash64(m_descriptor.GetHash(), m_hash);
            return ResourceView::Init(buffer);
        }

        const BufferViewDescriptor& BufferView::GetDescriptor() const
        {
            return m_descriptor;
        }

        const Buffer& BufferView::GetBuffer() const
        {
            return static_cast<const Buffer&>(GetResource());
        }

        bool BufferView::IsFullView() const
        {
            const BufferDescriptor& bufferDescriptor = GetBuffer().GetDescriptor();
            const uint32_t bufferViewSize = m_descriptor.m_elementCount * m_descriptor.m_elementSize;
            return m_descriptor.m_elementOffset == 0 && bufferViewSize >= bufferDescriptor.m_byteCount;
        }

        HashValue64 BufferView::GetHash() const
        {
            return m_hash;
        }

        bool BufferView::ValidateForInit(const Buffer& buffer, const BufferViewDescriptor& viewDescriptor) const
        {
            return !IsInitialized() && buffer.IsInitialized() && 
                CheckBitsAll(static_cast<uint32_t>(buffer.GetDescriptor().m_bindFlags), static_cast<uint32_t>(viewDescriptor.m_overrideBindFlags));
        }
	}
}