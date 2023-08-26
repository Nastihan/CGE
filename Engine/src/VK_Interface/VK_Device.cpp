#include "VK_Device.h"
#include <GLFW/glfw3.h>

namespace CGE
{
   
    VK_Device::~VK_Device()
    {
    }

    VK_Device::VK_Device()
    {
        InitInstance();
    }

    void VK_Device::InitInstance()
    {
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "No app";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "CGE engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions{};

        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        createInfo.enabledExtensionCount = glfwExtensionCount;
        createInfo.ppEnabledExtensionNames = glfwExtensions;
        createInfo.enabledLayerCount = 0;

        ThrowIfFailed(vkCreateInstance(&createInfo, nullptr, &instance));

    }

    const VK_Device& VK_Device::GetInstance()
    {
        return VKDeviceInstance;
    }

    VK_Device VK_Device::VKDeviceInstance;
}