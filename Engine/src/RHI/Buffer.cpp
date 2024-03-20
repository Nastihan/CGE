
// RHI
#include "Buffer.h"
#include "BufferView.h"

namespace CGE
{
	namespace RHI
	{
		const BufferDescriptor& Buffer::GetDescriptor() const
		{
			return m_descriptor;
		}

		Ptr<BufferView> Buffer::GetBufferView(const BufferViewDescriptor& bufferViewDescriptor)
		{
			return Base::GetResourceView(bufferViewDescriptor);
		}

		const HashValue64 Buffer::GetHash() const
		{
			HashValue64 hash = HashValue64{ 0 };
			hash = m_descriptor.GetHash();
			return hash;
		}

		void Buffer::SetDescriptor(const BufferDescriptor& descriptor)
		{
			m_descriptor = descriptor;
		}
	}
}