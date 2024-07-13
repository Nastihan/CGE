
// Scene
#include "Scene.h"
#include "Model.h"

// Pass
#include "../Pass/ForwardPass.h"

// RHI
#include "../RHI/BufferPool.h"
#include "../RHI/BufferViewDescriptor.h"
#include "../RHI/Graphics.h"

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"

#include "FixedShapes.h"

namespace CGE
{
	namespace Scene
	{
		void Scene::Init()
		{
			for (size_t i = 0; i < 1; i++)
			{
				Light light;
				light.m_type = Light::LightType::Directional;
				light.m_directionWS = glm::vec4(0.0, 0.0, 1.0, 0.0);
				light.m_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
				light.m_intensity = 1.0;
				m_lights.push_back(light);

				//Light light;
				light.m_type = Light::LightType::Directional;
				light.m_directionWS = glm::vec4(0.0, 0.0, -1.0, 0.0);
				light.m_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
				light.m_intensity = 1.0;
				m_lights.push_back(light);

				//Light light;
				light.m_type = Light::LightType::Directional;
				light.m_directionWS = glm::vec4(1.0, 0.0, 0.0, 0.0);
				light.m_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
				light.m_intensity = 1.0;
				m_lights.push_back(light);

				//Light light;
				light.m_type = Light::LightType::Directional;
				light.m_directionWS = glm::vec4(-1.0, 0.0, 0.0, 0.0);
				light.m_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
				light.m_intensity = 1.0;
				m_lights.push_back(light);

				light.m_type = Light::LightType::Directional;
				light.m_directionWS = glm::vec4(0.0, 1.0, 0.0, 0.0);
				light.m_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
				light.m_intensity = 1.0;
				m_lights.push_back(light);

				light.m_type = Light::LightType::Directional;
				light.m_directionWS = glm::vec4(0.0, -1.0, 0.0, 0.0);
				light.m_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
				light.m_intensity = 1.0;
				m_lights.push_back(light);

				light.m_type = Light::LightType::Point;
				light.m_positionWS = glm::vec4(5.5, 2.4, 1.4, 1.0);
				light.m_color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
				light.m_intensity = 1.0;
				light.m_range = 2.0;
				m_lights.push_back(light);

				light.m_type = Light::LightType::Point;
				light.m_positionWS = glm::vec4(5.5, 2.4, -0.6, 1.0);
				light.m_color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
				light.m_intensity = 1.0;
				light.m_range = 2.0;
				m_lights.push_back(light);

				light.m_type = Light::LightType::Point;
				light.m_positionWS = glm::vec4(5.5, 4.4, 1.4, 1.0);
				light.m_color = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
				light.m_intensity = 1.0;
				light.m_range = 2.0;
				m_lights.push_back(light);

				light.m_type = Light::LightType::Point;
				light.m_positionWS = glm::vec4(5.5, 4.4, -0.6, 1.0);
				light.m_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
				light.m_intensity = 1.0;
				light.m_range = 2.0;
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

			const RHI::ShaderPermutation& specularGlossiness_Shader = *RHI::Graphics::GetAssetProcessor().GetShaderPermutation("SpecularGlossiness_Shader");
			const RHI::ShaderResourceGroupLayout* sceneSrgLayout = specularGlossiness_Shader.m_pipelineLayoutDescriptor->GetShaderResourceGroupLayout(RHI::ShaderResourceGroupType::Scene);
			m_sceneSrg = rhiFactory.CreateShaderResourceGroup();
			RHI::ShaderResourceGroupData sceneSrgData(sceneSrgLayout);
			RHI::ShaderInputBufferIndex lightBufferInputIdx = sceneSrgLayout->FindShaderInputBufferIndex("PerScene_Lights");
			sceneSrgData.SetBufferView(lightBufferInputIdx, m_lightBufferView.get(), 0);
			m_sceneSrg->Init(m_lightBufferView->GetDevice(), sceneSrgData);
			m_sceneSrg->Compile();

			RHI::Graphics::GetImguiManager().PushSpawnableWindow(std::bind(&Scene::SpawnImGuiWindow, this));

			// m_camera.SetViewport(Viewport(0, 0, g_Config.WindowWidth, g_Config.WindowHeight));
			// m_camera.SetTranslation(glm::vec3(0.0, 0.0, 3.0));
			// m_camera.SetRotation(glm::quat(glm::vec3(glm::radians(90.0), glm::radians(180.0), glm::radians(0.0))));
			// m_camera.SetProjectionRH(45.0f, RHI::Limits::Device::ClientWidth / (float)RHI::Limits::Device::ClientHeight, 0.1f, 1000.0f);
		}

		void Scene::AddLight(const Light& light)
		{
			m_lights.push_back(light);
			m_needLightUpdate = true;
		}

		void Scene::LoadModel(const std::string& pathString, const std::string& modelName, glm::vec3 pos, glm::vec3 scale, glm::quat rot)
		{
			m_models.emplace_back(modelName, pos, scale, rot);
			m_models.back().LoadFromFile(pathString, modelName);
		}

		void Scene::AddShape(std::shared_ptr<Shape> shape)
		{
			m_shapes.push_back(shape);
		}

		Camera& Scene::GetCamera()
		{
			return m_camera;
		}

		void Scene::SpawnLightImGuiWindow()
		{
			bool lightUpdated = false;
			const auto updateChange = [this](bool check) {m_needLightUpdate = check || m_needLightUpdate; };

			if (ImGui::Begin("Light Editor"))
			{

				// Create the drop-down list for selecting a light
				std::vector<std::string> lightNames;
				for (size_t i = 0; i < m_lights.size(); ++i)
				{
					lightNames.push_back("Light " + std::to_string(i + 1));
				}

				// Convert vector to array of const char*
				std::vector<const char*> lightNamesCStr;
				for (const auto& name : lightNames)
				{
					lightNamesCStr.push_back(name.c_str());
				}

				// Drop-down list
				ImGui::Combo("Select Light", &m_currentSelectedLight, lightNamesCStr.data(), lightNamesCStr.size());
				// Display and edit the properties of the selected light
				if (m_currentSelectedLight >= 0 && m_currentSelectedLight < m_lights.size())
				{
					Light& selectedLight = m_lights[m_currentSelectedLight];

					ImGui::Text("Light %d", m_currentSelectedLight + 1);

					switch (selectedLight.m_type)
					{
					case Light::LightType::Directional:
					{
						updateChange(ImGui::DragFloat4("Direction WS", &selectedLight.m_directionWS[0]));
						updateChange(ImGui::ColorEdit4("Color", &selectedLight.m_color[0]));
						updateChange(ImGui::DragFloat("Intensity", &selectedLight.m_intensity));
						updateChange(ImGui::Checkbox("Enabled", (bool*)&selectedLight.m_enabled));
						updateChange(ImGui::Checkbox("Selected", (bool*)&selectedLight.m_selected));
						break;
					}

					case Light::LightType::Point:
					{
						updateChange(ImGui::DragFloat4("Position WS", &selectedLight.m_positionWS[0]));
						updateChange(ImGui::ColorEdit4("Color", &selectedLight.m_color[0]));
						updateChange(ImGui::DragFloat("Range", &selectedLight.m_range));
						updateChange(ImGui::DragFloat("Intensity", &selectedLight.m_intensity));
						updateChange(ImGui::Checkbox("Enabled", (bool*)&selectedLight.m_enabled));
						updateChange(ImGui::Checkbox("Selected", (bool*)&selectedLight.m_selected));
						break;
					}
					case Light::LightType::Spot:
					{
						updateChange(ImGui::DragFloat4("Direction WS", &selectedLight.m_directionWS[0]));
						updateChange(ImGui::DragFloat4("Position WS", &selectedLight.m_positionWS[0]));
						updateChange(ImGui::ColorEdit4("Color", &selectedLight.m_color[0]));
						updateChange(ImGui::DragFloat("Spotlight Angle", &selectedLight.m_spotlightAngle));
						updateChange(ImGui::DragFloat("Range", &selectedLight.m_range));
						updateChange(ImGui::DragFloat("Intensity", &selectedLight.m_intensity));
						updateChange(ImGui::Checkbox("Enabled", (bool*)&selectedLight.m_enabled));
						updateChange(ImGui::Checkbox("Selected", (bool*)&selectedLight.m_selected));
						break;
					}
					default:
						break;
					}
				}
			}
			ImGui::End();
		}

		void Scene::SpawnModelsImGuiWindow()
		{
			if (ImGui::Begin("Loaded Models"))
			{
				std::vector<std::string> modelNames;
				for (size_t i = 0; i < m_models.size(); i++)
				{
					modelNames.push_back(m_models[i].GetName());
				}

				// Convert vector to array of const char*
				std::vector<const char*> modelNamesCStr;
				for (const auto& name : modelNames)
				{
					modelNamesCStr.push_back(name.c_str());
				}

				// Drop-down list
				ImGui::Combo("Select Model", &m_currentSelectedModel, modelNamesCStr.data(), modelNamesCStr.size());
				if (m_currentSelectedModel >= 0 && m_currentSelectedModel < m_models.size())
				{
					m_models[m_currentSelectedModel].SpawnImGuiWindow();
				}
			}
			ImGui::End();
		}

		void Scene::SpawnImGuiWindow()
		{
			m_camera.SpawnCameraImGuiWindow();

			if (!m_lights.empty())
			{
				SpawnLightImGuiWindow();
			}

			if (!m_models.empty())
			{
				SpawnModelsImGuiWindow();
			}
		}

		void Scene::Update()
		{
			m_camera.Update();
			if (m_needLightUpdate)
			{
				UpdateLightBuffer();
				m_needLightUpdate = false;
			}

			for (auto& model : m_models)
			{
				model.Update();
			}
		}

		RHI::ResultCode Scene::UpdateLightBuffer()
		{
			const auto& readOnlyBufferPool = RHI::Graphics::GetBufferSystem().GetCommonBufferPool(RHI::CommonBufferPoolType::ReadOnly);

			RHI::BufferMapRequest mapRequest{};
			mapRequest.m_buffer = m_lightBuffer.get();
			mapRequest.m_byteCount = sizeof(Light);
			mapRequest.m_byteOffset = sizeof(Light) * m_currentSelectedLight;

			RHI::BufferMapResponse mapResponse{};

			RHI::ResultCode mapSuccess = readOnlyBufferPool->MapBuffer(mapRequest, mapResponse);
			if (mapSuccess == RHI::ResultCode::Success)
			{
				memcpy(mapResponse.m_data, m_lights.data() + m_currentSelectedLight /* or &m_lights[m_currentSelectedLight] */, sizeof(Light));
				readOnlyBufferPool->UnmapBuffer(*m_lightBuffer);
			}
			return mapSuccess;
		}

		std::vector<RHI::DrawItem> Scene::BuildDrawList()
		{
			std::vector<RHI::DrawItem> drawItems;
			std::array<RHI::ShaderResourceGroup*, RHI::Limits::Pipeline::ShaderResourceGroupCountMax> srgsToBind;
			srgsToBind.fill(nullptr);
			srgsToBind[RHI::SrgBindingSlot::Scene] = m_sceneSrg.get();
			srgsToBind[RHI::SrgBindingSlot::View] = m_camera.GetCameraSrg();
			for (const auto& model : m_models)
			{
				model.BuildDrawList(drawItems, srgsToBind);
			}
			for (const auto& shape : m_shapes)
			{
				shape->BuildDrawList(drawItems, srgsToBind);
			}
			return drawItems;
		}
	}
}