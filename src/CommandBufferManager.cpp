#include "CommandBufferManager.h"

namespace Raven
{
    namespace CommandBufferManager
    {
        /**
         * @brief Creates a new command pool. Each application thread should use
         *        it's own command pool as command pools cannot be accessed from
         *        multiple threads at the same time.
         * @param logicalDevice
         * @param createInfo
         * @param cmdPool
         * @return False if something went wrong.
         */
        bool createCommandPool(const VkDevice logicalDevice,
                               const VkCommandPoolCreateInfo createInfo,
                               VkCommandPool &cmdPool)
        {
            VkResult result = vkCreateCommandPool(logicalDevice, &createInfo, nullptr, &cmdPool);
            if(result != VK_SUCCESS)
            {
                std::cerr << "Failed to create a command pool!" << std::endl;
                return false;
            }
            return true;
        }

        /**
         * @brief Allocates a new command buffer from a given command pool.
         * @return False if something went wrong.
         */
        bool allocateCommandBuffer(const VkDevice logicalDevice,
                                   const VkCommandBufferAllocateInfo allocInfo,
                                   std::vector<VkCommandBuffer> &commandBuffers)
        {
            VkResult result;
            result = vkAllocateCommandBuffers(logicalDevice, &allocInfo, commandBuffers.data());
            if(result != VK_SUCCESS)
            {
                std::cerr << "Failed to allocate command buffers!" << std::endl;
                return false;
            }
            return true;
        }
    }
}
