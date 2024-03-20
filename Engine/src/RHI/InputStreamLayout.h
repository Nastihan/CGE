#pragma once

// RHI
#include "RHI_Common.h"
#include "Format.h"
#include "TypeHash.h"
#include "Limits.h"

// std
#include <span>
#include <vector>

namespace CGE
{
	namespace RHI
	{
        // Describes the primitive topology of an input assembly stream.
        enum class PrimitiveTopology : uint32_t
        {
            Undefined = 0,
            PointList,
            LineList,
            LineListAdj,
            LineStrip,
            LineStripAdj,
            TriangleList,
            TriangleListAdj,
            TriangleStrip,
            TriangleStripAdj,
            PatchList
        };

        enum class StreamStepFunction : uint32_t
        {
            Constant = 0,
            PerVertex,
            PerInstance,
            PerPatch,
            PerPatchControlPoint
        };

        // Semantics declared in hlsl
        struct ShaderSemantic
        {
            std::wstring m_name;
            uint32_t m_index;
        };

        // Used to configure D3D12_INPUT_ELEMENT_DESC
        // One for each attribute in a single vertex buffer
        class StreamChannelDescriptor
        {
        public:
            StreamChannelDescriptor() = default;
            StreamChannelDescriptor(ShaderSemantic semantic, Format format, uint32_t byteOffset, uint32_t bufferIndex);
            HashValue64 GetHash(HashValue64 seed = HashValue64{ 0 }) const;

            ShaderSemantic m_semantic;
            Format m_format = Format::Unknown;

            // The parent stream buffer index. (used for D3D12_INPUT_ELEMENT_DESC::InputSlot)
            uint32_t m_bufferIndex = 0;

            // Byte offset from the base of the StreamBufferView to the first element in this channel. 
            // Used to interleave multiple channels in a single buffer. (D3D12_INPUT_ELEMENT_DESC::AlignedByteOffset)
            uint32_t m_byteOffset = 0;
        };

        // Describes an instance of a StreamBufferView within the stream layout. Each stream
        // buffer provides new data to the shader at a specified step rate. The byte stride
        // is the total width of a single element in the buffer stream.
        // One for each vertex buffer
        class StreamBufferDescriptor
        {
        public:
            StreamBufferDescriptor() = default;
            StreamBufferDescriptor(StreamStepFunction stepFunction, uint32_t stepRate, uint32_t byteStride);
            HashValue64 GetHash(HashValue64 seed = HashValue64{ 0 }) const;

            // D3D12_INPUT_ELEMENT_DESC::InputSlotClass
            StreamStepFunction m_stepFunction = StreamStepFunction::PerVertex;
            // D3D12_INPUT_ELEMENT_DESC::InstanceDataStepRate
            uint32_t m_stepRate = 1;
            // The distance in bytes between consecutive vertex entries in the buffer. (should match stride value in StreamBufferView)
            uint32_t m_byteStride = 0;
        };

        class InputStreamLayout
        {
        public:
            InputStreamLayout();
            bool operator==(const InputStreamLayout& rhs) const;

            void Clear();
            bool Finalize();
            bool IsFinalized() const;

            void SetTopology(PrimitiveTopology topology);
            void AddStreamChannel(const StreamChannelDescriptor& descriptor);
            void AddStreamBuffer(const StreamBufferDescriptor& descriptor);

            const PrimitiveTopology GetTopology() const;
            std::span<const StreamChannelDescriptor> GetStreamChannels() const;
            std::span<const StreamBufferDescriptor> GetStreamBuffers() const;
            HashValue64 GetHash() const;

        private:
            PrimitiveTopology m_topology = PrimitiveTopology::Undefined;
            std::vector<StreamChannelDescriptor> m_streamChannels;
            std::vector<StreamBufferDescriptor> m_streamBuffers;
            HashValue64 m_hash = HashValue64{ 0 };
        };
	}
}