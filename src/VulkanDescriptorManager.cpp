#include "VulkanDescriptorManager.h"

namespace Raven
{
    namespace VulkanDescriptorManager
    {
        /**
         * @brief Destroys a descriptor set layout.
         * @param logicalDevice
         * @param layout
         */
        void destroyDescriptorSetLayout(const VkDevice logicalDevice, VkDescriptorSetLayout &layout) noexcept
        {
            if(layout != VK_NULL_HANDLE)
            {
                vkDestroyDescriptorSetLayout(logicalDevice, layout, nullptr);
                layout = VK_NULL_HANDLE;
            }
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
