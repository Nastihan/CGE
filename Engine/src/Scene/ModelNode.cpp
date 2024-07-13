
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

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"

namespace CGE
{
	namespace Scene
	{
		ModelNode::ModelNode(const glm::mat4& localTransform) : m_localTransform(localTransform), m_name("SceneNode")
		{
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
            m_dirty = true;
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
            for (const auto& mesh : m_meshes)
            {
                srgsToBind[RHI::SrgBindingSlot::Object] = mesh->GetObjectSrg();
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

        void ModelNode::SpawnImGuiWindow()
        {
            if (ImGui::TreeNode(m_name.c_str()))
            {
                for (const auto& mesh : m_meshes)
                {
                    mesh->SpawnImGuiWindow();
                }
                ImGui::TreePop();
            }
            for (const auto& child : m_children)
            {
                child->SpawnImGuiWindow();
            }
        }

        void ModelNode::Update()
        {
            for (const auto& mesh : m_meshes)
            {
                if (m_dirty || AreAncestorsDirty())
                {
                    mesh->Update(true);
                    m_dirty = false;
                }
                else
                {
                    mesh->Update(false);
                }
            }
            for (const auto& child : m_children)
            {
                child->Update();
            }
        }

        bool ModelNode::AreAncestorsDirty() const
        {
            if (m_dirty)
            {
                return true;
            }
            if (m_parent.lock() == nullptr)
            {
                return m_dirty;
            }
            return m_parent.lock()->AreAncestorsDirty();
        }

        void ModelNode::ClearDirtyFlag()
        {
            m_dirty = false;
            for (const auto& child : m_children)
            {
                child->ClearDirtyFlag();
            }
        }
	}
}