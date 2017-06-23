#pragma once
#include "Headers.h"

namespace Raven
{
    namespace VulkanDescriptorManager
    {
        //Destroys a descriptor set layout.
        void destroyDescriptorSetLayout(const VkDevice logicalDevice,
                                        VkDescriptorSetLayout &layout) noexcept;

        //Destroys a descriptor pool.
        void destroyDescriptorPool(const VkDevice logicalDevice,
                                   VkDescriptorPool &pool) noexcept;

        //Resets a descriptor pool.
        bool resetDescriptorPool(const VkDevice logicalDevice,
                                 VkDescriptorPool &pool) noexcept;

        //Frees descriptor sets.
        bool freeDescriptorSets(const VkDevice logicalDevice,
                                VkDescriptorPool &pool,
                                std::vector<VkDescriptorSet> &descriptorSets) noexcept;
    }
}
