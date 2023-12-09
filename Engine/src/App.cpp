#include "App.h"
#include "RHI/Limits.h"

namespace CGE
{
	App::App() : window(RHI::Limits::Device::ClientWidth, RHI::Limits::Device::ClientHeight, "CGE"), gfx("DX12", window) {}
	App::~App() {}
	void App::Run()
	{
		while (!glfwWindowShouldClose(&window.GetWnd()))
		{
			glfwPollEvents();
		}
	}
}