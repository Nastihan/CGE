#pragma once

// RHI
#include "../RHI/RHI_Common.h"
#include "../RHI/BufferSystem.h"
#include "../RHI/BufferPool.h"
#include "../RHI/DrawItem.h"
#include "../RHI/InputStreamLayout.h"
#include "../RHI/BufferView.h"
#include "../RHI/ShaderResourceGroup.h"

// std
#include <optional>

#include <glm/glm.hpp>

struct aiMesh;

namespace CGE
{
	namespace Pass
	{
		class ForwardPass;
	}

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
			void SetIndexBufferAndView(RHI::Ptr<RHI::Buffer> indexBuffer, RHI::IndexBufferView& indexBufferView, uint32_t indexCount);
			RHI::DrawItem* BuildAndGetDrawItem();
			void SetSrgsToBind(const std::vector<RHI::ShaderResourceGroup*>& srgsToBind);
			const RHI::ShaderResourceGroup* const* GetSrgsToBind() const;
		
		private:
			std::vector<RHI::Ptr<RHI::Buffer>> m_vertexBuffers;
			std::vector<RHI::StreamBufferView> m_streamBufferViews;
			RHI::InputStreamLayout m_inputStreamLayoutPacked;
			
			RHI::Ptr<RHI::Buffer> m_triangleIndexBuffer;
			RHI::IndexBufferView m_triangleIndexBufferView;
			uint32_t m_indexCount;
			
			std::shared_ptr<Material> m_material;
			RHI::DrawItem m_drawItem;

			std::vector<RHI::ShaderResourceGroup*> m_srgsToBind;
		};
	}
}