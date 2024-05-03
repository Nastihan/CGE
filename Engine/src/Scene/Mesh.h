#pragma once

// RHI
#include "../RHI/RHI_Common.h"
#include "../RHI/BufferSystem.h"
#include "../RHI/BufferPool.h"
#include "../RHI/DrawItem.h"
#include "../RHI/InputStreamLayout.h"
#include "../RHI/BufferView.h"

// std
#include <optional>

#include <glm/glm.hpp>

struct aiMesh;

namespace CGE
{
	namespace Scene
	{
		class Material;

		class Mesh
		{
		public:
			Mesh() = default;

			void SetMaterial(std::shared_ptr<Material> material);
			std::shared_ptr<Material> GetMaterial() const;

			void AddVertexBuffer(RHI::Ptr<RHI::Buffer> vertexBuffer, RHI::StreamBufferView& streamBufferView);
			void SetInputStreamLayout(RHI::InputStreamLayout& inputStreamLayout);
			void SetIndexBufferAndView(RHI::Ptr<RHI::Buffer> indexBuffer, RHI::IndexBufferView& indexBufferView);

			void BuildDrawItem();
		private:
			// Used to the meshesh tranformation matrix (constant buffer)
			glm::mat4 m_transform;
			RHI::Ptr<RHI::Buffer> m_transformCbuff;
			RHI::Ptr<RHI::BufferView> m_transformCbuffView;

			std::vector<RHI::Ptr<RHI::Buffer>> m_vertexBuffers;
			std::vector<RHI::StreamBufferView> m_streamBufferViews;
			RHI::InputStreamLayout m_inputStreamLayoutPacked;
			RHI::Ptr<RHI::Buffer> m_triangleIndexBuffer;
			RHI::IndexBufferView m_triangleIndexBufferView;
			std::shared_ptr<Material> m_material;

		};
	}
}