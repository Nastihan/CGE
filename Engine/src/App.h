#pragma once
#include "RHI/Graphics.h"
#include "Window.h"

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
		RHI::Graphics gfx;
	};
}