
#include "ModelNode.h"

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
	}
}