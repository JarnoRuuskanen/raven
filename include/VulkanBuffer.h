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
        //The data that is being held in the buffer.
        void* data = nullptr;

        /**
         * @brief Binds the allocated memory into the buffer.
         * @param logicalDevice
         * @param offset
         * @return False if the memory could not be bound to the buffer.
         */
        bool bindBuffer(const VkDevice logicalDevice, VkDeviceSize offset = 0)
        {
            VkResult result = vkBindBufferMemory(logicalDevice, buffer, memory, offset);
            if(result != VK_SUCCESS)
            {
                std::cerr << "Failed to bind buffer memory!" << std::endl;
                return false;
            }
            return true;
        }
    };
}
