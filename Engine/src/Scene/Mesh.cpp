
// assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// Scene
#include "Mesh.h"
#include "Material.h"

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

		void Mesh::SetIndexBufferAndView(RHI::Ptr<RHI::Buffer> indexBuffer, RHI::IndexBufferView& indexBufferView)
		{
			m_triangleIndexBuffer = indexBuffer;
			m_triangleIndexBufferView = indexBufferView;
		}
	}
}