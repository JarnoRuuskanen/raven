#pragma once
#include "Headers.h"

//A struct for holding all the buffer information.
//This is different from a command buffer and they
//should not be mixed.

namespace Raven
{
    struct VulkanBuffer
    {
        //For holding the data.
        VkBuffer buffer = VK_NULL_HANDLE;
        //Buffer size.
        VkDeviceSize size = 0;
        //Buffer memory.
        VkDeviceMemory memory = VK_NULL_HANDLE;
        //Buffer usage flags.
        VkBufferUsageFlags usageFlags;
        //View to the buffer's data.
        VkBufferView bufferView;
    };
}
