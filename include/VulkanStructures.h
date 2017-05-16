#pragma once
//Namespace for all of the vulkan structures used in object creation
#include "Headers.h"

namespace VulkanStructures
{
    inline VkApplicationInfo applicationInfo()
    {
        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pNext = nullptr;
        return appInfo;
    }

    inline VkInstanceCreateInfo instanceCreateInfo()
    {
        VkInstanceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        return createInfo;
    }

    inline VkDeviceCreateInfo deviceCreateInfo()
    {
        VkDeviceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.flags = 0;
        createInfo.pNext = nullptr;
        return createInfo;
    }
}
