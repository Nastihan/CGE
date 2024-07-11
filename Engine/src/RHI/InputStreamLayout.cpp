
// RHI
#include "InputStreamLayout.h"

namespace CGE
{
	namespace RHI
	{
		StreamChannelDescriptor::StreamChannelDescriptor(ShaderSemantic semantic, Format format, uint32_t byteOffset, uint32_t bufferIndex)
			: m_semantic{ std::move(semantic) }
			, m_format{ format }
			, m_byteOffset{ byteOffset }
			, m_bufferIndex{ bufferIndex } {}

		HashValue64 StreamChannelDescriptor::GetHash(HashValue64 seed) const
		{
			seed = TypeHash64(m_semantic, seed);
			seed = TypeHash64(m_format, seed);
			seed = TypeHash64(m_byteOffset, seed);
			seed = TypeHash64(m_bufferIndex, seed);
			return seed;
		}

		StreamBufferDescriptor::StreamBufferDescriptor(StreamStepFunction stepFunction, uint32_t stepRate, uint32_t byteStride)
			: m_stepFunction{ stepFunction }
			, m_stepRate{ stepRate }
			, m_byteStride{ byteStride } {}

		HashValue64 StreamBufferDescriptor::GetHash(HashValue64 seed) const
		{
			seed = TypeHash64(m_stepFunction, seed);
			seed = TypeHash64(m_stepRate, seed);
			seed = TypeHash64(m_byteStride, seed);
			return seed;
		}

		InputStreamLayout::InputStreamLayout()
		{
			m_streamChannels.reserve(RHI::Limits::Pipeline::StreamChannelCountMax);
			m_streamBuffers.reserve(RHI::Limits::Pipeline::StreamCountMax);
		}

		void InputStreamLayout::Clear()
		{
			m_topology = PrimitiveTopology::Undefined;
			m_streamChannels.clear();
			m_streamBuffers.clear();
			m_hash = HashValue64{ 0 };
		}

		bool InputStreamLayout::Finalize()
		{
			for (const auto& channelDescriptor : m_streamChannels)
			{
				assert(channelDescriptor.m_bufferIndex < m_streamBuffers.size(), "InputStreamLayout channel does not exist.");
				assert(m_topology != PrimitiveTopology::Undefined, "InputStreamLayout Topology is undefined.");
			}

			HashValue64 seed = TypeHash64(m_topology, HashValue64{ 0 });

			for (const StreamChannelDescriptor& channel : m_streamChannels)
			{
				seed = channel.GetHash(seed);
			}
			for (const StreamBufferDescriptor& buffer : m_streamBuffers)
			{
				seed = buffer.GetHash(seed);
			}
			m_hash = seed;

			return true;
		}

		bool InputStreamLayout::IsFinalized() const
		{
			return m_hash != HashValue64{ 0 };
		}

		void InputStreamLayout::SetTopology(PrimitiveTopology topology)
		{
			m_topology = topology;
		}

		void InputStreamLayout::AddStreamChannel(const StreamChannelDescriptor& descriptor)
		{
			m_streamChannels.push_back(descriptor);
		}

		void InputStreamLayout::AddStreamBuffer(const StreamBufferDescriptor& descriptor)
		{
			m_streamBuffers.push_back(descriptor);
		}

		const PrimitiveTopology InputStreamLayout::GetTopology() const
		{
			return m_topology;
		}

		std::span<const StreamChannelDescriptor> InputStreamLayout::GetStreamChannels() const
		{
			return m_streamChannels;
		}

		std::span<const StreamBufferDescriptor> InputStreamLayout::GetStreamBuffers() const
		{
			return m_streamBuffers;
		}

		HashValue64 InputStreamLayout::GetHash() const
		{
			return m_hash;
		}

		bool InputStreamLayout::operator==(const InputStreamLayout& rhs) const
		{
			bool same = (m_streamChannels.size() == rhs.m_streamChannels.size() && m_streamBuffers.size() == rhs.m_streamBuffers.size());
			if (same)
			{
				for (size_t index = 0; index < m_streamChannels.size(); index++)
				{
					if (m_streamChannels[index].GetHash() != rhs.m_streamChannels[index].GetHash())
					{
						same = false;
						break;
					}
				}

				if (same)
				{
					for (size_t index = 0; index < m_streamBuffers.size(); index++)
					{
						if (m_streamBuffers[index].GetHash() != rhs.m_streamBuffers[index].GetHash())
						{
							same = false;
							break;
						}
					}
				}
			}
			return same && m_hash == rhs.m_hash && m_topology == rhs.m_topology;
		}
	}
}