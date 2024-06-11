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
        // To understand whats going on in the classes I'll give a short description in the input assembler stage of the pipeline:
        // 
        // We can bind up to 16 buffers to the input assembler stage and we need to fill an array of D3D12_INPUT_ELEMENT_DESC to describe all the elements in these buffer.
        // D3D12_INPUT_ELEMENT_DESC::SemanticName: Just a name corresponds to the shader semantics.
        // D3D12_INPUT_ELEMENT_DESC::SemanticIndex: A semantic index is only needed in a case where there is more than one element with the same semantic. 
        // For example, a 4x4 matrix would have four components each with the semantic name matrix, however each of the four component would have different semantic indices (0, 1, 2, and 3).
        // D3D12_INPUT_ELEMENT_DESC::Format: The format of that element.
        // D3D12_INPUT_ELEMENT_DESC::InputSlot: The buffer that this element belonges to (which is bound to one of the 16 slots the IA has)
        // D3D12_INPUT_ELEMENT_DESC::AlignedByteOffset: The offset to this element in the buffer.
        // D3D12_INPUT_ELEMENT_DESC::InputSlotClass: Is this data per-vertex or per-instance.
        // We have two options here, D3D11_INPUT_PER_VERTEX_DATA and D3D11_INPUT_PER_INSTANCE_DATA. These are pretty self explanatory. 
        // D3D11_INPUT_PER_VERTEX_DATA says the element is used "PER VERTEX", so that every vertex passed through the graphics pipeline gets it's own data from the input, 
        // while the D3D11_INPUT_PER_INSTANCE_DATA says that the element is used "PER INSTANCE", so that each instance of the geometry passed through the graphics pipeline gets it's own data.
        // D3D12_INPUT_ELEMENT_DESC::InstanceDataStepRate: The number of instances to draw using the same per-instance data before advancing in the buffer by one element.

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

        // Describes the rate at which an input assembly channel increments to the next element in the buffer stream.
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
            std::string m_name;
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

            // The parent stream buffer index. This will correspond to InputStreamLayout::m_streamBuffers index number.
            uint32_t m_bufferIndex = 0;

            // Byte offset from the base of the StreamBufferView to the first element in this channel. 
            // Used to interleave multiple channels in a single buffer. (D3D12_INPUT_ELEMENT_DESC::AlignedByteOffset)
            uint32_t m_byteOffset = 0;
        };

        // Describes an instance of a StreamBufferView within the stream layout. Each stream
        // buffer provides new data to the shader at a specified step rate. The byte stride
        // is the total width of a single element in the buffer stream. Set one for each vertex buffer.
        class StreamBufferDescriptor
        {
        public:
            StreamBufferDescriptor() = default;
            StreamBufferDescriptor(StreamStepFunction stepFunction, uint32_t stepRate, uint32_t byteStride);
            HashValue64 GetHash(HashValue64 seed = HashValue64{ 0 }) const;

            StreamStepFunction m_stepFunction = StreamStepFunction::PerVertex;
            // Only used if StreamStepFunction::PerInstance. The number of instances to process before moving to the next buffer entry. 
            uint32_t m_stepRate = 1;
            // The distance in bytes between consecutive vertex entries in the buffer. (should match stride value in StreamBufferView)
            uint32_t m_byteStride = 0;
        };

        // Describes the input assembly stream buffer layout for the pipeline state.
        // Use InputStreamLayoutBuilder to create this object.
        // We can bind 16 buffers to the input assembly stage and each buffer need a StreamChannelDescriptor
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

            // Multiple buffers can be bound to the input assembly stage.
            std::vector<StreamBufferDescriptor> m_streamBuffers;

            // Each buffer can have multiple attributes these have to be set.
            std::vector<StreamChannelDescriptor> m_streamChannels;
            HashValue64 m_hash = HashValue64{ 0 };
        };
	}
}