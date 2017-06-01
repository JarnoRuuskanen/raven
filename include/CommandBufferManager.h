#pragma once
#include "Headers.h"

namespace Raven
{
    /**A struct to help synchronize command submits.
    *  The idea for this struct comes from VulkanCookbook.
    **/
    struct WaitSemaphoreInfo
    {
        VkSemaphore semaphore;
        //At which stage should the hardware wait?
        VkPipelineStageFlags waitingStage;
    };

    namespace CommandBufferManager
    {
        //Creates a new command pool.
        bool createCommandPool(const VkDevice logicalDevice,
                               const VkCommandPoolCreateInfo createInfo,
                               VkCommandPool &cmdPool);
        //Allocates a new command buffer from a given command pool.
        bool allocateCommandBuffer(const VkDevice logicalDevice,
                                   const VkCommandBufferAllocateInfo allocInfo,
                                   std::vector<VkCommandBuffer> &commandBuffers);
        //Opens a command buffer for recording.
        bool beginCommandBuffer(VkCommandBuffer &cmdBuffer, const VkCommandBufferBeginInfo beginInfo);
        //Ends command buffer recording.
        bool endCommandBuffer(VkCommandBuffer &cmdBuffer);
        //Resets a command buffer. This is far less expensive than creating a new cmd buffer.
        bool resetCommandBuffer(VkCommandBuffer &cmdBuffer, VkBool32 releaseResources);
        //Resets a command pool and all the buffers in it.
        bool resetCommandPool(VkDevice logicalDevice, VkCommandPool &cmdPool, VkBool32 releaseResources);
        //Submits command buffers to a queue.
        bool submitCommandBuffers(const VkQueue queue, const uint32_t submitInfoCount, VkSubmitInfo submitInfo, VkFence &fence);
        //Frees the memory of given command buffers and clears the vector.
        void freeCommandBuffers(const VkDevice logicalDevice, VkCommandPool &cmdPool, std::vector<VkCommandBuffer> &cmdBuffers);
        //Destroys a command pool and all the command buffers allocated from it.
        void destroyCommandPool(const VkDevice logicalDevice, VkCommandPool &cmdPool);
    }
}
