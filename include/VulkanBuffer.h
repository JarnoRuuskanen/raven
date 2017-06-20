#pragma once
#include "Headers.h"

//Structs regarding buffers and buffer memory barriers
namespace Raven
{
    //I copied this buffer memory barrier idea from VulkanCookbook,
    //4 Resources and Memory - Setting a buffer memory barrier.
    struct BufferTransition
    {
        //The buffer that will use the barrier.
        VkBuffer buffer;
        //How the buffer has been used so far.
        VkAccessFlags currentAccess;
        //How the buffer will be used from now on.
        VkAccessFlags newAccess;
        uint32_t currentQueueFamily;
        uint32_t newQueueFamily;
    };

    //A struct for holding all the buffer information.
    //This is different from a command buffer and they
    //should not be mixed.
    struct VulkanBuffer
    {
        //For holding the data.
        VkBuffer buffer = VK_NULL_HANDLE;
        //Buffer size.
        VkDeviceSize size = 0;
        //Buffer usage flags.
        VkBufferUsageFlags usageFlags;
        //View to the buffer's data.
        VkBufferView bufferView;
        //The data that is being held in the buffer.
        void* data = nullptr;

        /**
         * @brief Binds the allocated memory into the buffer. It is better to have
         *        multiple buffers bound to a single, bigger memory object than to have
         *        a dedicated memory object for each buffer. This is because certain
         *        graphics devices can only do so many memory allocations no matter the memory size.
         *        Smaller memory objects can also end up using more memory due to rounding up etc.
         * @param logicalDevice
         * @param offset
         * @return False if the memory could not be bound to the buffer.
         */
        bool bindMemoryObject(const VkDevice logicalDevice,VkDeviceMemory &memory,VkDeviceSize offset = 0)
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
