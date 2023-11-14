#pragma once
#include <GLFW/glfw3.h>
#include <memory>
#include <string>
#include <Windows.h>

namespace CGE
{
	static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);
	class Window
	{
	public:
		Window(uint16_t width, uint16_t height, std::string title);
		~Window();
		GLFWwindow& Wnd();
		HWND& Hwnd();
		void EnableCursor();
		void DisableCursor();
		bool CursorEnabled();

	private:
		bool cursorEnabled = true;
		float lastMouseX;
		float lastMouseY;
		GLFWwindow* pWindow;
		// [todo] remove for VK backend
		HWND hwnd;
		RECT windowRect;
		uint16_t width;
		uint16_t height;
	};
}