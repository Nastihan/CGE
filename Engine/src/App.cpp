#include "App.h"
#include "RHI/Limits.h"
#include "DX_Interface/DX_ImguiManager.h"
#include "imgui/imgui.h"

namespace CGE
{
	App::App() : window(RHI::Limits::Device::ClientWidth, RHI::Limits::Device::ClientHeight, "CGE"), gfx("DX12", window) 
	{
		window.InitImgui();
		m_scene = std::make_shared<Scene::Scene>();
		m_scene->Init(gfx.GetFrameGraphExecuter()->GetForwardPass());
		// m_scene->LoadModel("nano_textured\\nanosuit.obj", gfx.GetFrameGraphExecuter()->GetForwardPass());
		// m_scene->LoadModel("suzanne\\suzanne.obj", gfx.GetFrameGraphExecuter()->GetForwardPass());
		m_scene->LoadModel("lord_inquisitor_servo_skull_gltf\\scene.gltf", gfx.GetFrameGraphExecuter()->GetForwardPass());
		// m_scene->LoadModel("demon_skull_ring_gltf\\scene.gltf", gfx.GetFrameGraphExecuter()->GetForwardPass());
		gfx.GetFrameGraphExecuter()->GetForwardPass()->SetScenePtr(m_scene);
		RegisterKeyboardEventCallback(m_scene->GetCamera().GetKeyPressedFunctionBindable());
		RHI::Graphics::GetImguiManager().PushSpawnableWindow(std::bind(&Scene::Camera::SpawnCameraImGuiWindow, &m_scene->GetCamera()));
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

		m_scene->GetCamera().Update();
	}

	void App::RegisterKeyboardEventCallback(boost::function<void(KeyEventArgs&, UpdateEventArgs&)> functionType)
	{
		m_keyPressed += functionType;
	}
}