#pragma once
#include "RHI/Graphics.h"
#include "MeshBuilder.h"
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
		void Update();
		void SetGLFWUserPointer();
	private:
		Window window;
		RHI::Graphics gfx;
		MeshBuilder m_meshBuilder;
		uint64_t currentFrame = 0;
	};
}