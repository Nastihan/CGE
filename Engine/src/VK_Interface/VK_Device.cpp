#include "VK_Device.h"
#include <GLFW/glfw3.h>

namespace CGE
{
   
    VK_Device::VK_Device()
    {
        InitInstance();
    }

    VK_Device::~VK_Device()
    {

        LOG_CONSOLE(LogLevel::Info, "Cleaned up resources.");
    }
    
    void VK_Device::InitInstance()
    {
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "No app";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 3, 0);
        appInfo.pEngineName = "CGE engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        auto extensions = GetRequiredExtensions();
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();
        createInfo.enabledLayerCount = 0;

        ThrowIfFailed(vkCreateInstance(&createInfo, nullptr, &instance));
        LOG_CONSOLE(LogLevel::Info, "Vulkan instance created");
        

    }

    std::vector<const char*> VK_Device::GetRequiredExtensions()
    {
        // [todo] fix the issue regarding glfwGetRequiredInstanceExtensions()
        /*uint32_t glfwExtensionCount;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);*/

        std::vector<const char*> extensions{};
        extensions.push_back("VK_KHR_SURFACE");
        extensions.push_back("VK_KHR_win32_surface");
        //extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

        return extensions;
    }

    const VK_Device& VK_Device::GetInstance()
    {
        return VKDeviceInstance;
    }

    VK_Device VK_Device::VKDeviceInstance;
}