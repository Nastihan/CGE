#pragma once

// RHI
#include "../RHI/Buffer.h"
#include "../RHI/Image.h"
#include "../RHI/DrawItem.h"

// Scene
#include "Material.h"

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace CGE
{
	namespace Scene
	{
		struct MeshBuffers
		{
			RHI::Ptr<RHI::Buffer> m_positionBuffer;
			RHI::Ptr<RHI::Buffer> m_normalBuffer;
			RHI::Ptr<RHI::Buffer> m_textureCoordinateBuffer;
			RHI::Ptr<RHI::Buffer> m_tangentBuffer;
			RHI::Ptr<RHI::Buffer> m_bitangentBuffer;

			RHI::StreamBufferView m_positionBufferView;
			RHI::StreamBufferView m_normalBufferView;
			RHI::StreamBufferView m_textureCoordinateBufferView;
			RHI::StreamBufferView m_tangentBufferView;
			RHI::StreamBufferView m_bitangentBufferView;

			// This is what we set for the drawitem ([todo] move later since we dont need two copies)
			// Order should match the shader
			std::vector<RHI::StreamBufferView> m_streamBufferViews;

			uint32_t m_triangleIndexCount;
			RHI::Ptr<RHI::Buffer> m_indexBuffer;
			RHI::IndexBufferView m_indexBufferView;
		};

		class Shape
		{
		private:
			__declspec(align(16)) struct PerObjectData
			{
				glm::mat4 m_modelTransform;
			};
		public:
			Shape(glm::vec3 pos, glm::vec3 scale, glm::quat rotation, std::shared_ptr<Material> material);
			Shape() = delete;
			virtual ~Shape() = default;

			// Return to pass for rendering
			RHI::DrawItem GetDrawItem();
			Material& GetMaterial();
			void BuildDrawList(std::vector<RHI::DrawItem>& drawItems, std::array<RHI::ShaderResourceGroup*, RHI::Limits::Pipeline::ShaderResourceGroupCountMax>& srgsToBind) const;

		protected:
			std::pair<glm::vec3, glm::vec3> CalculateTangentAndBitangent(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec2 uv1, glm::vec2 uv2, glm::vec2 uv3);
			RHI::ResultCode ConstructStreamBuffer(RHI::Ptr<RHI::Buffer> buffer, const void* data, unsigned int count, unsigned int stride);

		protected:
			MeshBuffers m_meshBuffers;
			RHI::DrawItem m_drawItem;

		private:
			Material m_material;

			glm::vec3 m_worldPos;
			glm::vec3 m_worldScale;
			glm::quat m_worldRotation;
			PerObjectData* m_perObjectData;
			RHI::Ptr<RHI::Buffer> m_modelToWorldTransformCbuff;
			RHI::Ptr<RHI::BufferView> m_modelToWorldTransformCbuffView;

			std::vector<RHI::ShaderResourceGroup*> m_srgsToBind;
		};

		class Box : public Shape
		{
			using Base = Shape;
		public:
			Box(glm::vec3 pos, glm::vec3 scale, glm::quat rotation, std::shared_ptr<Material> material);
			Box() = delete;
			~Box() = default;
		};

		class Sphere : public Shape
		{
			using Base = Shape;
		public:
			Sphere(glm::vec3 pos
				, glm::vec3 scale
				, glm::quat rotation
				, float radius
				, uint32_t latitudeDivisions
				, uint32_t longitudeDivisions
				, std::shared_ptr<Material> material);
			~Sphere() = default;
		};
	}
}