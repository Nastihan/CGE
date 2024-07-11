#pragma once

#include <GLFW/glfw3.h>
#include <memory>
#include <string>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "Events.h"

// The min/max macros conflict with like-named member functions.
// Only use std::min and std::max defined in <algorithm>.
#if defined(min)
#undef min
#endif

#if defined(max)
#undef max
#endif

namespace CGE
{
	// static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);
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
		bool GetResizeFlag() const;
		void ResetResizeFlag();
		void InitImgui();

		std::vector<KeyEventArgs>& GetKeyPresses();

		// [todo] handle full screen borderless window
		static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);
		static void KeyPressedCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
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
		bool m_resizeFlag = false;

		std::vector<KeyEventArgs> m_keyPresses;
	};
}