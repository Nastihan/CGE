
// Scene
#include "Scene.h"
#include "Model.h"

// Pass
#include "../Pass/ForwardPass.h"

// RHI
#include "../RHI/BufferPool.h"
#include "../RHI/BufferViewDescriptor.h"
#include "../RHI/Graphics.h"

namespace CGE
{
	namespace Scene
	{
		void Scene::Init(Pass::ForwardPass* pForwardPass)
		{
			for (size_t i = 0; i < 1; i++)
			{
				Light light;
				light.m_Type = Light::LightType::Directional;
				light.m_DirectionVS = glm::vec4(0.0, 0.0, -1.0, 0.0);
				light.m_Intensity = 1.5;
				m_lights.push_back(light);
			}

			auto& rhiFactory = RHI::Graphics::GetFactory();

			// Construct the light buffer. For now lets add all the lights before this point.
			// If we need to dynamically add more at runtime I need to invalidate this buffer and its view and reconstruct the whole thing.
			m_lightBuffer = rhiFactory.CreateBuffer();
			m_lightBufferView = rhiFactory.CreateBufferView();
			const auto& readOnlyBufferPool = RHI::Graphics::GetBufferSystem().GetCommonBufferPool(RHI::CommonBufferPoolType::ReadOnly);

			// Create light buffer
			RHI::ResultCode result = RHI::ResultCode::Fail;
			RHI::BufferInitRequest lightBufferInitRequest;
			lightBufferInitRequest.m_buffer = m_lightBuffer.get();
			lightBufferInitRequest.m_descriptor.m_byteCount = m_lights.size() * sizeof(Light);
			lightBufferInitRequest.m_descriptor.m_bindFlags = RHI::BufferBindFlags::ShaderRead;
			lightBufferInitRequest.m_initialData = m_lights.data();
			result = readOnlyBufferPool->InitBuffer(lightBufferInitRequest);
			assert(result == RHI::ResultCode::Success);

			// Create light buffer view
			RHI::BufferViewDescriptor lightBufferViewDescriptor = RHI::BufferViewDescriptor::CreateStructured(0, m_lights.size(), sizeof(Light));
			m_lightBufferView->Init(*m_lightBuffer, lightBufferViewDescriptor);

			// [todo] This is a bad way to retrieve the srg I'm hardcoding the index it got pushed in the ForwardPass::Init method.
			const RHI::ShaderResourceGroupLayout* sceneSrgLayout = pForwardPass->GetPipelineStateDescriptorForDraw().m_pipelineLayoutDescriptor->GetShaderResourceGroupLayout(0);
			m_sceneSrg = rhiFactory.CreateShaderResourceGroup();
			RHI::ShaderResourceGroupData sceneSrgData(sceneSrgLayout);
			RHI::ShaderInputBufferIndex lightBufferInputIdx(0);
			sceneSrgData.SetBufferView(lightBufferInputIdx, m_lightBufferView.get(), 0);
			m_sceneSrg->Init(m_lightBufferView->GetDevice(), sceneSrgData);
			m_sceneSrg->Compile();


			// m_camera.SetViewport(Viewport(0, 0, g_Config.WindowWidth, g_Config.WindowHeight));
			m_camera.SetTranslation(glm::vec3(0.0, 0.0, 3.0));
			m_camera.SetRotation(glm::quat(glm::vec3(glm::radians(90.0), glm::radians(180.0), glm::radians(0.0))));
			m_camera.SetProjectionRH(45.0f, RHI::Limits::Device::ClientWidth / (float)RHI::Limits::Device::ClientHeight, 0.1f, 1000.0f);
		}

		void Scene::AddLight(const Light& light)
		{
			m_lights.push_back(light);
		}

		void Scene::LoadModel(const std::string& pathString, Pass::ForwardPass* pForwardPass)
		{
			m_models.emplace_back();
			m_models.back().LoadFromFile(pathString, pForwardPass);
		}

		void Scene::Render(Pass::ForwardPass* pForwardPass, RHI::CommandList* commandList)
		{
			pForwardPass->PushSrg(m_sceneSrg.get());
			for (const auto& model : m_models)
			{
				model.Render(pForwardPass, m_camera, commandList);
			}
		}

		Camera& Scene::GetCamera()
		{
			return m_camera;
		}
	}
}