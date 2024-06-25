
// Scene
#include "ModelNode.h"
#include "Scene.h"
#include "Mesh.h"
#include "Material.h"

// Pass
#include "../Pass/ForwardPass.h"

// RHI
#include "../RHI/CommandList.h"
#include "../RHI/Graphics.h"

namespace CGE
{
	namespace Scene
	{
		ModelNode::ModelNode(const glm::mat4& localTransform) : m_localTransform(localTransform), m_name("SceneNode")
		{
            m_perObjectData = (PerObjectData*)_aligned_malloc(sizeof(PerObjectData), 16);
			m_inverseTransform = glm::inverse(m_localTransform);
		}

		ModelNode::~ModelNode()
		{
			m_children.clear();
		}

		const std::string& ModelNode::GetName() const
		{
			return m_name;
		}

		void ModelNode::SetName(const std::string& name)
		{
			m_name = name;
		}

		glm::mat4 ModelNode::GetLocalTransform() const
		{
			return m_localTransform;
		}

		glm::mat4 ModelNode::GetInverseLocalTransform() const
		{
			return m_inverseTransform;
		}

		void ModelNode::SetLocalTransform(const glm::mat4& localTransform)
		{
			m_localTransform = localTransform;
			m_inverseTransform = glm::inverse(localTransform);
		}

		glm::mat4 ModelNode::GetWorldTransfom() const
		{
			return GetParentWorldTransform() * m_localTransform;
		}

		glm::mat4 ModelNode::GetInverseWorldTransform() const
		{
			return glm::inverse(GetWorldTransfom());
		}

		void ModelNode::SetWorldTransform(const glm::mat4& worldTransform)
		{
			glm::mat4 inverseParentTransform = glm::inverse(GetParentWorldTransform());
			SetLocalTransform(inverseParentTransform * worldTransform);
		}

        void ModelNode::AddChild(std::shared_ptr<ModelNode> pNode)
        {
            if (pNode)
            {
                NodeList::iterator iter = std::find(m_children.begin(), m_children.end(), pNode);
                if (iter == m_children.end())
                {
                    glm::mat4 worldTransform = pNode->GetWorldTransfom();
                    pNode->m_parent = shared_from_this();
                    glm::mat4 localTransform = GetInverseWorldTransform() * worldTransform;
                    pNode->SetLocalTransform(localTransform);
                    m_children.push_back(pNode);
                    if (!pNode->GetName().empty())
                    {
                        m_childrenByName.insert(NodeNameMap::value_type(pNode->GetName(), pNode));
                    }
                }
            }
        }

        void ModelNode::RemoveChild(std::shared_ptr<ModelNode> pNode)
        {
            if (pNode)
            {
                NodeList::iterator iter = std::find(m_children.begin(), m_children.end(), pNode);
                if (iter != m_children.end())
                {
                    pNode->SetParent(std::weak_ptr<ModelNode>());
                    m_children.erase(iter);

                    // Also remove it from the name map.
                    NodeNameMap::iterator iter2 = m_childrenByName.find(pNode->GetName());
                    if (iter2 != m_childrenByName.end())
                    {
                        m_childrenByName.erase(iter2);
                    }
                }
                else
                {
                    // Maybe this node appears lower in the hierarchy.
                    for (auto child : m_children)
                    {
                        child->RemoveChild(pNode);
                    }
                }
            }
        }

        void ModelNode::SetParent(std::weak_ptr<ModelNode> wpNode)
        {
            std::shared_ptr<ModelNode> me = shared_from_this();
            if (std::shared_ptr<ModelNode> parent = wpNode.lock())
            {
                parent->AddChild(shared_from_this());
            }
            else if (parent = m_parent.lock())
            {
                glm::mat4 worldTransform = GetWorldTransfom();
                parent->RemoveChild(shared_from_this());
                m_parent.reset();
                SetLocalTransform(worldTransform);
            }
        }

        void ModelNode::AddMesh(std::shared_ptr<Mesh> mesh)
        {
            assert(mesh);
            MeshList::iterator iter = std::find(m_meshes.begin(), m_meshes.end(), mesh);
            if (iter == m_meshes.end())
            {
                m_meshes.push_back(mesh);
            }
        }

        void ModelNode::RemoveMesh(std::shared_ptr<Mesh> mesh)
        {
            assert(mesh);
            MeshList::iterator iter = std::find(m_meshes.begin(), m_meshes.end(), mesh);
            if (iter != m_meshes.end())
            {
                m_meshes.erase(iter);
            }
        }

        glm::mat4 ModelNode::GetParentWorldTransform() const
        {
            glm::mat4 parentTransform(1.0f);
            if (std::shared_ptr<ModelNode> parent = m_parent.lock())
            {
                parentTransform = parent->GetWorldTransfom();
            }
            return parentTransform;
        }

        void ModelNode::BuildDrawList(std::vector<RHI::DrawItem>& drawList, std::array<RHI::ShaderResourceGroup*, RHI::Limits::Pipeline::ShaderResourceGroupCountMax>& srgsToBind)
        {
            srgsToBind[RHI::SrgBindingSlot::Object] = m_objectSrg.get();
            for (const auto& mesh : m_meshes)
            {
                srgsToBind[RHI::SrgBindingSlot::Material] = mesh->GetMaterial()->GetMaterialSrg();

                drawList.push_back(*mesh->BuildAndGetDrawItem());
                auto& currentItem = drawList.back();
                
                std::vector<RHI::ShaderResourceGroup*> srgs;
                for (RHI::ShaderResourceGroup* srg : srgsToBind)
                {
                    if (srg)
                    {
                        srgs.push_back(srg);
                    }
                }
                mesh->SetSrgsToBind(srgs);

                currentItem.m_shaderResourceGroupCount = srgs.size();
                currentItem.m_shaderResourceGroups = mesh->GetSrgsToBind();
            }
            for (const auto& node : m_children)
            {
                node->BuildDrawList(drawList, srgsToBind);
            }
        }

        RHI::ResultCode ModelNode::BuildModelMatrix()
        {
            m_perObjectData->m_modelTransform = GetParentWorldTransform() * m_localTransform;

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

            return result;
        }
	}
}