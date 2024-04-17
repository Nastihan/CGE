
#include "ModelNode.h"

namespace CGE
{
	namespace Scene
	{
		ModelNode::ModelNode(int id, const std::string& name, std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& transform) : m_meshPtrs(std::move(meshPtrs)), m_name(name)
		{
			DirectX::XMStoreFloat4x4(&m_transform, transform);
			DirectX::XMStoreFloat4x4(&appliedTransform, DirectX::XMMatrixIdentity());
		}
	}
}