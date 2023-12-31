#include "Window.h"
// [todo] have to check backend API
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

namespace CGE
{
    void FramebufferResizeCallback(GLFWwindow* window, int width, int height) {}
    Window::Window(uint16_t width, uint16_t height, std::string title) : m_width{ width }, m_height{ height }
	{
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		m_pWindow = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
        glfwSetFramebufferSizeCallback(m_pWindow, FramebufferResizeCallback);
        // [todo] remove for VK backend
        m_hwnd = glfwGetWin32Window(m_pWindow);
	}
	Window::~Window()
	{
		glfwDestroyWindow(m_pWindow);
		glfwTerminate();
	}

    GLFWwindow& Window::GetWnd() const
    {
        return *m_pWindow;
    }

    const HWND& Window::GetHwnd() const
    {
        return m_hwnd;
    }

    void Window::EnableCursor()
    {
        m_cursorEnabled = true;
        glfwSetInputMode(m_pWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    void Window::DisableCursor()
    {
        m_cursorEnabled = false;
        glfwSetInputMode(m_pWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    bool Window::CursorEnabled()
    {
        return m_cursorEnabled;
    }

    uint16_t Window::GetWidth() const
    {
        return m_width;
    }

    uint16_t Window::GetHeight() const
    {
        return m_height;
    }
}