#pragma once
#include "RHI/Graphics.h"
#include "Window.h"

// some comment
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
		uint64_t currentFrame = 0;
	};
}