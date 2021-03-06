#include "CommandBufferManager.h"
#include "VulkanStructures.h"

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
                               VkCommandPool &cmdPool) noexcept
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
        bool beginCommandBuffer(VkCommandBuffer &cmdBuffer,
                                const VkCommandBufferUsageFlags usage)
        {
            VkCommandBufferBeginInfo beginInfo =
                    VulkanStructures::commandBufferBeginInfo(usage);

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

        /**
         * @brief Resets a command buffer. This is far less expensive than creating a new cmd buffer.
         *        Note that a command buffer can only be reset explicitly with this function if the
         *        command pool was created with VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT flag.
         * @param cmdBuffer
         * @param releaseResources
         * @return False if something went wrong.
         */
        bool resetCommandBuffer(VkCommandBuffer &cmdBuffer, VkBool32 releaseResources)
        {
            VkResult result =
                    vkResetCommandBuffer(cmdBuffer,
                                         releaseResources ?
                                             VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT : 0);
            if(result != VK_SUCCESS)
            {
                std::cerr << "Failed to reset a command buffer!" << std::endl;
                return false;
            }
            return true;
        }

        /**
         * @brief Resets a command pool and all the buffers in it.
         * @param logicalDevice
         * @param cmdPool
         * @param releaseResources
         * @return False if something went wrong.
         */
        bool resetCommandPool(VkDevice logicalDevice, VkCommandPool &cmdPool, VkBool32 releaseResources)
        {
            VkResult result =
                    vkResetCommandPool(logicalDevice,
                                       cmdPool,
                                       releaseResources ?
                                           VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT : 0);
            if(result != VK_SUCCESS)
            {
                std::cerr << "Failed to reset a command pool!" << std::endl;
                return false;
            }
            return true;
        }

        /**
         * @brief Submits command buffers to a queue and signals the fence when done submitting.
         *        Note that submitting command buffers is fairly expensive
         *        operation so always try to batch as many command buffers
         *        together for submit as possible.
         * @param queue
         * @param submitInfoCount
         * @param submitInfo
         * @param fence
         * @return False if the submit fails.
         */
        bool submitCommandBuffers(const VkQueue queue, const uint32_t submitInfoCount,
                                  VkSubmitInfo submitInfo, VkFence &fence)
        {
            VkResult result = vkQueueSubmit(queue, submitInfoCount, &submitInfo, fence);
            if(result != VK_SUCCESS)
            {
                std::cerr << "Failed to submit command buffers to a queue" << std::endl;
                return false;
            }
            return true;
        }

        /**
         * @brief Frees the memory of given command buffers and clears the vector. Make sure, that the
         *        command buffers are not used or referenced to anywhere at this point.
         * @param device
         * @param cmdPool
         * @param cmdBuffers
         */
        void freeCommandBuffers(const VkDevice logicalDevice,
                                VkCommandPool &cmdPool,
                                std::vector<VkCommandBuffer> &cmdBuffers)
        {
            if(cmdBuffers.size() > 0)
            {
                vkFreeCommandBuffers(logicalDevice, cmdPool,
                                     static_cast<uint32_t>(cmdBuffers.size()), cmdBuffers.data());
                cmdBuffers.clear();
            }
        }

        /**
         * @brief Destroys a command pool and all the command buffers allocated from it.
         * @param logicalDevice
         * @param cmdPool
         */
        void destroyCommandPool(const VkDevice logicalDevice, VkCommandPool &cmdPool)
        {
            if(cmdPool != VK_NULL_HANDLE)
            {
                vkDestroyCommandPool(logicalDevice,cmdPool, nullptr);
                cmdPool = VK_NULL_HANDLE;
            }
        }

        /**
         * @brief recordCommandBuffersOnMultipleThreads
         * @param threadParams
         * @param queue
         * @param waitSemaphoreInfos
         * @param signaledSemaphores
         * @param fence
         * @return
         */
        bool recordCommandBuffersOnMultipleThreads(const std::vector<CommandBufferRecordingThreadParameters> &threadParams,
                                                   VkQueue queue,
                                                   std::vector<WaitSemaphoreInfo> waitSemaphoreInfos,
                                                   std::vector<VkSemaphore> signaledSemaphores,
                                                   VkFence fence)
        {
            std::vector<std::thread> threads(threadParams.size());
            for(size_t i = 0; i < threadParams.size(); ++i)
            {
                threads[i] = std::thread(threadParams[i].recordingFunction, threadParams[i].cmdBuffer);
            }

            std::vector<VkCommandBuffer> cmdBuffers(threadParams.size());
            for(size_t i = 0; i < threadParams.size(); ++i)
            {
                threads[i].join();
                cmdBuffers[i] = threadParams[i].cmdBuffer;
            }

            //Create the submit information.
            std::vector<VkSemaphore> waitingSemaphores;
            std::vector<VkPipelineStageFlags> waitingStages;
            for(auto &info : waitSemaphoreInfos)
            {
                waitingSemaphores.push_back(info.semaphore);
                waitingStages.push_back(info.waitingStage);
            }

            VkSubmitInfo submitInfo = VulkanStructures::submitInfo(cmdBuffers, waitingSemaphores,
                                                                   waitingStages, signaledSemaphores);

            if(!submitCommandBuffers(queue, 1, submitInfo, fence))
                return false;

            return true;
        }

        /**
         * @brief A function for creating both the command pool and allocating buffers from it.
         * @param logicalDevice
         * @param queueFamilyIndex
         * @param cmdPool
         * @param cmdBufferCount
         * @param cmdBuffers
         * @return
         */
        bool createCmdPoolAndBuffers(const VkDevice &logicalDevice,
                                     uint32_t queueFamilyIndex,
                                     VkCommandPool &cmdPool,
                                     uint32_t cmdBufferCount,
                                     std::vector<VkCommandBuffer> &cmdBuffers)
        {
            //Create the command pool info.
            VkCommandPoolCreateInfo poolInfo =
                    VulkanStructures::commandPoolCreateInfo(queueFamilyIndex);
            //Create a command pool.
            if(!CommandBufferManager::createCommandPool(logicalDevice, poolInfo, cmdPool))
                return false;

            VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            VkCommandBufferAllocateInfo allocInfo =
                    VulkanStructures::commandBufferAllocateInfo(level, cmdPool, cmdBufferCount);
            //Allocate command buffers from the pool into a vector.
            cmdBuffers.clear();
            cmdBuffers.resize(cmdBufferCount);
            if(!allocateCommandBuffer(logicalDevice, allocInfo, cmdBuffers))
            {
                std::cerr << "Failed to allocate command buffers!" << std::endl;
                return false;
            }
            return true;
        }
    }
}
