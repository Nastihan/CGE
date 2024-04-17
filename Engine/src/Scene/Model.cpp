
// Scene
#include "ModelNode.h"
#include "Mesh.h"
#include "Model.h"

// assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// std
#include <assert.h>

namespace CGE
{
	namespace Scene
	{
		Model::Model(const std::string& pathString)
		{
			Assimp::Importer imp;
			const auto pScene = imp.ReadFile(pathString.c_str(),
				aiProcess_Triangulate |
				aiProcess_JoinIdenticalVertices |
				aiProcess_ConvertToLeftHanded |
				aiProcess_GenNormals |
				aiProcess_CalcTangentSpace);
			assert(pScene);

			for (size_t i = 0; i < pScene->mNumMeshes; i++)
			{
				const auto& mesh = *pScene->mMeshes[i];
				m_meshPtrs.push_back(std::make_unique<Mesh>(mesh));
			}
		}
	}
}