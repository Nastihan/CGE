
// Scene
#include "Model.h"
#include "ModelNode.h"
#include "Mesh.h"
#include "Material.h"
#include "Scene.h"

// Pass
#include "../Pass/ForwardPass.h"

// assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// std
#include <assert.h>
#include <filesystem>

// DXToolKit
// [todo] I dont want RHI level classes to be dependant on a platform specific toolkit
// Need to replace this with a platform independent texture loader later.
#include <DirectXTex/DirectXTex.h>
using namespace DirectX;

// glm
#include <glm/gtx/compatibility.hpp>

// RHI
#include "../RHI/Graphics.h"
#include "../RHI/Image.h"
#include "../RHI/Buffer.h"
#include "../RHI/CommandList.h"

// DX12
#include "../DX_Interface/DX_CommonHeaders.h"
#include "../DX_Interface/DX_Conversions.h"

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"

#define STRINGIFY(x) #x
#define EXPAND(x) STRINGIFY(x)


namespace CGE
{
	namespace Scene
	{

        Model::Model(const std::string& name, glm::vec3 rootTranslation, glm::vec3 rootScale, glm::quat rootRotation)
            : m_root{ nullptr }
            , m_modelName{ name }
            , m_rootTranslation{ rootTranslation }
            , m_rootScale{ rootScale }
            , m_rootRotation{ rootRotation }
            , m_currentRotation{ glm::degrees(glm::eulerAngles(m_rootRotation)) }
            , m_previousRotation{ glm::degrees(glm::eulerAngles(m_rootRotation)) } {}

		Model::~Model() {}

        bool Model::LoadFromFile(const std::string& pathString, const std::string modelName)
		{
            std::string projPath = EXPAND(UNITTESTPRJ);
            projPath.erase(0, 1);
            projPath.erase(projPath.size() - 2);
            std::filesystem::path fullFilePath = projPath + "Assets\\Models\\" + pathString;
            std::filesystem::path rootModelPath = fullFilePath.parent_path();

            Assimp::Importer imp;
            const aiScene* pScene;
            pScene = imp.ReadFile(fullFilePath.string(),
                aiProcess_Triangulate |
                aiProcess_JoinIdenticalVertices |
                aiProcess_ConvertToLeftHanded |
                aiProcess_GenNormals |
                aiProcess_CalcTangentSpace);
            assert(pScene);

            // If we have a previously loaded scene, delete it.
            glm::mat4 localTransform(1);
            if (m_root)
            {
                // Save the root nodes local transform
                // so it can be restored on reload.
                localTransform = m_root->GetLocalTransform();
                m_root.reset();
            }
            // Delete the previously loaded assets.
            m_materials.clear();
            m_meshes.clear();

            // Import scene materials.
            for (unsigned int i = 0; i < pScene->mNumMaterials; ++i)
            {
                ImportMaterial(*pScene->mMaterials[i], rootModelPath.string());
            }
            // Import meshes
            for (unsigned int i = 0; i < pScene->mNumMeshes; ++i)
            {
                ImportMesh(*pScene->mMeshes[i]);
            }

            glm::mat4 translateMatrix = glm::translate(glm::mat4(1.0), m_rootTranslation);
            glm::mat4 rotationMatrix = glm::toMat4(m_rootRotation);
            glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0), m_rootScale);
            glm::mat4 worldTransform = (translateMatrix * (rotationMatrix * scaleMatrix));

            m_root = ImportSceneNode(m_root, pScene->mRootNode);
            m_root->SetLocalTransform(localTransform);
            m_root->SetWorldTransform(worldTransform);

            return true;
		}

        const std::string& Model::GetName()
        {
            return m_modelName;
        }

		void Model::ImportMaterial(const aiMaterial& material, const std::string& parentPath)
		{
			aiString materialName;
			aiString aiTexturePath;
			aiTextureOp aiBlendOperation;
			float blendFactor;
			aiColor4D diffuseColor;
			aiColor4D specularColor;
			aiColor4D ambientColor;
			aiColor4D emissiveColor;
			float opacity;
			float indexOfRefraction;
			float reflectivity;
			float shininess;
			float bumpIntensity;

            const auto rootPath = parentPath + "\\";

            // [todo] For now models loaded will use the SpecularGlossiness_MaterialLayout
            // Later on I will enable to switch materials
            std::shared_ptr<Material> pMaterial = std::make_shared<Material>();
            // Copy the material so the instance is unique to the mesh
            *pMaterial = *RHI::Graphics::GetAssetProcessor().GetMaterialLayout("SpecularGlossiness_MaterialLayout");

            if (material.Get(AI_MATKEY_COLOR_AMBIENT, ambientColor) == aiReturn_SUCCESS)
            {
                pMaterial->SetProperty("AmbientColor", glm::vec4(ambientColor.r, ambientColor.g, ambientColor.b, ambientColor.a));
            }
            if (material.Get(AI_MATKEY_COLOR_EMISSIVE, emissiveColor) == aiReturn_SUCCESS)
            {
                pMaterial->SetProperty("EmissiveColor", glm::vec4(emissiveColor.r, emissiveColor.g, emissiveColor.b, emissiveColor.a));
            }
            if (material.Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor) == aiReturn_SUCCESS)
            {
                pMaterial->SetProperty("DiffuseColor", glm::vec4(diffuseColor.r, diffuseColor.g, diffuseColor.b, diffuseColor.a));
            }
            if (material.Get(AI_MATKEY_COLOR_SPECULAR, specularColor) == aiReturn_SUCCESS)
            {
                pMaterial->SetProperty("SpecularColor", glm::vec4(specularColor.r, specularColor.g, specularColor.b, specularColor.a));
            }
            if (material.Get(AI_MATKEY_SHININESS, shininess) == aiReturn_SUCCESS)
            {
                pMaterial->SetProperty("SpecularPower", shininess);
            }
            if (material.Get(AI_MATKEY_OPACITY, opacity) == aiReturn_SUCCESS)
            {
                pMaterial->SetProperty("Opacity", opacity);
            }
            if (material.Get(AI_MATKEY_REFRACTI, indexOfRefraction))
            {
                pMaterial->SetProperty("IndexOfRefraction", indexOfRefraction);
            }
            if (material.Get(AI_MATKEY_REFLECTIVITY, reflectivity) == aiReturn_SUCCESS)
            {
                pMaterial->SetProperty("Reflectance", glm::float4(reflectivity));
            }
            if (material.Get(AI_MATKEY_BUMPSCALING, bumpIntensity) == aiReturn_SUCCESS)
            {
                pMaterial->SetProperty("BumpIntensity", bumpIntensity);
            }

            const auto& imagePool = RHI::Graphics::GetImageSystem().GetSimpleImagePool();

            // Load ambient textures.
            if (material.GetTextureCount(aiTextureType_AMBIENT) > 0 &&
                material.GetTexture(aiTextureType_AMBIENT, 0, &aiTexturePath, nullptr, nullptr, &blendFactor, &aiBlendOperation) == aiReturn_SUCCESS)
            {
                TexMetadata metaData;
                ScratchImage scratchImage;
                std::string fullPath = rootPath + aiTexturePath.C_Str();
                DX12::DXAssertSuccess(LoadFromWICFile(DX12::s2ws(fullPath).c_str(), WIC_FLAGS_FORCE_RGB, &metaData, scratchImage));

                RHI::Ptr<RHI::Image> image = RHI::Graphics::GetFactory().CreateImage();
                ConstructTexture(image, metaData, scratchImage);

                RHI::ImageDescriptor imageDesc = image->GetDescriptor();
                RHI::Ptr<RHI::ImageView> imageView = RHI::Graphics::GetFactory().CreateImageView();
                RHI::ImageViewDescriptor imageViewDesc = RHI::ImageViewDescriptor::Create(imageDesc.m_format, 0, 0);
                imageView->Init(*image, imageViewDesc);

                pMaterial->SetProperty("HasAmbientTexture", true);
                pMaterial->SetTexture("PerMaterial_AmbientTexture", image, imageView);
            }

            // Load emissive textures.
            if (material.GetTextureCount(aiTextureType_EMISSIVE) > 0 &&
                material.GetTexture(aiTextureType_EMISSIVE, 0, &aiTexturePath, nullptr, nullptr, &blendFactor, &aiBlendOperation) == aiReturn_SUCCESS)
            {
                TexMetadata metaData;
                ScratchImage scratchImage;
                std::string fullPath = rootPath + aiTexturePath.C_Str();
                DX12::DXAssertSuccess(LoadFromWICFile(DX12::s2ws(fullPath).c_str(), WIC_FLAGS_FORCE_RGB, &metaData, scratchImage));

                RHI::Ptr<RHI::Image> image = RHI::Graphics::GetFactory().CreateImage();
                ConstructTexture(image, metaData, scratchImage);

                RHI::ImageDescriptor imageDesc = image->GetDescriptor();
                RHI::Ptr<RHI::ImageView> imageView = RHI::Graphics::GetFactory().CreateImageView();
                RHI::ImageViewDescriptor imageViewDesc = RHI::ImageViewDescriptor::Create(imageDesc.m_format, 0, 0);
                imageView->Init(*image, imageViewDesc);

                pMaterial->SetProperty("HasEmissiveTexture", true);
                pMaterial->SetTexture("PerMaterial_EmissiveTexture", image, imageView);
            }

            // Load diffuse textures.
            if (material.GetTextureCount(aiTextureType_DIFFUSE) > 0 &&
                material.GetTexture(aiTextureType_DIFFUSE, 0, &aiTexturePath, nullptr, nullptr, &blendFactor, &aiBlendOperation) == aiReturn_SUCCESS)
            {
                TexMetadata metaData;
                ScratchImage scratchImage;
                std::string fullPath = rootPath + aiTexturePath.C_Str();
                DX12::DXAssertSuccess(LoadFromWICFile(DX12::s2ws(fullPath).c_str(), WIC_FLAGS_FORCE_RGB, &metaData, scratchImage));

                RHI::Ptr<RHI::Image> image = RHI::Graphics::GetFactory().CreateImage();
                ConstructTexture(image, metaData, scratchImage);
                
                RHI::ImageDescriptor imageDesc = image->GetDescriptor();
                RHI::Ptr<RHI::ImageView> imageView = RHI::Graphics::GetFactory().CreateImageView();
                RHI::ImageViewDescriptor imageViewDesc = RHI::ImageViewDescriptor::Create(imageDesc.m_format, 0, 0);
                imageView->Init(*image, imageViewDesc);

                pMaterial->SetProperty("HasDiffuseTexture", true);
                pMaterial->SetTexture("PerMaterial_DiffuseTexture", image, imageView);
            }

            // Load specular texture.
            if (material.GetTextureCount(aiTextureType_SPECULAR) > 0 &&
                material.GetTexture(aiTextureType_SPECULAR, 0, &aiTexturePath, nullptr, nullptr, &blendFactor, &aiBlendOperation) == aiReturn_SUCCESS)
            {
                TexMetadata metaData;
                ScratchImage scratchImage;
                std::string fullPath = rootPath + aiTexturePath.C_Str();
                DX12::DXAssertSuccess(LoadFromWICFile(DX12::s2ws(fullPath).c_str(), WIC_FLAGS_FORCE_RGB, &metaData, scratchImage));

                RHI::Ptr<RHI::Image> image = RHI::Graphics::GetFactory().CreateImage();
                ConstructTexture(image, metaData, scratchImage);
                
                RHI::ImageDescriptor imageDesc = image->GetDescriptor();
                RHI::Ptr<RHI::ImageView> imageView = RHI::Graphics::GetFactory().CreateImageView();
                RHI::ImageViewDescriptor imageViewDesc = RHI::ImageViewDescriptor::Create(imageDesc.m_format, 0, 0);
                imageView->Init(*image, imageViewDesc);

                pMaterial->SetProperty("HasSpecularTexture", true);
                pMaterial->SetTexture("PerMaterial_SpecularTexture", image, imageView);
            }


            // Load specular power texture.
            if (material.GetTextureCount(aiTextureType_SHININESS) > 0 &&
                material.GetTexture(aiTextureType_SHININESS, 0, &aiTexturePath, nullptr, nullptr, &blendFactor, &aiBlendOperation) == aiReturn_SUCCESS)
            {
                TexMetadata metaData;
                ScratchImage scratchImage;
                std::string fullPath = rootPath + aiTexturePath.C_Str();
                DX12::DXAssertSuccess(LoadFromWICFile(DX12::s2ws(fullPath).c_str(), WIC_FLAGS_FORCE_RGB, &metaData, scratchImage));

                RHI::Ptr<RHI::Image> image = RHI::Graphics::GetFactory().CreateImage();
                ConstructTexture(image, metaData, scratchImage);
                
                RHI::ImageDescriptor imageDesc = image->GetDescriptor();
                RHI::Ptr<RHI::ImageView> imageView = RHI::Graphics::GetFactory().CreateImageView();
                RHI::ImageViewDescriptor imageViewDesc = RHI::ImageViewDescriptor::Create(imageDesc.m_format, 0, 0);
                imageView->Init(*image, imageViewDesc);

                pMaterial->SetProperty("HasSpecularPowerTexture", true);
                pMaterial->SetTexture("PerMaterial_SpecularPowerTexture", image, imageView);
            }

            if (material.GetTextureCount(aiTextureType_OPACITY) > 0 &&
                material.GetTexture(aiTextureType_OPACITY, 0, &aiTexturePath, nullptr, nullptr, &blendFactor, &aiBlendOperation) == aiReturn_SUCCESS)
            {
                TexMetadata metaData;
                ScratchImage scratchImage;
                std::string fullPath = rootPath + aiTexturePath.C_Str();
                DX12::DXAssertSuccess(LoadFromWICFile(DX12::s2ws(fullPath).c_str(), WIC_FLAGS_FORCE_RGB, &metaData, scratchImage));

                RHI::Ptr<RHI::Image> image = RHI::Graphics::GetFactory().CreateImage();
                ConstructTexture(image, metaData, scratchImage);
                
                RHI::ImageDescriptor imageDesc = image->GetDescriptor();
                RHI::Ptr<RHI::ImageView> imageView = RHI::Graphics::GetFactory().CreateImageView();
                RHI::ImageViewDescriptor imageViewDesc = RHI::ImageViewDescriptor::Create(imageDesc.m_format, 0, 0);
                imageView->Init(*image, imageViewDesc);

                pMaterial->SetProperty("HasOpacityTexture", true);
                pMaterial->SetTexture("PerMaterial_OpacityTexture", image, imageView);
            }

            // Load normal map texture.
            if (material.GetTextureCount(aiTextureType_NORMALS) > 0 &&
                material.GetTexture(aiTextureType_NORMALS, 0, &aiTexturePath) == aiReturn_SUCCESS)
            {
                TexMetadata metaData;
                ScratchImage scratchImage;
                std::string fullPath = rootPath + aiTexturePath.C_Str();
                DX12::DXAssertSuccess(LoadFromWICFile(DX12::s2ws(fullPath).c_str(), WIC_FLAGS_FORCE_RGB, &metaData, scratchImage));

                RHI::Ptr<RHI::Image> image = RHI::Graphics::GetFactory().CreateImage();
                ConstructTexture(image, metaData, scratchImage);
                
                RHI::ImageDescriptor imageDesc = image->GetDescriptor();
                RHI::Ptr<RHI::ImageView> imageView = RHI::Graphics::GetFactory().CreateImageView();
                RHI::ImageViewDescriptor imageViewDesc = RHI::ImageViewDescriptor::Create(imageDesc.m_format, 0, 0);
                imageView->Init(*image, imageViewDesc);

                pMaterial->SetProperty("HasNormalTexture", true);
                pMaterial->SetTexture("PerMaterial_NormalTexture", image, imageView);
            }
            // Load bump map (only if there is no normal map).
            else if (material.GetTextureCount(aiTextureType_HEIGHT) > 0 &&
                material.GetTexture(aiTextureType_HEIGHT, 0, &aiTexturePath, nullptr, nullptr, &blendFactor) == aiReturn_SUCCESS)
            {
                TexMetadata metaData;
                ScratchImage scratchImage;
                std::string fullPath = rootPath + aiTexturePath.C_Str();
                DX12::DXAssertSuccess(LoadFromWICFile(DX12::s2ws(fullPath).c_str(), WIC_FLAGS_FORCE_RGB, &metaData, scratchImage));

                RHI::Ptr<RHI::Image> image = RHI::Graphics::GetFactory().CreateImage();
                ConstructTexture(image, metaData, scratchImage);
                
                RHI::ImageDescriptor imageDesc = image->GetDescriptor();
                RHI::Ptr<RHI::ImageView> imageView = RHI::Graphics::GetFactory().CreateImageView();
                RHI::ImageViewDescriptor imageViewDesc = RHI::ImageViewDescriptor::Create(imageDesc.m_format, 0, 0);
                imageView->Init(*image, imageViewDesc);

                pMaterial->SetProperty("HasBumpTexture", true);
                pMaterial->SetTexture("PerMaterial_BumpTexture", image, imageView);
            }
            pMaterial->InitMaterialCbuff();
            pMaterial->InitMaterialSrg();
            m_materials.push_back(pMaterial);
		}

		void Model::ImportMesh(const aiMesh& mesh)
		{
            auto& rhiFactory = RHI::Graphics::GetFactory();

			std::shared_ptr<Mesh> pMesh = std::make_shared<Mesh>(mesh.mName.C_Str());
			assert(mesh.mMaterialIndex < m_materials.size());
            pMesh->SetMaterial(m_materials[mesh.mMaterialIndex]);

            RHI::InputStreamLayout inputStreamLayoutPacked = {};
            uint32_t streamBufferIdx = 0;
            if (mesh.HasPositions())
            {
                RHI::Ptr<RHI::Buffer> positions = rhiFactory.CreateBuffer();
                ConstructInputAssemblyBuffer(positions, &(mesh.mVertices[0].x), mesh.mNumVertices, sizeof(aiVector3D));
                RHI::StreamBufferView positionBufferView(*positions, 0, mesh.mNumVertices * sizeof(aiVector3D), sizeof(aiVector3D));
                inputStreamLayoutPacked.SetTopology(RHI::PrimitiveTopology::TriangleList);
                inputStreamLayoutPacked.AddStreamBuffer(RHI::StreamBufferDescriptor{ RHI::StreamStepFunction::PerVertex, 1, sizeof(aiVector3D) });
                inputStreamLayoutPacked.AddStreamChannel(RHI::StreamChannelDescriptor{ RHI::ShaderSemantic{"POSITION", 0}, RHI::Format::R32G32B32_FLOAT, 0, streamBufferIdx });
                pMesh->AddVertexBuffer(positions, positionBufferView);
                streamBufferIdx++;
            }

            if (mesh.HasNormals())
            {
                RHI::Ptr<RHI::Buffer> normals = rhiFactory.CreateBuffer();
                ConstructInputAssemblyBuffer(normals, &(mesh.mNormals[0].x), mesh.mNumVertices, sizeof(aiVector3D));
                RHI::StreamBufferView normalBufferView(*normals, 0, mesh.mNumVertices * sizeof(aiVector3D), sizeof(aiVector3D));
                inputStreamLayoutPacked.SetTopology(RHI::PrimitiveTopology::TriangleList);
                inputStreamLayoutPacked.AddStreamBuffer(RHI::StreamBufferDescriptor{ RHI::StreamStepFunction::PerVertex, 1, sizeof(aiVector3D) });
                inputStreamLayoutPacked.AddStreamChannel(RHI::StreamChannelDescriptor{ RHI::ShaderSemantic{"NORMAL", 0}, RHI::Format::R32G32B32_FLOAT, 0, streamBufferIdx });
                pMesh->AddVertexBuffer(normals, normalBufferView);
                streamBufferIdx++;
            }

            if (mesh.HasTangentsAndBitangents())
            {
                RHI::Ptr<RHI::Buffer> tangents = rhiFactory.CreateBuffer();
                ConstructInputAssemblyBuffer(tangents, &(mesh.mTangents[0].x), mesh.mNumVertices, sizeof(aiVector3D));
                RHI::StreamBufferView tangentBufferView(*tangents, 0, mesh.mNumVertices * sizeof(aiVector3D), sizeof(aiVector3D));
                inputStreamLayoutPacked.SetTopology(RHI::PrimitiveTopology::TriangleList);
                inputStreamLayoutPacked.AddStreamBuffer(RHI::StreamBufferDescriptor{ RHI::StreamStepFunction::PerVertex, 1, sizeof(aiVector3D) });
                inputStreamLayoutPacked.AddStreamChannel(RHI::StreamChannelDescriptor{ RHI::ShaderSemantic{"TANGENT", 0}, RHI::Format::R32G32B32_FLOAT, 0, streamBufferIdx });
                pMesh->AddVertexBuffer(tangents, tangentBufferView);
                streamBufferIdx++;

                RHI::Ptr<RHI::Buffer> bitangents = rhiFactory.CreateBuffer();
                ConstructInputAssemblyBuffer(bitangents, &(mesh.mBitangents[0].x), mesh.mNumVertices, sizeof(aiVector3D));
                RHI::StreamBufferView bitangentBufferView(*bitangents, 0, mesh.mNumVertices * sizeof(aiVector3D), sizeof(aiVector3D));
                inputStreamLayoutPacked.SetTopology(RHI::PrimitiveTopology::TriangleList);
                inputStreamLayoutPacked.AddStreamBuffer(RHI::StreamBufferDescriptor{ RHI::StreamStepFunction::PerVertex, 1, sizeof(aiVector3D) });
                inputStreamLayoutPacked.AddStreamChannel(RHI::StreamChannelDescriptor{ RHI::ShaderSemantic{"BINORMAL", 0}, RHI::Format::R32G32B32_FLOAT, 0, streamBufferIdx });
                pMesh->AddVertexBuffer(bitangents, bitangentBufferView);
                streamBufferIdx++;
            }

            for (unsigned int i = 0; mesh.HasTextureCoords(i); ++i)
            {
                switch (mesh.mNumUVComponents[i])
                {
                case 1: // 1-component texture coordinates (U)
                {

                    std::vector<float> texcoods1D(mesh.mNumVertices);
                    for (unsigned int j = 0; j < mesh.mNumVertices; ++j)
                    {
                        texcoods1D[j] = mesh.mTextureCoords[i][j].x;
                    }
                    RHI::Ptr<RHI::Buffer> texcoords = rhiFactory.CreateBuffer();
                    ConstructInputAssemblyBuffer(texcoords, texcoods1D.data(), (unsigned int)texcoods1D.size(), sizeof(float));
                    RHI::StreamBufferView texcoordBufferView(*texcoords, 0, (unsigned int)texcoods1D.size() * sizeof(float), sizeof(float));
                    inputStreamLayoutPacked.SetTopology(RHI::PrimitiveTopology::TriangleList);
                    inputStreamLayoutPacked.AddStreamBuffer(RHI::StreamBufferDescriptor{ RHI::StreamStepFunction::PerVertex, 1, sizeof(float) });
                    inputStreamLayoutPacked.AddStreamChannel(RHI::StreamChannelDescriptor{ RHI::ShaderSemantic{"TEXCOORD", 0}, RHI::Format::R32_FLOAT, 0, streamBufferIdx });
                    pMesh->AddVertexBuffer(texcoords, texcoordBufferView);
                    streamBufferIdx++;
                }
                break;
                case 2: // 2-component texture coordinates (U,V)
                {
                    std::vector<aiVector2D> texcoods2D(mesh.mNumVertices);
                    for (unsigned int j = 0; j < mesh.mNumVertices; ++j)
                    {
                        texcoods2D[j] = aiVector2D(mesh.mTextureCoords[i][j].x, mesh.mTextureCoords[i][j].y);
                    }
                    RHI::Ptr<RHI::Buffer> texcoords = rhiFactory.CreateBuffer();
                    ConstructInputAssemblyBuffer(texcoords, &(texcoods2D[0].x), (unsigned int)texcoods2D.size(), sizeof(aiVector2D));
                    RHI::StreamBufferView texcoordBufferView(*texcoords, 0, (unsigned int)texcoods2D.size() * sizeof(aiVector2D), sizeof(aiVector2D));
                    inputStreamLayoutPacked.SetTopology(RHI::PrimitiveTopology::TriangleList);
                    inputStreamLayoutPacked.AddStreamBuffer(RHI::StreamBufferDescriptor{ RHI::StreamStepFunction::PerVertex, 1, sizeof(aiVector2D) });
                    inputStreamLayoutPacked.AddStreamChannel(RHI::StreamChannelDescriptor{ RHI::ShaderSemantic{"TEXCOORD", 0}, RHI::Format::R32G32_FLOAT, 0, streamBufferIdx });
                    pMesh->AddVertexBuffer(texcoords, texcoordBufferView);
                    streamBufferIdx++;
                }
                break;
                case 3: // 3-component texture coordinates (U,V,W)
                {
                    std::vector<aiVector3D> texcoods3D(mesh.mNumVertices);
                    for (unsigned int j = 0; j < mesh.mNumVertices; ++j)
                    {
                        texcoods3D[j] = mesh.mTextureCoords[i][j];
                    }
                    RHI::Ptr<RHI::Buffer> texcoords = rhiFactory.CreateBuffer();
                    ConstructInputAssemblyBuffer(texcoords, &(texcoods3D[0].x), (unsigned int)texcoods3D.size(), sizeof(aiVector3D));
                    RHI::StreamBufferView texcoordBufferView(*texcoords, 0, (unsigned int)texcoods3D.size() * sizeof(aiVector3D), sizeof(aiVector3D));
                    inputStreamLayoutPacked.SetTopology(RHI::PrimitiveTopology::TriangleList);
                    inputStreamLayoutPacked.AddStreamBuffer(RHI::StreamBufferDescriptor{ RHI::StreamStepFunction::PerVertex, 1, sizeof(aiVector3D) });
                    inputStreamLayoutPacked.AddStreamChannel(RHI::StreamChannelDescriptor{ RHI::ShaderSemantic{"TEXCOORD", 0}, RHI::Format::R32G32B32_FLOAT, 0, streamBufferIdx });
                    pMesh->AddVertexBuffer(texcoords, texcoordBufferView);
                    streamBufferIdx++;
                }
                break;
                }
            }

            // Extract the index buffer.
            if (mesh.HasFaces())
            {
                std::vector<unsigned int> indices;
                indices.reserve(mesh.mNumFaces * 3);
                for (unsigned int i = 0; i < mesh.mNumFaces; i++)
                {
                    const aiFace& face = mesh.mFaces[i];
                    assert(face.mNumIndices == 3);
                    indices.push_back(face.mIndices[0]);
                    indices.push_back(face.mIndices[1]);
                    indices.push_back(face.mIndices[2]);
                }
                if (indices.size() > 0)
                {
                    RHI::Ptr<RHI::Buffer> indexBuffer = rhiFactory.CreateBuffer();
                    ConstructInputAssemblyBuffer(indexBuffer, indices.data(), indices.size(), sizeof(unsigned int));
                    RHI::IndexBufferView indexBufferView = RHI::IndexBufferView{
                        *indexBuffer,
                        0,
                        static_cast<uint32_t>(indices.size() * sizeof(unsigned int)),
                        RHI::IndexFormat::Uint32 };

                    pMesh->SetIndexBufferAndView(indexBuffer, indexBufferView, mesh.mNumFaces * 3);
                }
            }
            pMesh->SetInputStreamLayout(inputStreamLayoutPacked);
            m_meshes.push_back(pMesh);
		}

        std::shared_ptr<ModelNode> Model::ImportSceneNode(std::shared_ptr<ModelNode> parent, aiNode* aiNode)
        {
            if (!aiNode)
            {
                return nullptr;
            }

            // Assimp stores its matrices in row-major but GLM uses column-major.
            // We have to transpose the matrix before using it to construct a glm matrix.
            // The transformation relative to the node's parent.
            aiMatrix4x4 mat = aiNode->mTransformation;
            glm::mat4 localTransform(mat.a1, mat.b1, mat.c1, mat.d1,
                mat.a2, mat.b2, mat.c2, mat.d2,
                mat.a3, mat.b3, mat.c3, mat.d3,
                mat.a4, mat.b4, mat.c4, mat.d4);

            std::shared_ptr<ModelNode> pNode = std::make_shared<ModelNode>(localTransform);
            pNode->SetParent(parent);

            std::string nodeName(aiNode->mName.C_Str());
            if (!nodeName.empty())
            {
                pNode->SetName(nodeName);
            }

            // Add meshes to scene node also set the node reference to the mesh
            for (unsigned int i = 0; i < aiNode->mNumMeshes; ++i)
            {
                assert(aiNode->mMeshes[i] < m_meshes.size());
                std::shared_ptr<Mesh> pMesh = m_meshes[aiNode->mMeshes[i]];
                pNode->AddMesh(pMesh);
                pMesh->SetModelNode(pNode);
            }

            // Recursively Import children
            for (unsigned int i = 0; i < aiNode->mNumChildren; ++i)
            {
                std::shared_ptr<ModelNode> pChild = ImportSceneNode(pNode, aiNode->mChildren[i]);
                pNode->AddChild(pChild);
            }

            return pNode;
        }

        RHI::ResultCode Model::ConstructTexture(RHI::Ptr<RHI::Image> image, const DirectX::TexMetadata& metaData, const DirectX::ScratchImage& scratchImage)
        {
            const auto& imagePool = RHI::Graphics::GetImageSystem().GetSimpleImagePool();
            RHI::ResultCode result = RHI::ResultCode::Fail;

            // Init the image
            RHI::ImageInitRequest imageInitRequest;
            imageInitRequest.m_image = image.get();
            imageInitRequest.m_descriptor = RHI::ImageDescriptor::Create2D(RHI::ImageBindFlags::ShaderRead,
                static_cast<uint32_t>(metaData.width),
                static_cast<uint32_t>(metaData.width),
                DX12::ConvertFormat(metaData.format));
            result = imagePool->InitImage(imageInitRequest);
            assert(result == RHI::ResultCode::Success);

            // Update the image contents with the scratch image
            RHI::ImageUpdateRequest imageUpdateRequest;
            imageUpdateRequest.m_image = image.get();
            imageUpdateRequest.m_sourceSubresourceLayout = RHI::GetImageSubresourceLayout(image->GetDescriptor(), RHI::ImageSubresource{});
            imageUpdateRequest.m_sourceData = scratchImage.GetImages()[0].pixels;
            result = imagePool->UpdateImageContents(imageUpdateRequest);
            assert(result == RHI::ResultCode::Success);
            // [todo] Remember to call the resolve on the frame executer.

            return result;
        }

        RHI::ResultCode Model::ConstructInputAssemblyBuffer(RHI::Ptr<RHI::Buffer> buffer, const void* data, unsigned int count, unsigned int stride)
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

        void Model::BuildDrawList(std::vector<RHI::DrawItem>& drawItems, std::array<RHI::ShaderResourceGroup*, RHI::Limits::Pipeline::ShaderResourceGroupCountMax>& srgsToBind) const
        {
            m_root->BuildDrawList(drawItems, srgsToBind);
        }

        void Model::SpawnImGuiWindow()
        {
            const auto updateChange = [this](bool check) {m_dirty = check || m_dirty; };

            ImGui::Text("Position");
            updateChange(ImGui::SliderFloat("Pos X", &m_rootTranslation.x, -80.0f, 80.0f, "%.1f"));
            updateChange(ImGui::SliderFloat("Pos Y", &m_rootTranslation.y, -80.0f, 80.0f, "%.1f"));
            updateChange(ImGui::SliderFloat("Pos Z", &m_rootTranslation.z, -80.0f, 80.0f, "%.1f"));

            ImGui::Text("Scale");
            updateChange(ImGui::SliderFloat("Scale X", &m_rootScale.x, 0.001f, 10.0f, "%.3f"));
            updateChange(ImGui::SliderFloat("Scale Y", &m_rootScale.y, 0.001f, 10.0f, "%.3f"));
            updateChange(ImGui::SliderFloat("Scale Z", &m_rootScale.z, 0.001f, 10.0f, "%.3f"));

            ImGui::Text("Orientation");
            updateChange(ImGui::SliderFloat("Roll", &m_currentRotation.z, 0.995f * -180.0f, 0.995f * 180.0f));
            updateChange(ImGui::SliderFloat("Pitch", &m_currentRotation.x, 0.995f * -180.0f, 0.995f * 180.0f));
            updateChange(ImGui::SliderFloat("Yaw", &m_currentRotation.y, 0.995f * -180.0f, 0.995f * 180.0f));
            
            m_root->SpawnImGuiWindow();
        }

        void Model::Update()
        {
            if (m_dirty)
            {
                glm::vec3 delta = m_previousRotation - m_currentRotation;
                if (delta.x > glm::epsilon<float>() || delta.x < glm::epsilon<float>())
                {
                    m_rootRotation = glm::angleAxis(glm::radians(delta.x), m_rootRotation * glm::vec3(1, 0, 0)) * m_rootRotation;
                }
                if (delta.y > glm::epsilon<float>() || delta.y < glm::epsilon<float>())
                {
                    m_rootRotation = glm::angleAxis(glm::radians(delta.y), m_rootRotation * glm::vec3(0, 1, 0)) * m_rootRotation;
                }
                if (delta.z > glm::epsilon<float>() || delta.z < glm::epsilon<float>())
                {
                    m_rootRotation = glm::angleAxis(glm::radians(delta.z), m_rootRotation * glm::vec3(0, 0, 1)) * m_rootRotation;
                }
                m_previousRotation = m_currentRotation;

                glm::mat4 translateMatrix = glm::translate(glm::mat4(1.0), m_rootTranslation);
                glm::mat4 rotationMatrix = glm::toMat4(m_rootRotation);
                glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0), m_rootScale);
                glm::mat4 worldTransform = (translateMatrix * (rotationMatrix * scaleMatrix));

                m_root->SetWorldTransform(worldTransform);
            }
            m_dirty = false;
            m_root->Update();
            m_root->ClearDirtyFlag();
        }
	}
}