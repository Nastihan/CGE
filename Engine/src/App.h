#pragma once
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
	};
}