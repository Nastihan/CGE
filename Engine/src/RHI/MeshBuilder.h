#pragma once

// RHI
#include "RHI_Common.h"
#include "BufferPool.h"

namespace CGE
{
	namespace RHI
	{
		class MeshBuilder final
		{
		public:
			MeshBuilder() = default;
			~MeshBuilder() = default;

			bool Init();
		private:
			struct Mesh
			{
				RHI::Ptr<Buffer> m_vertexBuffer;
				RHI::Ptr<Buffer> m_normalBuffer;
				RHI::Ptr<Buffer> m_indexBuffer;
			};
		private:
			RHI::Ptr<BufferPool> m_meshBuilderBufferPool;
		};
	}
}