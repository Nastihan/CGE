#pragma once

// Scene
#include "Material.h"

// RHI
#include "../RHI/RHI_Common.h"
#include "../RHI/BufferSystem.h"
#include "../RHI/BufferPool.h"
#include "../RHI/DrawItem.h"
#include "../RHI/InputStreamLayout.h"
#include "../RHI/BufferView.h"

#include <optional>

#include <DirectXMath.h>

struct aiMesh;

namespace CGE
{
	namespace Scene
	{
		class Material;

		class Mesh
		{
		public:
			Mesh(const aiMesh& mesh, const std::optional<aiMaterial> material, const std::string& pathString);

			void BuildDrawItem();
		private:
			// The primitive topology will be triangles for now.
			struct MeshBuffers
			{
				struct VertexInterleaved
				{
					RHI::Math::Vector3 m_pos;
					RHI::Math::Vector2 m_uv;
					// RHI::Math::Vector3 m_color;
					RHI::Math::Vector3 m_normal;
					RHI::Math::Vector3 m_tangent;
					RHI::Math::Vector3 m_bitangent;
				};

				// Individual Stream Buffers
				RHI::Ptr<RHI::Buffer> m_positionBuffer;
				RHI::Ptr<RHI::Buffer> m_uvBuffer;
				// RHI::Ptr<RHI::Buffer> m_colorBuffer;
				RHI::Ptr<RHI::Buffer> m_normalBuffer;
				RHI::Ptr<RHI::Buffer> m_tangentBuffer;
				RHI::Ptr<RHI::Buffer> m_bitangentBuffer;
				std::array<RHI::StreamBufferView, 6> m_individualStreamBufferViews;
				RHI::InputStreamLayout m_indivisualInputStreamLayout;

				RHI::Ptr<RHI::Buffer> m_triangleIndexBuffer;
				RHI::IndexBufferView m_triangleIndexBufferView;

				// Interleaved format
				std::vector<VertexInterleaved> m_interleavedVertexData;
				RHI::Ptr<RHI::Buffer> m_interleavedBuffer;
				RHI::StreamBufferView m_interleavedStreamBufferView;
				RHI::InputStreamLayout m_interleavedInputStreamLayout;
			};
		private:
			// Used to the meshesh tranformation matrix (constant buffer)
			RHI::Math::Matrix44 m_transform;
			RHI::Ptr<RHI::Buffer> m_transformCbuff;
			RHI::Ptr<RHI::BufferView> m_transformCbuffView;

			MeshBuffers m_meshBuffers;
			std::unique_ptr<Material> m_material;

			RHI::DrawItem m_drawItem;
		};
	}
}