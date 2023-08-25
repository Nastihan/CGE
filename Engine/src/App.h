#pragma once
#include "Window.h"
#include "Graphics.h"

namespace CGE
{
	class App
	{
	public:
		App();
		~App();
		void Run();
	private:
		Window window;
		Graphics gfx;
	};
}