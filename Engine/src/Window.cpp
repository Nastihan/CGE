#include "Window.h"
// [todo] have to check backend API
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

namespace CGE
{
    void FramebufferResizeCallback(GLFWwindow* window, int width, int height) {}
	Window::Window(uint16_t width, uint16_t height, std::string title)
	{
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		pWindow = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
        glfwSetFramebufferSizeCallback(pWindow, FramebufferResizeCallback);
        // [todo] remove for VK backend
        hwnd = glfwGetWin32Window(pWindow);
	}
	Window::~Window()
	{
		glfwDestroyWindow(pWindow);
		glfwTerminate();
	}

    GLFWwindow& Window::Wnd()
    {
        return *pWindow;
    }

    HWND& Window::Hwnd()
    {

        return hwnd;
    }

    void Window::EnableCursor()
    {
        cursorEnabled = true;
        glfwSetInputMode(pWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    void Window::DisableCursor()
    {
        cursorEnabled = false;
        glfwSetInputMode(pWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    bool Window::CursorEnabled()
    {
        return cursorEnabled;
    }
}