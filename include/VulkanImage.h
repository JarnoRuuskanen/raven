#pragma once
#include "Headers.h"

namespace Raven
{
    //A structure for
    struct VulkanImage
    {
        VkImage image;

        /**
         * @brief Binds memory object to an image. It is better to bind multiple images to a bigger memory object
         *        than to have a unique memory object for each image.
         * @param logicalDevice
         * @param image
         * @param memory
         * @return False if the memory binding fails.
         */
        bool bindImageMemory(const VkDevice logicalDevice, VkDeviceMemory &memory)
        {
            if(memory == VK_NULL_HANDLE)
            {
                std::cerr << "Failed to bind memory object to an image since memory object was VK_NULL_HANDLE!" << std::endl;
                return false;
            }

            VkResult result = vkBindImageMemory(logicalDevice, image, memory, 0);
            if(result != VK_SUCCESS)
            {
                std::cerr << "Failed to bind memory object to an image!" << std::endl;
                return false;
            }
            return true;
        }
    };
}
