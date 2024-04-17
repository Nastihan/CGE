
#include "MeshBuilder.h"

// RHI
#include "../RHI/BufferSystem.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include <DirectXMath.h>

namespace CGE
{
	MeshBuilder::MeshBuilder(RHI::BufferSystem& bufferSystem) : m_bufferSystem(bufferSystem)
	{
		m_systemStaticInputAssemblyBufferPool = m_bufferSystem.GetCommonBufferPool(RHI::CommonBufferPoolType::Constant);
		m_systemStaticInputAssemblyBufferPool = m_bufferSystem.GetCommonBufferPool(RHI::CommonBufferPoolType::StaticInputAssembly);
	}

	MeshBuilder::Mesh MeshBuilder::LoadObjModel(std::string filePath)
	{
		Mesh suzanneMesh;

		Assimp::Importer imp;
		const auto pModel = imp.ReadFile("Models\\suzanne.obj", aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);
		const auto pMesh = pModel->mMeshes[0];

		// Vertex Buffer
		{
			struct Vertex
			{
				DirectX::XMFLOAT3 pos;
				DirectX::XMFLOAT3 n;
			};

			std::vector<Vertex> vertices;
			vertices.reserve(pMesh->mNumVertices);
			for (unsigned int i = 0; i < pMesh->mNumVertices; i++)
			{
				vertices.push_back({
					{ pMesh->mVertices[i].x,pMesh->mVertices[i].y, pMesh->mVertices[i].z },
					{ *reinterpret_cast<DirectX::XMFLOAT3*>(&pMesh->mNormals[i]) } });
			}

			RHI::BufferInitRequest vertexBufferRequest;
			vertexBufferRequest.m_buffer = suzanneMesh.m_vertexBuffer.get();
			vertexBufferRequest.m_descriptor.m_bindFlags = RHI::BufferBindFlags::InputAssembly;
			vertexBufferRequest.m_descriptor.m_byteCount = vertices.size() * sizeof(Vertex);
			vertexBufferRequest.m_initialData = vertices.data();
			m_systemStaticInputAssemblyBufferPool->InitBuffer(vertexBufferRequest);
		}

		// Index Buffer
		{
			std::vector<unsigned short> indices;
			indices.reserve(pMesh->mNumFaces * 3);

			for (unsigned int i = 0; i < pMesh->mNumFaces; i++)
			{
				const auto face = pMesh->mFaces[i];
				assert(face.mNumIndices == 3);
				indices.push_back(face.mIndices[0]);
				indices.push_back(face.mIndices[1]);
				indices.push_back(face.mIndices[2]);
			}

			RHI::BufferInitRequest indexBufferRequest;
			indexBufferRequest.m_buffer = suzanneMesh.m_indexBuffer.get();
			indexBufferRequest.m_descriptor.m_bindFlags = RHI::BufferBindFlags::InputAssembly;
			indexBufferRequest.m_descriptor.m_byteCount = indices.size() * sizeof(unsigned short);
			indexBufferRequest.m_initialData = indices.data();
			m_systemStaticInputAssemblyBufferPool->InitBuffer(indexBufferRequest);
		}

		return suzanneMesh;
	}
}