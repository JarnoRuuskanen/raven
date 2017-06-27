#include "VulkanDescriptorManager.h"

namespace Raven
{
    namespace VulkanDescriptorManager
    {
        /**
         * @brief Creates a descriptor set layout.
         * @param logicalDevice
         * @param createInfo
         * @param descriptorSetLayout
         * @return False if descriptor set layout could not be created.
         */
        bool createDescriptorSetLayout(const VkDevice logicalDevice,
                                       VkDescriptorSetLayoutCreateInfo createInfo,
                                       VkDescriptorSetLayout &descriptorSetLayout) noexcept
        {
            VkResult result = vkCreateDescriptorSetLayout(logicalDevice,
                                                          &createInfo,
                                                          nullptr,
                                                          &descriptorSetLayout);
            if(result != VK_SUCCESS)
            {
                std::cerr << "Failed to create a descriptor set layout!" << std::endl;
                return false;
            }
            return true;
        }

        /**
         * @brief Destroys a descriptor set layout.
         * @param logicalDevice
         * @param layout
         */
        void destroyDescriptorSetLayout(const VkDevice logicalDevice,
                                        VkDescriptorSetLayout &layout) noexcept
        {
            if(layout != VK_NULL_HANDLE)
            {
                vkDestroyDescriptorSetLayout(logicalDevice, layout, nullptr);
                layout = VK_NULL_HANDLE;
            }
        }

        /**
         * @brief Creates a descriptor pool.
         * @param logicalDevice
         * @param createInfo
         * @param pool
         * @return False if descriptor pool could not be created.
         */
        bool createDescriptorPool(const VkDevice logicalDevice,
                                  VkDescriptorPoolCreateInfo createInfo,
                                  VkDescriptorPool &pool) noexcept
        {
            VkResult result = vkCreateDescriptorPool(logicalDevice,
                                                     &createInfo,
                                                     nullptr,
                                                     &pool);
            if(result != VK_SUCCESS)
            {
                std::cerr << "Failed to create a descriptor pool!" << std::endl;
                return false;
            }
            return true;
        }
        /**
         * @brief destroyDescriptorPool. Make sure that no descriptor set allocated from this pool
         *        is in use when the pool gets destroyed.
         * @param logicalDevice
         * @param pool
         */
        void destroyDescriptorPool(const VkDevice logicalDevice, VkDescriptorPool &pool) noexcept
        {
            if(pool != VK_NULL_HANDLE)
            {
                vkDestroyDescriptorPool(logicalDevice, pool, nullptr);
                pool = VK_NULL_HANDLE;
            }
        }

        /**
         * @brief Resets a descriptor pool freeing all the descriptor sets allocated from it
         *        at once.
         * @param logicalDevice
         * @param pool
         * @return False if the descriptor pool could not be reset.
         */
        bool resetDescriptorPool(const VkDevice logicalDevice, VkDescriptorPool &pool) noexcept
        {
            VkResult result = vkResetDescriptorPool(logicalDevice, pool, 0);
            if(result != VK_SUCCESS)
            {
                std::cerr << "Failed to reset descriptor pool!" << std::endl;
                return false;
            }
            return true;
        }

        /**
         * @brief Allocates descriptor sets from a descriptor pool defined in the allocInfo.
         * @param logicalDevice
         * @param allocInfo
         * @param descriptorSets
         * @return
         */
        bool allocateDescriptorSets(const VkDevice logicalDevice,
                                    VkDescriptorSetAllocateInfo allocInfo,
                                    std::vector<VkDescriptorSet> descriptorSets) noexcept
        {
            descriptorSets.resize(allocInfo.descriptorSetCount);
            VkResult result = vkAllocateDescriptorSets(logicalDevice, &allocInfo, descriptorSets.data());
            if(result != VK_SUCCESS)
            {
                std::cerr << "Failed to allocate descriptor sets!" << std::endl;
                return false;
            }
            return true;
        }

        /**
         * @brief Updates descriptor sets.
         * @param logicalDevice
         * @param imageDescriptorInfos
         * @param bufferDescriptorInfos
         * @param texelBufferDescriptorInfos
         * @param copyDescriptorInfos
         */
        void updateDescriptorSets(VkDevice logicalDevice,
                                  const std::vector<ImageDescriptorInfo> &imageDescriptorInfos,
                                  const std::vector<BufferDescriptorInfo> &bufferDescriptorInfos,
                                  const std::vector<TexelBufferDescriptorInfo> &texelBufferDescriptorInfos,
                                  const std::vector<CopyDescriptorInfo> &copyDescriptorInfos) noexcept
        {
                std::vector<VkWriteDescriptorSet> writeDescriptors;
                std::vector<VkCopyDescriptorSet> copyDescriptors;

                /** Image Descriptors **/
                for(auto &imageDescriptor : imageDescriptorInfos)
                {
                    writeDescriptors.push_back(
                    {
                        VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                        nullptr,
                        imageDescriptor.targetDescriptorSet,
                        imageDescriptor.targetDescriptorBinding,
                        imageDescriptor.targetArrayElement,
                        static_cast<uint32_t>(imageDescriptor.imageInfos.size()),
                        imageDescriptor.targetDescriptorType,
                        imageDescriptor.imageInfos.data(),
                        nullptr,
                        nullptr
                    });
                }

                /** Buffer Descriptors **/
                for(auto &bufferDescriptor : bufferDescriptorInfos)
                {
                    writeDescriptors.push_back(
                    {
                        VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                        nullptr,
                        bufferDescriptor.targetDescriptorSet,
                        bufferDescriptor.targetDescriptorBinding,
                        bufferDescriptor.targetArrayElement,
                        static_cast<uint32_t>(bufferDescriptor.bufferInfos.size()),
                        bufferDescriptor.targetDescriptorType,
                        nullptr,
                        bufferDescriptor.bufferInfos.data(),
                        nullptr
                    });
                }

                /** Texel Buffer Descriptors **/
                for(auto &texelDescriptor : texelBufferDescriptorInfos)
                {
                    writeDescriptors.push_back(
                    {
                        VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                        nullptr,
                        texelDescriptor.targetDescriptorSet,
                        texelDescriptor.targetDescriptorBinding,
                        texelDescriptor.targetArrayElement,
                        static_cast<uint32_t>(texelDescriptor.texelBufferViews.size()),
                        texelDescriptor.targetDescriptorType,
                        nullptr,
                        nullptr,
                        texelDescriptor.texelBufferViews.data()
                    });
                }

                /** Copy Descriptors **/
                for(auto &copyDescriptor : copyDescriptorInfos)
                {
                    copyDescriptors.push_back(
                    {
                         VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET,
                         nullptr,
                         copyDescriptor.sourceDescriptorSet,
                         copyDescriptor.sourceDescriptorBinding,
                         copyDescriptor.sourceArrayElement,
                         copyDescriptor.targetDescriptorSet,
                         copyDescriptor.targetDescriptorBinding,
                         copyDescriptor.targetArrayElement,
                         copyDescriptor.descriptorCount
                    });
                }

                /** Update the descriptor sets **/
                vkUpdateDescriptorSets(logicalDevice, static_cast<uint32_t>(writeDescriptors.size()),
                                       writeDescriptors.data(),
                                       static_cast<uint32_t>(copyDescriptors.size()),
                                       copyDescriptors.data());
        }

        /**
         * @brief Binds descriptor sets to a command buffer. This function is also almost a
         *        direct copy from VulkanCookbook.
         * @param cmdBuffer
         * @param pipelineType
         * @param pipelineLayout
         * @param indexForFirstSet
         * @param descriptorSets
         * @param dynamicOffsets
         */
        void bindDescriptorSets(VkCommandBuffer cmdBuffer,
                                VkPipelineBindPoint pipelineType,
                                VkPipelineLayout pipelineLayout,
                                uint32_t indexForFirstSet,
                                std::vector<VkDescriptorSet> const &descriptorSets,
                                std::vector<uint32_t> const &dynamicOffsets) noexcept
        {
            vkCmdBindDescriptorSets(cmdBuffer, pipelineType, pipelineLayout, indexForFirstSet,
                                    static_cast<uint32_t>(descriptorSets.size()), descriptorSets.data(),
                                    static_cast<uint32_t>(dynamicOffsets.size()), dynamicOffsets.data());
        }

        /**
         * @brief Frees descriptor sets that were allocated from the same pool.
         *        The descriptor pool must have been created with correct flags to allow
         *        freeing individual descriptor sets.
         * @param logicalDevice
         * @param pool
         * @param descriptorSets
         * @return False if the descriptor sets could not be freed.
         */
        bool freeDescriptorSets(const VkDevice logicalDevice,
                                VkDescriptorPool &pool,
                                std::vector<VkDescriptorSet> &descriptorSets) noexcept
        {
            VkResult result =
                    vkFreeDescriptorSets(logicalDevice,
                                         pool,
                                         static_cast<uint32_t>(descriptorSets.size()),
                                         descriptorSets.data());
            if(result != VK_SUCCESS)
            {
                std::cerr << "Failed to free descriptor sets! Was descriptor pool "
                             "created with VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT flag?"
                          << std::endl;
                return false;
            }
            return true;
        }
    }
}
