#pragma once
#include "Headers.h"

namespace Raven
{
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
    }
}
