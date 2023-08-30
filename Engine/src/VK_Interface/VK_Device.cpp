#include "VK_Device.h"
#include <GLFW/glfw3.h>
#include <map>

namespace CGE
{
   
    VK_Device::VK_Device()
    {
        InitDevice();
    }

    VK_Device::~VK_Device()
    {
        vkDestroyInstance(instance, nullptr);
        LOG_CONSOLE(LogLevel::Info, "Cleaned up resources.");
    }
    
    void VK_Device::InitDevice()
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

        physicalDevice = PickDevice();
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
        LOG_CONSOLE(LogLevel::Info, deviceProperties.deviceName);

    }

    std::vector<const char*> VK_Device::GetRequiredExtensions()
    {
        // [todo] fix the issue regarding glfwGetRequiredInstanceExtensions()
        /*uint32_t glfwExtensionCount;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);*/

        std::vector<const char*> extensions{};
        extensions.push_back("VK_KHR_surface");
        extensions.push_back("VK_KHR_win32_surface");
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

        return extensions;
    }

    VkPhysicalDevice VK_Device::PickDevice()
    {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
        if (deviceCount == 0) {
            throw std::runtime_error("failed to find GPUs with Vulkan support!");
        }
        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

        // Use an ordered map to automatically sort candidates by increasing score
        std::multimap<int, VkPhysicalDevice> candidates;

        for (const auto& device : devices) {
            int score = RateDevice(device);
            candidates.insert(std::make_pair(score, device));
        }

        // Check if the best candidate is suitable at all
        if (candidates.rbegin()->first > 0) {
            return candidates.rbegin()->second;
        }
        else {
            throw std::runtime_error("failed to find a suitable GPU!");
        }
    }

    int VK_Device::RateDevice(VkPhysicalDevice device)
    {
        int score = 0;

        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

        // Discrete GPUs have a significant performance advantage
        if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            score += 1000;
        }

        // Maximum possible size of textures affects graphics quality
        score += deviceProperties.limits.maxImageDimension2D;

        // Application can't function without geometry shaders
        if (!deviceFeatures.geometryShader) {
            return 0;
        }

        return score;
    }

    const VK_Device& VK_Device::GetInstance()
    {
        return VKDeviceInstance;
    }

    VK_Device VK_Device::VKDeviceInstance;
}