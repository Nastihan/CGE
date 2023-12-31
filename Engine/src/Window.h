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
		GLFWwindow& GetWnd() const;
		const HWND& GetHwnd() const;
		void EnableCursor();
		void DisableCursor();
		bool CursorEnabled();
		uint16_t GetWidth() const;
		uint16_t GetHeight() const;

	private:
		bool m_cursorEnabled = true;
		float m_lastMouseX;
		float m_lastMouseY;
		GLFWwindow* m_pWindow;
		// [todo] remove for VK backend
		HWND m_hwnd;
		RECT m_windowRect;
		uint16_t m_width;
		uint16_t m_height;
	};
}