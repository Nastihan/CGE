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
#include <glm/gtx/quaternion.hpp>

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
		class ModelNode;

		class Mesh
		{
		public:
			Mesh(const std::string& name);

			void SetMaterial(std::shared_ptr<Material> material);
			std::shared_ptr<Material> GetMaterial() const;

			void AddVertexBuffer(RHI::Ptr<RHI::Buffer> vertexBuffer, RHI::StreamBufferView& streamBufferView);
			void SetInputStreamLayout(RHI::InputStreamLayout& inputStreamLayout);
			void SetIndexBufferAndView(RHI::Ptr<RHI::Buffer> indexBuffer, RHI::IndexBufferView& indexBufferView, uint32_t indexCount);
			RHI::DrawItem* BuildAndGetDrawItem();
			void SetSrgsToBind(const std::vector<RHI::ShaderResourceGroup*>& srgsToBind);
			const RHI::ShaderResourceGroup* const* GetSrgsToBind() const;
			void SpawnImGuiWindow();
			const std::string& GetName();
			void SetModelNode(const std::shared_ptr<ModelNode> node);
			RHI::ResultCode Update(bool forcedUpdate);
			RHI::ShaderResourceGroup* GetObjectSrg();


		private:
			__declspec(align(16)) struct PerObjectData
			{
				glm::mat4 m_modelTransform;
			};

		private:
			std::string m_name;

			std::vector<RHI::Ptr<RHI::Buffer>> m_vertexBuffers;
			std::vector<RHI::StreamBufferView> m_streamBufferViews;
			RHI::InputStreamLayout m_inputStreamLayoutPacked;

			RHI::Ptr<RHI::Buffer> m_triangleIndexBuffer;
			RHI::IndexBufferView m_triangleIndexBufferView;
			uint32_t m_indexCount;

			std::shared_ptr<Material> m_material;
			RHI::DrawItem m_drawItem;

			// I need this to reflect and update the sub meshesh local and world coordinates on ImGui.
			std::weak_ptr<ModelNode> m_modelNode;
			glm::vec3 m_localTranslation;
			glm::quat m_localRotation;

			glm::vec3 m_previousRotation{};
			glm::vec3 m_currentRotation{};
			bool m_dirty = false;


			// I also moved the object to world transform data from the ModelNode class to here
			// since each inidivdual submesh will get transformed
			PerObjectData* m_perObjectData;
			RHI::Ptr<RHI::Buffer> m_modelTransformCbuff;
			RHI::Ptr<RHI::BufferView> m_modelTransformBufferView;
			RHI::Ptr<RHI::ShaderResourceGroup> m_objectSrg;

			std::vector<RHI::ShaderResourceGroup*> m_srgsToBind;
		};
	}
}