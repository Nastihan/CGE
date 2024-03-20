#include "App.h"
#include "RHI/Limits.h"

namespace CGE
{
	App::App() : window(RHI::Limits::Device::ClientWidth, RHI::Limits::Device::ClientHeight, "CGE"), gfx("DX12", window), m_meshBuilder(gfx.GetBufferSystem()) {}
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