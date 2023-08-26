#pragma once
#include <vulkan/vulkan.h>
#include <exception>

namespace CGE
{
    inline void ThrowIfFailed(VkResult result)
    {
        if (result != VK_SUCCESS)
        {
            throw std::exception();
        }
    }
}