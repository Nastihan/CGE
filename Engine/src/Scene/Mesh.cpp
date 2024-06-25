
// assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// Scene
#include "Mesh.h"
#include "Material.h"

// Pass
#include "../Pass/ForwardPass.h"

// RHI
#include "../RHI/Graphics.h"

namespace CGE
{
	namespace Scene
	{
		void Mesh::SetMaterial(std::shared_ptr<Material> material)
		{
			m_material = material;
		}

		std::shared_ptr<Material> Mesh::GetMaterial() const
		{
			return m_material;
		}

		void Mesh::AddVertexBuffer(RHI::Ptr<RHI::Buffer> vertexBuffer, RHI::StreamBufferView& streamBufferView)
		{
			m_vertexBuffers.push_back(vertexBuffer);
			m_streamBufferViews.push_back(streamBufferView);
		}

		void Mesh::SetInputStreamLayout(RHI::InputStreamLayout& inputStreamLayout)
		{
			m_inputStreamLayoutPacked = inputStreamLayout;
		}

		void Mesh::SetIndexBufferAndView(RHI::Ptr<RHI::Buffer> indexBuffer, RHI::IndexBufferView& indexBufferView, uint32_t indexCount)
		{
			m_triangleIndexBuffer = indexBuffer;
			m_triangleIndexBufferView = indexBufferView;
			m_indexCount = indexCount;
		}

		RHI::DrawItem* Mesh::BuildAndGetDrawItem()
		{
			// Set the vertex buffer
			m_drawItem.m_streamBufferViewCount = m_streamBufferViews.size();
			m_drawItem.m_streamBufferViews = m_streamBufferViews.data();
			
			if (m_triangleIndexBuffer)
			{
				m_drawItem.m_indexBufferView = &m_triangleIndexBufferView;
				RHI::DrawIndexed drawIndexedArgs;
				drawIndexedArgs.m_indexCount = m_indexCount;
				drawIndexedArgs.m_indexOffset = 0;
				drawIndexedArgs.m_instanceCount = 1;
				drawIndexedArgs.m_instanceOffset = 0;
				drawIndexedArgs.m_vertexOffset = 0;
				m_drawItem.m_arguments = { drawIndexedArgs };
			}

			return &m_drawItem;
		}

		void Mesh::SetSrgsToBind(const std::vector<RHI::ShaderResourceGroup*>& srgsToBind)
		{
			m_srgsToBind = srgsToBind;
		}

		const RHI::ShaderResourceGroup* const* Mesh::GetSrgsToBind() const
		{
			return m_srgsToBind.data();
		}
	}
}