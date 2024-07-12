
#include "App.h"

// RHI
#include "RHI/Limits.h"

// DX12
#include "DX_Interface/DX_ImguiManager.h"

// Scene
#include "Scene/FixedShapes.h"

#include "imgui/imgui.h"

namespace CGE
{
	App::App() : window(RHI::Limits::Device::ClientWidth, RHI::Limits::Device::ClientHeight, "CGE"), gfx("DX12", window) 
	{
		window.InitImgui();
		m_scene = std::make_shared<Scene::Scene>();
		m_scene->Init();
		// m_scene->LoadModel("nano_textured\\nanosuit.obj", "Nano");
		// m_scene->LoadModel("suzanne\\suzanne.obj", "Suzanne");
		// m_scene->LoadModel("lord_inquisitor_servo_skull_gltf\\scene.gltf", "Skull");
		// m_scene->LoadModel("demon_skull_ring_gltf\\scene.gltf", "Skull Ring");
		m_scene->LoadModel("Sponza\\glTF\\Sponza.gltf", "Sponza");
		gfx.GetFrameGraphExecuter()->GetForwardPass()->SetScenePtr(m_scene);
		RegisterKeyboardEventCallback(m_scene->GetCamera().GetKeyPressedFunctionBindable());
		// RHI::Graphics::GetImguiManager().PushSpawnableWindow(std::bind(&Scene::Camera::SpawnCameraImGuiWindow, &m_scene->GetCamera()));

		std::shared_ptr<Scene::Shape> box = std::make_shared<Scene::Box>(
			glm::vec3(0.0, 0.0, 5.0)
			, glm::vec3(1.0, 1.0, 1.0)
			, glm::quat(glm::vec3(glm::radians(0.0), glm::radians(0.0), glm::radians(0.0)))
			, RHI::Graphics::GetAssetProcessor().GetMaterial("BrickCube_Material"));
		
		m_scene->AddShape(box);
	}
	
	App::~App() {}

	void App::Run()
	{
		while (!glfwWindowShouldClose(&window.GetWnd()))
		{
			glfwPollEvents();
			if (window.GetResizeFlag())
			{
				gfx.RecreateSwapChain();
			}
			Update();
			gfx.Render();
		}
	}

	void App::Update()
	{
		float deltaTime = m_timer.Mark();
		float applicationTime = m_timer.GetAppTime();
		UpdateEventArgs updateEventArgs(deltaTime, applicationTime);
		
		std::vector<KeyEventArgs>& keyPresses = window.GetKeyPresses();
		for (auto& keyPress : keyPresses)
		{
			m_keyPressed(keyPress, updateEventArgs);
		}
		keyPresses.clear();

		m_scene->Update();
	}

	void App::RegisterKeyboardEventCallback(boost::function<void(KeyEventArgs&, UpdateEventArgs&)> functionType)
	{
		m_keyPressed += functionType;
	}
}