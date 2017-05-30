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

        /**
         * @brief Opens a command buffer for recording. The command buffer must be in initial state
                  (it was allocated with VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT). Command buffer
                  recording starts with this command and ends with endCommandBuffer-function.
         * @param cmdBuffer
         * @param beginInfo
         */
        bool beginCommandBuffer(VkCommandBuffer &cmdBuffer, const VkCommandBufferBeginInfo beginInfo)
        {
            VkResult result = vkBeginCommandBuffer(cmdBuffer, &beginInfo);
            if(result != VK_SUCCESS)
            {
                std::cerr << "Failed to begin a command buffer!" << std::endl;
                return false;
            }
            return true;
        }

        /**
         * @brief Ends command buffer recording.
         * @param cmdBuffer
         * @return False if something went wrong.
         */
        bool endCommandBuffer(VkCommandBuffer &cmdBuffer)
        {
            VkResult result = vkEndCommandBuffer(cmdBuffer);
            if(result != VK_SUCCESS)
            {
                std::cerr << "Failed to end a command buffer recording!" << std::endl;
                return false;
            }
            return true;
        }
    }
}
