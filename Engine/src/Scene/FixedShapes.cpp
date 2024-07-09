
// Scene
#include "FixedShapes.h"

// RHI
#include "../RHI/Graphics.h"

namespace CGE
{
	namespace Scene
	{
		Shape::Shape(glm::vec3 pos, glm::vec3 scale, glm::quat rotation) : m_worldPos(pos), m_worldScale(scale), m_worldRotation(rotation)
		{
			m_perObjectData = (PerObjectData*)_aligned_malloc(sizeof(PerObjectData), 16);

			glm::mat4 translateMatrix = glm::translate(glm::mat4{ 1.0 }, m_worldPos);
			glm::mat4 rotationMatrix = glm::toMat4(m_worldRotation);
			glm::mat4 localTransformation = translateMatrix * rotationMatrix;

			m_perObjectData->m_modelTransform = localTransformation;

			const auto& constantBufferPool = RHI::Graphics::GetBufferSystem().GetCommonBufferPool(RHI::CommonBufferPoolType::Constant);
			auto& rhiFactory = RHI::Graphics::GetFactory();
			m_modelToWorldTransformCbuff = rhiFactory.CreateBuffer();
			RHI::ResultCode result = RHI::ResultCode::Fail;

			RHI::BufferInitRequest modelCbufferRequest;
			modelCbufferRequest.m_buffer = m_modelToWorldTransformCbuff.get();
			modelCbufferRequest.m_descriptor.m_byteCount = sizeof(PerObjectData);
			modelCbufferRequest.m_descriptor.m_bindFlags = RHI::BufferBindFlags::Constant;
			modelCbufferRequest.m_initialData = m_perObjectData;
			result = constantBufferPool->InitBuffer(modelCbufferRequest);
			assert(result == RHI::ResultCode::Success);

			RHI::BufferViewDescriptor modelBufferViewDescriptor = RHI::BufferViewDescriptor::CreateRaw(0, sizeof(PerObjectData));
			m_modelToWorldTransformCbuffView = rhiFactory.CreateBufferView();
			m_modelToWorldTransformCbuffView->Init(*m_modelToWorldTransformCbuff, modelBufferViewDescriptor);
		}

		RHI::DrawItem Shape::GetDrawItem()
		{
			return m_drawItem;
		}

		Material& Shape::GetMaterial()
		{
			return m_material;
		}

		void Shape::BuildDrawList(std::vector<RHI::DrawItem>& drawItems, std::array<RHI::ShaderResourceGroup*, RHI::Limits::Pipeline::ShaderResourceGroupCountMax>& srgsToBind) const
		{

		}

		std::pair<glm::vec3, glm::vec3> Shape::CalculateTangentAndBitangent(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec2 uv1, glm::vec2 uv2, glm::vec2 uv3)
		{
			glm::vec3 tangent{};
			glm::vec3 bitangent{};

			glm::vec3 edge1 = v2 - v1;
			glm::vec3 edge2 = v3 - v1;
			glm::vec2 deltaUV1 = uv2 - uv1;
			glm::vec2 deltaUV2 = uv3 - uv1;

			float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

			tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
			tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
			tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

			bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
			bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
			bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

			return { tangent, bitangent };
		}

		RHI::ResultCode Shape::ConstructStreamBuffer(RHI::Ptr<RHI::Buffer> buffer, const void* data, unsigned int count, unsigned int stride)
		{
			const auto& staticInputAssemblyBufferPool = RHI::Graphics::GetBufferSystem().GetCommonBufferPool(RHI::CommonBufferPoolType::StaticInputAssembly);
			RHI::ResultCode result = RHI::ResultCode::Fail;

			// Vertex buffer
			RHI::BufferInitRequest vertexBufferRequest;
			vertexBufferRequest.m_buffer = buffer.get();
			vertexBufferRequest.m_descriptor.m_byteCount = count * stride;
			vertexBufferRequest.m_descriptor.m_bindFlags = RHI::BufferBindFlags::InputAssembly | RHI::BufferBindFlags::ShaderRead;
			vertexBufferRequest.m_initialData = data;
			result = staticInputAssemblyBufferPool->InitBuffer(vertexBufferRequest);
			assert(result == RHI::ResultCode::Success);

			return result;
		}

		Box::Box(glm::vec3 pos, glm::vec3 scale, glm::quat rotation) : Base(pos, scale, rotation)
		{
			auto& rhiFactory = RHI::Graphics::GetFactory();
			const auto& staticInputAssemblyBufferPool = RHI::Graphics::GetBufferSystem().GetCommonBufferPool(RHI::CommonBufferPoolType::StaticInputAssembly);

			const uint32_t numberOfVertices = 6 * 4; // Number of faces * Number of vertices for each face
			const uint32_t numberOfIndices = 2 * 3 * 6; // Number of triangles for each face * Number of indicies needed for each triangle * Number of faces

			std::vector<glm::vec3> vertices;
			std::vector<uint32_t> indices;
			std::vector<glm::vec2> UVs;
			std::vector<glm::vec3> normals;
			std::vector<glm::vec3> tangents;
			std::vector<glm::vec3> bitangents;

			vertices.reserve(numberOfVertices);
			indices.reserve(numberOfIndices);
			UVs.reserve(numberOfVertices);
			normals.reserve(numberOfVertices);
			tangents.reserve(numberOfVertices);
			bitangents.reserve(numberOfVertices);

			// left face
			vertices.push_back(glm::vec3(-0.5, 0.5, 0.5)); // 0
			vertices.push_back(glm::vec3(-0.5, 0.5, -0.5)); // 1
			vertices.push_back(glm::vec3(-0.5, -0.5, -0.5)); // 2
			vertices.push_back(glm::vec3(-0.5, -0.5, 0.5)); // 3

			// bottom face
			vertices.push_back(glm::vec3(-0.5, -0.5, 0.5)); // 4
			vertices.push_back(glm::vec3(-0.5, -0.5, -0.5)); // 5
			vertices.push_back(glm::vec3(0.5, -0.5, -0.5)); // 6
			vertices.push_back(glm::vec3(0.5, -0.5, 0.5)); // 7

			// right face
			vertices.push_back(glm::vec3(0.5, -0.5, 0.5)); // 8
			vertices.push_back(glm::vec3(0.5, -0.5, -0.5)); // 9
			vertices.push_back(glm::vec3(0.5, 0.5, -0.5)); // 10
			vertices.push_back(glm::vec3(0.5, 0.5, 0.5)); // 11

			// top face
			vertices.push_back(glm::vec3(0.5, 0.5, 0.5)); // 12
			vertices.push_back(glm::vec3(0.5, 0.5, -0.5)); // 13
			vertices.push_back(glm::vec3(-0.5, 0.5, -0.5)); // 14
			vertices.push_back(glm::vec3(-0.5, 0.5, 0.5)); // 15

			// front face
			vertices.push_back(glm::vec3(-0.5, -0.5, -0.5)); // 16
			vertices.push_back(glm::vec3(-0.5, 0.5, -0.5)); // 17
			vertices.push_back(glm::vec3(0.5, 0.5, -0.5)); // 18
			vertices.push_back(glm::vec3(0.5, -0.5, -0.5)); // 19

			// back face
			vertices.push_back(glm::vec3(-0.5, 0.5, 0.5)); // 20
			vertices.push_back(glm::vec3(-0.5, -0.5, 0.5)); // 21
			vertices.push_back(glm::vec3(0.5, -0.5, 0.5)); // 22
			vertices.push_back(glm::vec3(0.5, 0.5, 0.5)); // 23

			// left face
			indices.push_back(0);
			indices.push_back(1);
			indices.push_back(2);
			indices.push_back(0);
			indices.push_back(2);
			indices.push_back(3);

			// bottom face
			indices.push_back(4);
			indices.push_back(5);
			indices.push_back(6);
			indices.push_back(4);
			indices.push_back(6);
			indices.push_back(7);

			// right face
			indices.push_back(8);
			indices.push_back(9);
			indices.push_back(10);
			indices.push_back(8);
			indices.push_back(10);
			indices.push_back(11);

			// top face
			indices.push_back(12);
			indices.push_back(13);
			indices.push_back(14);
			indices.push_back(12);
			indices.push_back(14);
			indices.push_back(15);

			// front face
			indices.push_back(16);
			indices.push_back(17);
			indices.push_back(18);
			indices.push_back(16);
			indices.push_back(18);
			indices.push_back(19);

			// back face
			indices.push_back(20);
			indices.push_back(21);
			indices.push_back(22);
			indices.push_back(20);
			indices.push_back(22);
			indices.push_back(23);

			// left face
			UVs.push_back(glm::vec2(0.0, 0.0)); // 0
			UVs.push_back(glm::vec2(1.0, 0.0)); // 1
			UVs.push_back(glm::vec2(1.0, 1.0)); // 2
			UVs.push_back(glm::vec2(0.0, 1.0)); // 3

			// bottom face
			UVs.push_back(glm::vec2(0.0, 1.0)); // 4
			UVs.push_back(glm::vec2(0.0, 0.0)); // 5
			UVs.push_back(glm::vec2(1.0, 0.0)); // 6
			UVs.push_back(glm::vec2(1.0, 1.0)); // 7

			// right face
			UVs.push_back(glm::vec2(1.0, 1.0)); // 8
			UVs.push_back(glm::vec2(0.0, 1.0)); // 9
			UVs.push_back(glm::vec2(0.0, 0.0)); // 10
			UVs.push_back(glm::vec2(1.0, 0.0)); // 11

			// top face
			UVs.push_back(glm::vec2(1.0, 0.0)); // 12
			UVs.push_back(glm::vec2(1.0, 1.0)); // 13
			UVs.push_back(glm::vec2(0.0, 1.0)); // 14
			UVs.push_back(glm::vec2(0.0, 0.0)); // 15

			// front face
			UVs.push_back(glm::vec2(0.0, 1.0)); // 16
			UVs.push_back(glm::vec2(0.0, 0.0)); // 17
			UVs.push_back(glm::vec2(1.0, 0.0)); // 18
			UVs.push_back(glm::vec2(1.0, 1.0)); // 19

			// back face
			UVs.push_back(glm::vec2(1.0, 0.0)); // 20
			UVs.push_back(glm::vec2(1.0, 1.0)); // 21
			UVs.push_back(glm::vec2(0.0, 1.0)); // 22
			UVs.push_back(glm::vec2(0.0, 0.0)); // 23

			// left face
			normals.push_back(glm::vec3(-1.0, 0.0, 0.0)); // 0
			normals.push_back(glm::vec3(-1.0, 0.0, 0.0)); // 1
			normals.push_back(glm::vec3(-1.0, 0.0, 0.0)); // 2
			normals.push_back(glm::vec3(-1.0, 0.0, 0.0)); // 3

			// bottom face
			normals.push_back(glm::vec3(0.0, -1.0, 0.0)); // 4
			normals.push_back(glm::vec3(0.0, -1.0, 0.0)); // 5
			normals.push_back(glm::vec3(0.0, -1.0, 0.0)); // 6
			normals.push_back(glm::vec3(0.0, -1.0, 0.0)); // 7

			// right face
			normals.push_back(glm::vec3(1.0, 0.0, 0.0)); // 8
			normals.push_back(glm::vec3(1.0, 0.0, 0.0)); // 9
			normals.push_back(glm::vec3(1.0, 0.0, 0.0)); // 10
			normals.push_back(glm::vec3(1.0, 0.0, 0.0)); // 11

			// top face
			normals.push_back(glm::vec3(0.0, 1.0, 0.0)); // 12
			normals.push_back(glm::vec3(0.0, 1.0, 0.0)); // 13
			normals.push_back(glm::vec3(0.0, 1.0, 0.0)); // 14
			normals.push_back(glm::vec3(0.0, 1.0, 0.0)); // 15

			// front face
			normals.push_back(glm::vec3(0.0, 0.0, -1.0)); // 16
			normals.push_back(glm::vec3(0.0, 0.0, -1.0)); // 17
			normals.push_back(glm::vec3(0.0, 0.0, -1.0)); // 18
			normals.push_back(glm::vec3(0.0, 0.0, -1.0)); // 19

			// back face
			normals.push_back(glm::vec3(0.0, 0.0, 1.0)); // 20
			normals.push_back(glm::vec3(0.0, 0.0, 1.0)); // 21
			normals.push_back(glm::vec3(0.0, 0.0, 1.0)); // 22
			normals.push_back(glm::vec3(0.0, 0.0, 1.0)); // 23

			// The 4 verticies for each face in a cube will have the same Normal, Tangent and Bitangent.
			for (int i = 0; i < vertices.size(); i = i + 4)
			{
				glm::vec3 v1 = vertices[i];
				glm::vec3 v2 = vertices[i + 1];
				glm::vec3 v3 = vertices[i + 3];

				glm::vec2 uv1 = UVs[i];
				glm::vec2 uv2 = UVs[i + 1];
				glm::vec2 uv3 = UVs[i + 3];

				std::pair<glm::vec3, glm::vec3> TB = CalculateTangentAndBitangent(v1, v3, v2, uv1, uv3, uv2);
				
				tangents.push_back(TB.first);
				tangents.push_back(TB.first);
				tangents.push_back(TB.first);
				tangents.push_back(TB.first);

				bitangents.push_back(TB.second);
				bitangents.push_back(TB.second);
				bitangents.push_back(TB.second);
				bitangents.push_back(TB.second);
			}

			// Stream buffers
			m_meshBuffers.m_positionBuffer = rhiFactory.CreateBuffer();
			m_meshBuffers.m_normalBuffer = rhiFactory.CreateBuffer();
			m_meshBuffers.m_tangentBuffer = rhiFactory.CreateBuffer();
			m_meshBuffers.m_bitangentBuffer = rhiFactory.CreateBuffer();
			m_meshBuffers.m_textureCoordinateBuffer = rhiFactory.CreateBuffer();

			ConstructStreamBuffer(m_meshBuffers.m_positionBuffer, vertices.data(), vertices.size(), sizeof(glm::vec3));
			ConstructStreamBuffer(m_meshBuffers.m_normalBuffer, normals.data(), normals.size(), sizeof(glm::vec3));
			ConstructStreamBuffer(m_meshBuffers.m_tangentBuffer, tangents.data(), tangents.size(), sizeof(glm::vec3));
			ConstructStreamBuffer(m_meshBuffers.m_bitangentBuffer, bitangents.data(), bitangents.size(), sizeof(glm::vec3));
			ConstructStreamBuffer(m_meshBuffers.m_textureCoordinateBuffer, UVs.data(), UVs.size(), sizeof(glm::vec2));

			m_meshBuffers.m_positionBufferView = RHI::StreamBufferView(*m_meshBuffers.m_positionBuffer, 0, vertices.size() * sizeof(glm::vec3), sizeof(glm::vec3));
			m_meshBuffers.m_normalBufferView = RHI::StreamBufferView(*m_meshBuffers.m_normalBuffer, 0, normals.size() * sizeof(glm::vec3), sizeof(glm::vec3));
			m_meshBuffers.m_tangentBufferView = RHI::StreamBufferView(*m_meshBuffers.m_tangentBuffer, 0, tangents.size() * sizeof(glm::vec3), sizeof(glm::vec3));
			m_meshBuffers.m_bitangentBufferView = RHI::StreamBufferView(*m_meshBuffers.m_bitangentBuffer, 0, bitangents.size() * sizeof(glm::vec3), sizeof(glm::vec3));
			m_meshBuffers.m_textureCoordinateBufferView = RHI::StreamBufferView(*m_meshBuffers.m_textureCoordinateBuffer, 0, UVs.size() * sizeof(glm::vec2), sizeof(glm::vec2));

			// Triangle index buffer
			m_meshBuffers.m_indexBuffer = rhiFactory.CreateBuffer();
			m_meshBuffers.m_triangleIndexCount = indices.size();
			ConstructStreamBuffer(m_meshBuffers.m_indexBuffer, indices.data(), indices.size(), sizeof(uint32_t));
			m_meshBuffers.m_indexBufferView = RHI::IndexBufferView{ *m_meshBuffers.m_indexBuffer, 0, static_cast<uint32_t>(indices.size() * sizeof(uint32_t)), RHI::IndexFormat::Uint32 };

			m_meshBuffers.m_streamBufferViews.push_back(m_meshBuffers.m_positionBufferView);
			m_meshBuffers.m_streamBufferViews.push_back(m_meshBuffers.m_normalBufferView);
			m_meshBuffers.m_streamBufferViews.push_back(m_meshBuffers.m_tangentBufferView);
			m_meshBuffers.m_streamBufferViews.push_back(m_meshBuffers.m_bitangentBufferView);
			m_meshBuffers.m_streamBufferViews.push_back(m_meshBuffers.m_textureCoordinateBufferView);
			m_drawItem.m_streamBufferViewCount = m_meshBuffers.m_streamBufferViews.size();
			m_drawItem.m_streamBufferViews = m_meshBuffers.m_streamBufferViews.data();

			m_drawItem.m_indexBufferView = &m_meshBuffers.m_indexBufferView;
			RHI::DrawIndexed drawIndexedArgs;
			drawIndexedArgs.m_indexCount = m_meshBuffers.m_triangleIndexCount;
			drawIndexedArgs.m_indexOffset = 0;
			drawIndexedArgs.m_instanceCount = 1;
			drawIndexedArgs.m_instanceOffset = 0;
			drawIndexedArgs.m_vertexOffset = 0;
			m_drawItem.m_arguments = { drawIndexedArgs };
		}

		Sphere::Sphere(glm::vec3 pos
			, glm::vec3 scale
			, glm::quat rotation
			, float radius
			, uint32_t latitudeDivisions
			, uint32_t longitudeDivisions) : Base(pos, scale, rotation)
		{

		}
	}
}