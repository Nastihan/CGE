#include "App.h"
#include "RHI/Limits.h"
#include "DX_Interface/DX_ImguiManager.h"
#include "imgui/imgui.h"

#include <boost/signals2.hpp>

namespace CGE
{
	App::App() : window(RHI::Limits::Device::ClientWidth, RHI::Limits::Device::ClientHeight, "CGE"), gfx("DX12", window) 
	{
		window.InitImgui();
		m_scene = std::make_shared<Scene::Scene>();
		m_scene->Init(gfx.GetFrameGraphExecuter()->GetForwardPass());
		m_scene->LoadModel("nano_textured\\nanosuit.obj", gfx.GetFrameGraphExecuter()->GetForwardPass());
		gfx.GetFrameGraphExecuter()->GetForwardPass()->SetScenePtr(m_scene);
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
			// Update();
			gfx.Render();
		}
	}
}