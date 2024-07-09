#pragma once

#include "StreamBufferView.h"
#include "IndexBufferView.h"
#include "Viewport.h"
#include "Scissor.h"
#include "ShaderResourceGroup.h"

namespace CGE
{
	namespace RHI
	{
		class PipelineState;

		enum class DrawType : uint8_t
		{
			Indexed = 0,
			Linear,
			Indirect
		};

		// No index buffer
		struct DrawLinear
		{
			DrawLinear() = default;

			DrawLinear(
				uint32_t instanceCount,
				uint32_t instanceOffset,
				uint32_t vertexCount,
				uint32_t vertexOffset)
				: m_instanceCount(instanceCount)
				, m_instanceOffset(instanceOffset)
				, m_vertexCount(vertexCount)
				, m_vertexOffset(vertexOffset)
			{}

			uint32_t m_instanceCount = 1;
			uint32_t m_instanceOffset = 0;
			uint32_t m_vertexCount = 0;
			uint32_t m_vertexOffset = 0;
		};

		// Used for indexed draw call arguments.
		struct DrawIndexed
		{
			DrawIndexed() = default;

			DrawIndexed(
				uint32_t instanceCount,
				uint32_t instanceOffset,
				uint32_t vertexOffset,
				uint32_t indexCount,
				uint32_t indexOffset)
				: m_instanceCount(instanceCount)
				, m_instanceOffset(instanceOffset)
				, m_vertexOffset(vertexOffset)
				, m_indexCount(indexCount)
				, m_indexOffset(indexOffset)
			{}

			// Number in instances to draw
			uint32_t m_instanceCount = 1;
			// Value will be added to each index per instance.
			uint32_t m_instanceOffset = 0;
			// BaseVertexLocation is the first vertex to start indexing in.
			// You might have a big vertex buffer with multiple objects in it, and then have separate index buffers for each object. 
			// You will want to set this as the position in the vertex buffer of the first vertex used for this current object. 
			// An example is you have two quads stored in a vertex buffer, giving you 8 vertices in the vertex buffer. 
			// Quad1 uses vertices 0-3, while quad2 uses vertices 4-7. Maybe you have a single index buffer, which uses vertices 0-3. 
			// We want to draw Quad2 using this index buffer, so we will set this parameter to 4, so we add 4 to each index value, 
			// which draws vertices 4-7 from the vertex buffer.
			uint32_t m_vertexOffset = 0;
			// Number of indices to draw for each instance
			uint32_t m_indexCount = 0;
			// Offset in the index buffer to start drawing from (If we have multiple geometry data in one buffer)
			uint32_t m_indexOffset = 0;
		};

		struct DrawArguments
		{
			DrawArguments() : DrawArguments(DrawIndexed{}) {}
			DrawArguments(const DrawIndexed& indexed) : m_type{ DrawType::Indexed }, m_indexed{ indexed } {}
			DrawArguments(const DrawLinear& linear) : m_type{ DrawType::Linear }, m_linear{ linear } {}

			DrawType m_type;
			union
			{
				DrawIndexed m_indexed;
				DrawLinear m_linear;
			};
		};

		struct DrawItem
		{
			DrawArguments m_arguments;

			uint8_t m_stencilRef = 0;

			// Pipeline state pointer
			const PipelineState* m_pipelineState = nullptr;

			// Vertex buffer view
			// Pay very close mind to how you order and set these buffers both in the shader and draw item.
			// They should match or you'll be using the wrong values in the shader.
			uint8_t m_streamBufferViewCount = 0;
			const StreamBufferView* m_streamBufferViews = nullptr;
			
			// Index buffer view
			const IndexBufferView* m_indexBufferView = nullptr;

			uint8_t m_shaderResourceGroupCount = 0;
			const ShaderResourceGroup* const* m_shaderResourceGroups = nullptr;

			// List of viewports and scissors to be applied to this draw item only.
			// Will restore to the previous state after the DrawItem has been processed.
			// [todo] For now I will not use these.
			uint8_t m_scissorsCount = 0;
			uint8_t m_viewportsCount = 0;
			const Viewport* m_viewports = nullptr;
			const Scissor* m_scissors = nullptr;

			// Once the draw item gets submitted to the pass draw list, the pass adds the RenderAttachmentConfiguration which is the final touch
			// to build and set the m_pipelineState for this current draw item.
			// The ShaderPermutation will be set and add to the item from an EngineShader or MaterialShader File.
			// const ShaderPermutation& m_shaderPermutation;
		};
	}
}