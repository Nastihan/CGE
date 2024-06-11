
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

		void Mesh::BuildSrg(Pass::ForwardPass* pForwardPass)
		{
			const RHI::ShaderResourceGroupLayout* objectSrgLayout = pForwardPass->GetPipelineStateDescriptorForDraw().m_pipelineLayoutDescriptor->GetShaderResourceGroupLayout(1);
			auto& rhiFactory = RHI::Graphics::GetFactory();
			m_objectSrg = rhiFactory.CreateShaderResourceGroup();
			RHI::ShaderResourceGroupData objectSrgData(objectSrgLayout);
			
			objectSrgData.SetConstantPointer(reinterpret_cast<uint8_t*>(pForwardPass->GetPerObjectDataPtr()));
			RHI::ShaderInputImageIndex materialTexturesInputIdx(0);
			for (size_t arrayIdx = 0; arrayIdx < 8; arrayIdx++)
			{
				auto viewPtr = m_material->GetTextureAndView(static_cast<Material::TextureType>(arrayIdx)).second;
				if (viewPtr)
				{
					objectSrgData.SetImageView(materialTexturesInputIdx, viewPtr.get(), arrayIdx);
				}
			}
			RHI::ShaderInputBufferIndex materialCbuffInputIdx(0);
			objectSrgData.SetBufferView(materialCbuffInputIdx, m_material->GetMaterialCbuffView().get(), 0);
			m_objectSrg->Init(m_triangleIndexBufferView.GetBuffer()->GetDevice(), objectSrgData);
			m_objectSrg->Compile();
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

		void Mesh::Render(Pass::ForwardPass* pForwardPass, RHI::CommandList* commandList)
		{
			pForwardPass->PushSrg(m_objectSrg.get());
			BuildAndGetDrawItem();
			m_drawItem.m_pipelineState = &pForwardPass->GetPipelineState();
			m_drawItem.m_shaderResourceGroupCount = pForwardPass->GetSrgsToBind().size();
			m_drawItem.m_shaderResourceGroups = pForwardPass->GetSrgsToBind().data();
			commandList->Submit(m_drawItem);
			pForwardPass->PopSrg();
		}
	}
}