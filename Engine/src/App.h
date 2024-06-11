#pragma once
#include "RHI/Graphics.h"
#include "Window.h"
#include "ImguiManager.h"
#include "Scene/Scene.h"

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
		uint64_t currentFrame = 0;
		std::shared_ptr<Scene::Scene> m_scene;
	};
}