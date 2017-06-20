#pragma once
#include "Headers.h"

namespace Raven
{
    struct ImageTransition
    {
        VkImage image;
        //What type of memory operations are being used before the barrier.
        VkAccessFlags currentAccess;
        //What type of memory operations will be used after the barrier.
        VkAccessFlags newAccess;
        //Current image's internal memory layout.
        VkImageLayout currentLayout;
        //Layout image's memory should change into.
        VkImageLayout newLayout;
        //Queues that have been using the image so far.
        uint32_t currentQueueFamily;
        //The queues which will be using the image from now on.
        uint32_t newQueueFamily;
        //Defines the image's usage context (color,depth or stencil aspect).
        VkImageAspectFlags aspect;
    };

    //A structure for vulkan image management.
    struct VulkanImage
    {
        VkImage image;
        VkImageView imageView;
        //A "fall back" memory object which can be used but it is not recommended.
        VkDeviceMemory imageMemory;
        /**
         * @brief Binds memory object to an image. It is better to bind multiple
         *        images to a bigger memory object than to have a unique memory
         *        object for each image.
         * @param logicalDevice
         * @param image
         * @param memory
         * @return False if the memory binding fails.
         */
        bool bindMemoryObject(const VkDevice logicalDevice, VkDeviceMemory &memory)
        {
            if(memory == VK_NULL_HANDLE)
            {
                std::cerr << "Failed to bind memory object to an image since memory "
                             "object was VK_NULL_HANDLE!" << std::endl;
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
