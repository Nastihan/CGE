#pragma once
#include <vulkan/vulkan.h>
#include <exception>
#include <stdexcept>
#include "..\ConsoleLog.h"

namespace CGE
{
    inline void ThrowIfFailed(VkResult result)
    {
        if (result != VK_SUCCESS)
        {
            throw std::runtime_error("VK Fail: " + result);
        }
    }
}