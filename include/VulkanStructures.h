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

    inline VkSwapchainCreateInfoKHR swapchainCreateInfo()
    {
        VkSwapchainCreateInfoKHR createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        return createInfo;
    }

    inline VkSemaphoreCreateInfo semaphoreCreateInfo()
    {
        VkSemaphoreCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        createInfo.flags = 0;
        createInfo.pNext = nullptr;
        return createInfo;
    }

    inline VkPresentInfoKHR presentInfoKHR()
    {
        VkPresentInfoKHR presentInfo = {};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.pNext = nullptr;
        return presentInfo;
    }

    inline VkCommandPoolCreateInfo commandPoolCreateInfo(uint32_t familyIndex)
    {
        VkCommandPoolCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.queueFamilyIndex = familyIndex;
        return createInfo;
    }

    inline VkCommandBufferAllocateInfo commandBufferAllocateInfo(VkCommandBufferLevel level,
                                                                 VkCommandPool cmdPool,
                                                                 uint32_t bufferCount)
    {
        VkCommandBufferAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.pNext = nullptr;
        allocInfo.level = level;
        allocInfo.commandPool = cmdPool;
        allocInfo.commandBufferCount = bufferCount;
        return allocInfo;
    }

    inline VkCommandBufferBeginInfo commandBufferBeginInfo(VkCommandBufferUsageFlags usageFlags)
    {
        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.pNext = nullptr;
        beginInfo.flags = usageFlags;
        //For primary command buffers this value should be nullptr.
        beginInfo.pInheritanceInfo = nullptr;
        return beginInfo;
    }

    inline VkCommandBufferInheritanceInfo commandBufferInheritanceInfo()
    {
        VkCommandBufferInheritanceInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
        info.pNext = nullptr;
        return info;
    }

    inline VkFenceCreateInfo fenceCreateInfo(VkBool32 initializeSignaled)
    {
        VkFenceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = initializeSignaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;
        return createInfo;
    }

    inline VkSubmitInfo submitInfo()
    {
        VkSubmitInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        info.pNext = nullptr;
        return info;
    }

    inline VkBufferCreateInfo bufferCreateInfo(VkDeviceSize size,
                                               VkBufferUsageFlags usage,
                                               VkSharingMode sharingMode)
    {
        VkBufferCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        createInfo.flags = 0;
        createInfo.pNext = nullptr;
        createInfo.size = size;
        createInfo.usage = usage;
        createInfo.sharingMode = sharingMode;
        return createInfo;
    }

    inline VkMemoryAllocateInfo memoryAllocateInfo(VkDeviceSize size, uint32_t typeIndex)
    {
        VkMemoryAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.pNext = nullptr;
        allocInfo.allocationSize = size;
        allocInfo.memoryTypeIndex = typeIndex;
        return allocInfo;
    }
}
