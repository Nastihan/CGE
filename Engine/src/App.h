#pragma once

// RHI
#include "RHI/Graphics.h"

// Scene
#include "Scene/Scene.h"

#include "Window.h"
#include "ImguiManager.h"
#include "Events.h"
#include "Timer.h"

namespace CGE
{
	class App
	{
	public:
		App();
		~App();
		void Run();
		void Update();

	private:
		void RegisterKeyboardEventCallback(boost::function<void(KeyEventArgs&, UpdateEventArgs&)> functionType);
		void RegisterMouseEventCallback(boost::function<void(KeyEventArgs&, UpdateEventArgs&)> functionType);

	private:
		Timer m_timer;
		Window window;
		RHI::Graphics gfx;
		uint64_t currentFrame = 0;
		std::shared_ptr<Scene::Scene> m_scene;
		
		KeyboardEvent m_keyPressed;
	};
}