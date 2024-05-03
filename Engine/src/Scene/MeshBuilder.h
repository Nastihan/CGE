#pragma once

// RHI
#include "../RHI/RHI_Common.h"
#include "../RHI/BufferSystem.h"
#include "../RHI/BufferPool.h"
#include "../RHI/DrawItem.h"

namespace CGE
{
	class MeshBuilder final
	{
	private:
		struct Mesh
		{
			RHI::Ptr<RHI::Buffer> m_vertexBuffer;

			RHI::Ptr<RHI::Buffer> m_normalBuffer;
			RHI::Ptr<RHI::Buffer> m_indexBuffer;
			RHI::Ptr<RHI::Buffer> m_transformCbuff;

			RHI::DrawItem m_drawItem;
		};
	public:
		MeshBuilder(RHI::BufferSystem& bufferSystem);
		~MeshBuilder() = default;

		Mesh LoadObjModel(std::string filePath);
	private:
		RHI::BufferSystem& m_bufferSystem;
		RHI::Ptr<RHI::BufferPool> m_systemConstantBufferPool;
		RHI::Ptr<RHI::BufferPool> m_systemStaticInputAssemblyBufferPool;
	};
}