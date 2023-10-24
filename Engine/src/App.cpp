#include "App.h"

namespace CGE
{
	App::App() : window(800, 600, "CGE"), gfx("DX12") {}
	App::~App() {}
	void App::Run()
	{
		while (!glfwWindowShouldClose(&window.Wnd()))
		{
			glfwPollEvents();
		}
	}
}