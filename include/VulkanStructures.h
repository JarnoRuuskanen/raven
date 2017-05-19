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
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        return createInfo;
    }

    inline VkDeviceQueueCreateInfo deviceQueueCreateInfo(uint32_t familyIndex,
                                                         std::vector<float> priorities)
    {
        VkDeviceQueueCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.queueFamilyIndex = familyIndex;
        createInfo.queueCount = static_cast<uint32_t>(priorities.size());
        createInfo.pQueuePriorities = priorities.data();
        return createInfo;
    }

    inline WINDOW_SURFACE_CREATE_INFO surfaceCreateInfo()
    {
        WINDOW_SURFACE_CREATE_INFO createInfo = {};
        createInfo.sType = WINDOW_SURFACE_CREATE_INFO_STRUCTURE_TYPE;
        createInfo.flags = 0;
        createInfo.pNext = nullptr;
        return createInfo;
    }
}
