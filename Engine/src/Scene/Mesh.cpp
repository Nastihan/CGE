
// assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// Scene
#include "Mesh.h"

// RHI
#include "../RHI/Graphics.h"

namespace CGE
{
	namespace Scene
	{
		Mesh::Mesh(const aiMesh& mesh)
		{
			// Load the vertex data
			for (unsigned int i = 0; i < mesh.mNumVertices; i++)
			{
				m_meshBuffers.m_interleavedVertexData.push_back(
					{
						*reinterpret_cast<RHI::Math::Vector3*>(&mesh.mVertices[i]),
						*reinterpret_cast<RHI::Math::Vector2*>(&mesh.mTextureCoords[0][i]),
						*reinterpret_cast<RHI::Math::Vector3*>(&mesh.mNormals[i]),
						*reinterpret_cast<RHI::Math::Vector3*>(&mesh.mTangents[i]),
						*reinterpret_cast<RHI::Math::Vector3*>(&mesh.mBitangents[i])
					});
			}

			// Load the vertex index data
			std::vector<unsigned short> indices;
			indices.reserve(mesh.mNumFaces * 3);
			for (unsigned int i = 0; i < mesh.mNumFaces; i++)
			{
				const auto& face = mesh.mFaces[i];
				assert(face.mNumIndices == 3);
				indices.push_back(face.mIndices[0]);
				indices.push_back(face.mIndices[1]);
				indices.push_back(face.mIndices[2]);
			}

			const auto& staticInputAssemblyBufferPool = RHI::Graphics::GetBufferSystem().GetCommonBufferPool(RHI::CommonBufferPoolType::StaticInputAssembly);
			RHI::ResultCode result = RHI::ResultCode::Fail;

			// Vertex buffer
			RHI::BufferInitRequest vertexBufferRequest;
			vertexBufferRequest.m_buffer = m_meshBuffers.m_interleavedBuffer.get();
			vertexBufferRequest.m_descriptor.m_byteCount = m_meshBuffers.m_interleavedVertexData.size() * sizeof(MeshBuffers::VertexInterleaved);
			vertexBufferRequest.m_descriptor.m_bindFlags = RHI::BufferBindFlags::InputAssembly;
			vertexBufferRequest.m_initialData = m_meshBuffers.m_interleavedVertexData.data();
			result = staticInputAssemblyBufferPool->InitBuffer(vertexBufferRequest);
			assert(result != RHI::ResultCode::Success);

			// Vertex buffer view
			m_meshBuffers.m_interleavedStreamBufferView = RHI::StreamBufferView { 
				*m_meshBuffers.m_interleavedBuffer, 
				0,  
				static_cast<uint32_t>(m_meshBuffers.m_interleavedVertexData.size() * sizeof(MeshBuffers::VertexInterleaved)), 
				sizeof(MeshBuffers::VertexInterleaved) };

			// Configure input assembler.
			m_meshBuffers.m_interleavedInputStreamLayout = {};

			m_meshBuffers.m_interleavedInputStreamLayout.SetTopology(RHI::PrimitiveTopology::TriangleList);

			m_meshBuffers.m_interleavedInputStreamLayout.AddStreamBuffer(RHI::StreamBufferDescriptor{ 
				RHI::StreamStepFunction::PerVertex, 
				1,  
				static_cast<uint32_t>(m_meshBuffers.m_interleavedVertexData.size() * sizeof(MeshBuffers::VertexInterleaved)) 
				});
			uint32_t byteOffset = 0;
			m_meshBuffers.m_interleavedInputStreamLayout.AddStreamChannel(RHI::StreamChannelDescriptor{ 
				RHI::ShaderSemantic{L"POSITION", 0}, 
				RHI::Format::R32G32B32_FLOAT,
				byteOffset,
				0});
			byteOffset += RHI::GetFormatSize(RHI::Format::R32G32B32_FLOAT);
			m_meshBuffers.m_interleavedInputStreamLayout.AddStreamChannel(RHI::StreamChannelDescriptor{
				RHI::ShaderSemantic{L"TEXCOORD", 0},
				RHI::Format::R32G32_FLOAT,
				byteOffset,
				0 });
			byteOffset += RHI::GetFormatSize(RHI::Format::R32G32_FLOAT);
			m_meshBuffers.m_interleavedInputStreamLayout.AddStreamChannel(RHI::StreamChannelDescriptor{
				RHI::ShaderSemantic{L"NORMAL", 0},
				RHI::Format::R32G32B32_FLOAT,
				byteOffset,
				0 });
			byteOffset += RHI::GetFormatSize(RHI::Format::R32G32B32_FLOAT);
			m_meshBuffers.m_interleavedInputStreamLayout.AddStreamChannel(RHI::StreamChannelDescriptor{
				RHI::ShaderSemantic{L"TANGENTS", 0},
				RHI::Format::R32G32B32_FLOAT,
				byteOffset,
				0 });
			byteOffset += RHI::GetFormatSize(RHI::Format::R32G32B32_FLOAT);
			m_meshBuffers.m_interleavedInputStreamLayout.AddStreamChannel(RHI::StreamChannelDescriptor{
				RHI::ShaderSemantic{L"BITANGENTS", 0},
				RHI::Format::R32G32B32_FLOAT,
				byteOffset,
				0 });
			byteOffset += RHI::GetFormatSize(RHI::Format::R32G32B32_FLOAT);

			m_meshBuffers.m_interleavedInputStreamLayout.Finalize();
			

			// Index buffer
			RHI::BufferInitRequest indexBufferRequest;
			indexBufferRequest.m_buffer = m_meshBuffers.m_triangleIndexBuffer.get();
			indexBufferRequest.m_descriptor.m_byteCount = indices.size() * sizeof(uint16_t);
			indexBufferRequest.m_descriptor.m_bindFlags = RHI::BufferBindFlags::InputAssembly;
			indexBufferRequest.m_initialData = indices.data();
			result = staticInputAssemblyBufferPool->InitBuffer(indexBufferRequest);
			assert(result != RHI::ResultCode::Success);

			// Index buffer view
			m_meshBuffers.m_triangleIndexBufferView = RHI::IndexBufferView { 
				*m_meshBuffers.m_triangleIndexBuffer, 
				0, 
				static_cast<uint32_t>(indices.size() * sizeof(uint16_t)),
				RHI::IndexFormat::Uint16 };
		}
	}
}