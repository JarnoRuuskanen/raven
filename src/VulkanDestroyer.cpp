#include "VulkanDestroyer.h"
/*
namespace Raven
{
    //Define a destructor for most common Vulkan objects.
    #define VK_DESTROYER_SPECIALIZATION(type, deleter)  \
    template<>                                          \
    void VulkanDestroyer<type>::destroy(){              \
            if(deleter != nullptr){                     \
                deleter(device, object, nullptr);       \
            }                                           \
    }

    /**
     * @brief Destroys an instance.
     */
/*    void VulkanDestroyer<VkInstance>::destroy()
    {
        if(vkDestroyInstance != nullptr)
        {
            vkDestroyInstance(instance, nullptr);
        }
    }
*/
    /**
     * @brief Destroys a logical device.
     */
/*    void VulkanDestroyer<VkDevice>::destroy()
    {
        if(vkDestroyDevice != nullptr)
        {
            vkDestroyDevice(logicalDevice, nullptr);
        }
    }
*/
    /**
     * @brief Destroys a surface.
     */
/*    void VulkanDestroyer<VkSurfaceKHR>::destroy()
    {
        if(vkDestroySurfaceKHR != nullptr)
        {
            vkDestroySurfaceKHR(instance, surface, nullptr);
        }
    }

/*
    //Define what destructor functions should be called for each object type.
    VK_DESTROYER_SPECIALIZATION(VkSemaphore, vkDestroySemaphore)
    VK_DESTROYER_SPECIALIZATION(VkFence, vkDestroyFence)
    VK_DESTROYER_SPECIALIZATION(VkDeviceMemory, vkFreeMemory)
    VK_DESTROYER_SPECIALIZATION(VkBuffer, vkDestroyBuffer)
    VK_DESTROYER_SPECIALIZATION(VkImage, vkDestroyImage)
    VK_DESTROYER_SPECIALIZATION(VkEvent, vkDestroyEvent)
    VK_DESTROYER_SPECIALIZATION(VkQueryPool, vkDestroyQueryPool)
    VK_DESTROYER_SPECIALIZATION(VkBufferView, vkDestroyBufferView)
    VK_DESTROYER_SPECIALIZATION(VkImageView, vkDestroyImageView)
    VK_DESTROYER_SPECIALIZATION(VkShaderModule, vkDestroyShaderModule)
    VK_DESTROYER_SPECIALIZATION(VkPipelineCache, vkDestroyPipelineCache)
    VK_DESTROYER_SPECIALIZATION(VkPipelineLayout, vkDestroyPipelineLayout)
    VK_DESTROYER_SPECIALIZATION(VkRenderPass, vkDestroyRenderPass)
    VK_DESTROYER_SPECIALIZATION(VkPipeline, vkDestroyPipeline)
    VK_DESTROYER_SPECIALIZATION(VkDescriptorSetLayout, vkDestroyDescriptorSetLayout)
    VK_DESTROYER_SPECIALIZATION(VkSampler, vkDestroySampler)
    VK_DESTROYER_SPECIALIZATION(VkDescriptorPool, vkDestroyDescriptorPool)
    VK_DESTROYER_SPECIALIZATION(VkFramebuffer, vkDestroyFramebuffer)
    VK_DESTROYER_SPECIALIZATION(VkCommandPool, vkDestroyCommandPool)
    VK_DESTROYER_SPECIALIZATION(VkSwapchainKHR, vkDestroySwapchainKHR)
}
*/
