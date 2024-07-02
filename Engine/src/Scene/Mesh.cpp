
// assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// Scene
#include "Mesh.h"
#include "Material.h"
#include "ModelNode.h"

// Pass
#include "../Pass/ForwardPass.h"

// RHI
#include "../RHI/Graphics.h"

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"

#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

namespace CGE
{
	namespace Scene
	{
		Mesh::Mesh(const std::string& name) 
			: m_name{ name }
			, m_localTranslation(0.0f, 0.0f, 0.0f)
			, m_localRotation(glm::quat(glm::vec3(glm::radians(0.0f), glm::radians(0.0f), glm::radians(0.0f))))
		{
			m_perObjectData = (PerObjectData*)_aligned_malloc(sizeof(PerObjectData), 16);
		}

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

		void Mesh::SpawnImGuiWindow()
		{
			const auto updateChange = [this](bool check) {m_dirty = check || m_dirty; };
			if (ImGui::TreeNode(m_name.c_str()))
			{
				// Transformation
				if (ImGui::TreeNode("Transformation"))
				{
					ImGui::Text("Position");
					updateChange(ImGui::SliderFloat("Pos X", &m_localTranslation.x, -80.0f, 80.0f, "%.1f"));
					updateChange(ImGui::SliderFloat("Pos Y", &m_localTranslation.y, -80.0f, 80.0f, "%.1f"));
					updateChange(ImGui::SliderFloat("Pos Z", &m_localTranslation.z, -80.0f, 80.0f, "%.1f"));

					ImGui::Text("Orientation");
					updateChange(ImGui::SliderFloat("Roll", &m_currentRotation.z, 0.995f * -180.0f, 0.995f * 180.0f));
					updateChange(ImGui::SliderFloat("Pitch", &m_currentRotation.x, 0.995f * -180.0f, 0.995f * 180.0f));
					updateChange(ImGui::SliderFloat("Yaw", &m_currentRotation.y, 0.995f * -180.0f, 0.995f * 180.0f));
					ImGui::TreePop();
				}

				// Material
				if (ImGui::TreeNode("Material Properties"))
				{
					m_material->SpawnImGuiWindow();
					ImGui::TreePop();
				}

				ImGui::TreePop();
			}
		}

		const std::string& Mesh::GetName()
		{
			return m_name;
		}

		void Mesh::SetModelNode(const std::shared_ptr<ModelNode> node)
		{
			m_modelNode = node;

			glm::mat4 translateMatrix = glm::translate(glm::mat4{ 1.0 }, m_localTranslation);
			glm::mat4 rotationMatrix = glm::toMat4(m_localRotation);
			glm::mat4 localTransformation = translateMatrix * rotationMatrix;

			m_perObjectData->m_modelTransform = m_modelNode.lock()->GetWorldTransfom() * localTransformation;

			const auto& constantBufferPool = RHI::Graphics::GetBufferSystem().GetCommonBufferPool(RHI::CommonBufferPoolType::Constant);
			auto& rhiFactory = RHI::Graphics::GetFactory();
			m_modelTransformCbuff = rhiFactory.CreateBuffer();
			RHI::ResultCode result = RHI::ResultCode::Fail;

			RHI::BufferInitRequest modelCbufferRequest;
			modelCbufferRequest.m_buffer = m_modelTransformCbuff.get();
			modelCbufferRequest.m_descriptor.m_byteCount = sizeof(PerObjectData);
			modelCbufferRequest.m_descriptor.m_bindFlags = RHI::BufferBindFlags::Constant;
			modelCbufferRequest.m_initialData = m_perObjectData;
			result = constantBufferPool->InitBuffer(modelCbufferRequest);
			assert(result == RHI::ResultCode::Success);

			RHI::BufferViewDescriptor modelBufferViewDescriptor = RHI::BufferViewDescriptor::CreateRaw(0, sizeof(PerObjectData));
			m_modelTransformBufferView = rhiFactory.CreateBufferView();
			m_modelTransformBufferView->Init(*m_modelTransformCbuff, modelBufferViewDescriptor);

			const RHI::ShaderPermutation& defaultPBRForward_MaterialShader = *RHI::Graphics::GetAssetProcessor().GetShaderPermutation("DefaultPBRForward_MaterialShader");
			const RHI::ShaderResourceGroupLayout* objectSrgLayout = defaultPBRForward_MaterialShader.m_pipelineLayoutDescriptor->GetShaderResourceGroupLayout(RHI::ShaderResourceGroupType::Object);
			m_objectSrg = rhiFactory.CreateShaderResourceGroup();
			RHI::ShaderResourceGroupData objectSrgData(objectSrgLayout);

			RHI::ShaderInputBufferIndex modelTransformBufferIdx = objectSrgLayout->FindShaderInputBufferIndex("PerObject_Model");
			objectSrgData.SetBufferView(modelTransformBufferIdx, m_modelTransformBufferView.get(), 0);
			m_objectSrg->Init(m_modelTransformCbuff->GetDevice(), objectSrgData);
			m_objectSrg->Compile();
			assert(result == RHI::ResultCode::Success);
		}

		RHI::ResultCode Mesh::Update()
		{
			if (m_dirty)
			{
				glm::vec3 delta = m_previousRotation - m_currentRotation;
				if (delta.x > glm::epsilon<float>() || delta.x < glm::epsilon<float>())
				{
					m_localRotation = glm::angleAxis(glm::radians(delta.x), m_localRotation * glm::vec3(1, 0, 0)) * m_localRotation;
				}
				if (delta.y > glm::epsilon<float>() || delta.y < glm::epsilon<float>())
				{
					m_localRotation = glm::angleAxis(glm::radians(delta.y), m_localRotation * glm::vec3(0, 1, 0)) * m_localRotation;
				}
				if (delta.z > glm::epsilon<float>() || delta.z < glm::epsilon<float>())
				{
					m_localRotation = glm::angleAxis(glm::radians(delta.z), m_localRotation * glm::vec3(0, 0, 1)) * m_localRotation;
				}
				m_previousRotation = m_currentRotation;

				glm::mat4 translateMatrix = glm::translate(glm::mat4{ 1.0 }, m_localTranslation);
				glm::mat4 rotationMatrix = glm::toMat4(m_localRotation);
				glm::mat4 localTransformation = translateMatrix * rotationMatrix;

				m_perObjectData->m_modelTransform = m_modelNode.lock()->GetWorldTransfom() * localTransformation;

				const auto& constantBufferPool = RHI::Graphics::GetBufferSystem().GetCommonBufferPool(RHI::CommonBufferPoolType::Constant);

				RHI::BufferMapRequest mapRequest{};
				mapRequest.m_buffer = m_modelTransformCbuff.get();
				mapRequest.m_byteCount = sizeof(PerObjectData);
				mapRequest.m_byteOffset = 0;

				RHI::BufferMapResponse mapResponse{};

				RHI::ResultCode mapSuccess = constantBufferPool->MapBuffer(mapRequest, mapResponse);
				if (mapSuccess == RHI::ResultCode::Success)
				{
					memcpy(mapResponse.m_data, m_perObjectData, sizeof(PerObjectData));
					constantBufferPool->UnmapBuffer(*m_modelTransformCbuff);
				}
				return mapSuccess;
			}
		}

		RHI::ShaderResourceGroup* Mesh::GetObjectSrg()
		{
			return m_objectSrg.get();
		}
	}
}