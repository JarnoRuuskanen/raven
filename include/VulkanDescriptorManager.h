#pragma once
#include "Headers.h"

namespace Raven
{
    namespace VulkanDescriptorManager
    {
        //Creates a descriptor set layout.
        bool createDescriptorSetLayout(const VkDevice logicalDevice,
                                       VkDescriptorSetLayoutCreateInfo createInfo,
                                       VkDescriptorSetLayout &descriptorSetLayout) noexcept;
        //Destroys a descriptor set layout.
        void destroyDescriptorSetLayout(const VkDevice logicalDevice,
                                        VkDescriptorSetLayout &layout) noexcept;

        //Creates a descriptor pool.
        bool createDescriptorPool(const VkDevice logicalDevice,
                                  VkDescriptorPoolCreateInfo createInfo,
                                  VkDescriptorPool &pool) noexcept;
        //Destroys a descriptor pool.
        void destroyDescriptorPool(const VkDevice logicalDevice,
                                   VkDescriptorPool &pool) noexcept;

        //Resets a descriptor pool.
        bool resetDescriptorPool(const VkDevice logicalDevice,
                                 VkDescriptorPool &pool) noexcept;

        //Allocates descriptor sets from a pool.
        bool allocateDescriptorSets(const VkDevice logicalDevice,
                                    VkDescriptorSetAllocateInfo allocInfo,
                                    std::vector<VkDescriptorSet> descriptorSets) noexcept;

        //Frees descriptor sets.
        bool freeDescriptorSets(const VkDevice logicalDevice,
                                VkDescriptorPool &pool,
                                std::vector<VkDescriptorSet> &descriptorSets) noexcept;
    }
}
