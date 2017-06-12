#pragma once
#include "Headers.h"
#include "VulkanBuffer.h"
#include "VulkanImage.h"

//List of all vulkan utility functions used by the Raven application.
namespace Raven
{
    //Checks available instance extensions.
    bool checkAvailableInstanceExtensions(std::vector<VkExtensionProperties> &availableExtensions);
    //Checks if a given extension is found in a list of extensions.
    bool isExtensionSupported(std::vector<VkExtensionProperties> &availableExtensions,
                              const char *desiredProperty);
    //Creates a new vulkan instance.
    bool createVulkanInstance(std::vector<char const*> const& desiredExtensions,
                              char const* const appName, VkInstance &instance);
    //Enumerates through all physical, vulkan-capable devices and stores them inside a vector.
    bool loadPhysicalDevices(VkInstance &instance, std::vector<VkPhysicalDevice> &physicalDevices);
    //Checks if a physical device supports a given extension.
    bool arePhysicalDeviceExtensionsSupported(VkPhysicalDevice &physicalDevice,
                                              std::vector<char const*> const& desiredExtensions);
    //Gets physical device features and properties.
    void getPhysicalDeviceFeaturesAndProperties(VkPhysicalDevice &physicalDevice,
                                                VkPhysicalDeviceFeatures& features,
                                                VkPhysicalDeviceProperties& properties);
    //Gets the index of a desired queue family/families.
    bool getQueueFamilyIndex(std::vector<VkQueueFamilyProperties> &queueFamilies,
                             VkQueueFlags desiredQueueFamily,
                             uint32_t &queueFamilyIndex);
    //Gets all physical device queues and their properties.
    bool getPhysicalDeviceQueuesWithProperties(VkPhysicalDevice &physicalDevice,
                                               std::vector<VkQueueFamilyProperties> &queueFamilies); 
    //Creates a vulkan logical device.
    bool createLogicalDevice(VkPhysicalDevice &physicalDevice,
                             VkDeviceCreateInfo createInfo,
                             VkDevice &logicalDevice);

    //Gets queues of a logical device's queue family.
    void getQueueFamilyQueues(const VkDevice logicalDevice,
                              const uint32_t queueFamilyIndex,
                              const uint32_t queueCount,
                              std::vector<VkQueue> &queues);

    //Checks if the preferred presentation mode is supported by the physical device.
    //If not, chooses a default presentation mode.
    bool isPresentationModeSupported(VkPhysicalDevice &physicalDevice,
                                     VkSurfaceKHR &presentationSurface,
                                     VkPresentModeKHR &desiredPresentMode);

    //Gets physical device presentation surface capabilities.
    bool getSurfaceCapabilities(const VkPhysicalDevice &physicalDevice,
                                const VkSurfaceKHR presentationSurface,
                                VkSurfaceCapabilitiesKHR &surfaceCapabilities);
    //Selects swapchain image usage flags based on desired and supported usages.
    bool selectSwapchainImageUsage(const VkSurfaceCapabilitiesKHR &surfaceCapabilities,
                                   const VkImageUsageFlags desiredUsages,
                                   VkImageUsageFlags &imageUsage);

    //Selects swapchain surface transformation based on desired and supported transforms.
    void selectSwapchainSurfaceTransform(VkSurfaceCapabilitiesKHR const &surfaceCapabilities,
                                         VkSurfaceTransformFlagBitsKHR desiredFlags,
                                         VkSurfaceTransformFlagBitsKHR &surfaceTransforms);

    //Selects the format for swapchain images based on desired and supported formats.
    bool selectSwapchainImageFormat(const VkPhysicalDevice &physicalDevice,
                                    const VkSurfaceKHR &presentationSurface,
                                    const VkSurfaceFormatKHR desiredSurfaceFormat,
                                    VkFormat &imageFormat,
                                    VkColorSpaceKHR &imageColorSpace);

    //Creates a vulkan swapchain.
    bool createSwapchain(const VkDevice logicalDevice, const  VkSwapchainCreateInfoKHR createInfo, VkSwapchainKHR &swapchain);

    //Gathers the swapchain images from a given swapchain to a vector.
    bool getSwapchainImages(const VkDevice logicalDevice, const VkSwapchainKHR swapchain, std::vector<VkImage> &swapchainImages);

    //Gets a swapchain image for drawing from a given swapchain.
    bool getSwapchainImageForDrawing(const VkDevice logicalDevice, const VkSwapchainKHR swapchain, VkSemaphore semaphore, uint32_t &imageIndex);

    //Presents an image to the screen.
    bool presentImage(const VkQueue queue, const VkSwapchainKHR swapchain, const VkPresentInfoKHR presentInfo, VkSemaphore &presentationSemaphores);

    //Creates a semaphore.
    bool createSemaphore(const VkDevice logicalDevice, VkSemaphore &semaphore);

    //Destroys a semaphore.
    void destroySemaphore(const VkDevice logicalDevice, VkSemaphore &semaphore);

    //Creates a fence.
    bool createFence(const VkDevice logicalDevice, VkBool32 initializeSignaled, VkFence &fence);

    //Destroys a semaphore.
    void destroyFence(const VkDevice logicalDevice, VkFence &fence);

    //Resets fences.
    bool resetFences(const VkDevice logicalDevice, std::vector<VkFence> &fences);

    //Makes the application wait until the fences are signaled or until timeout has been reached.
    bool waitForFences(const VkDevice logicalDevice, const uint32_t timeout, const VkBool32 waitForAll, std::vector<VkFence>const &fences);

    //Checks if a fence has been signaled or not.
    bool isFenceSignaled(const VkDevice logicalDevice, VkFence &fence);

    //Destroys a presentation surface.
    void destroyPresentationSurface(const VkInstance instance, VkSurfaceKHR &surface);

    //Destroys a swapchain.
    void destroySwapchain(const VkDevice logicalDevice, VkSwapchainKHR &swapchain);

    //Makes the application wait until the selected device is idle.
    bool waitUntilDeviceIdle(VkDevice &logicalDevice);

    //Makes the application wait until the selected queue has completed all tasks given to it.
    bool waitUntilQueueIdle(VkQueue &queue);

    //Creates a buffer.
    bool createBuffer(const VkDevice logicalDevice, const VkBufferCreateInfo createInfo, VkBuffer &buffer);

    //Creates a buffer view.
    bool createBufferView(const VkDevice logicalDevice, const VkBufferViewCreateInfo createInfo, VkBufferView &bufferView);

    //Creates an image.
    bool createImage(const VkDevice logicalDevice, const VkImageCreateInfo createInfo, VkImage &image);

    //Creates an image view.
    bool createImageView(const VkDevice logicalDevice, const VkImageViewCreateInfo createInfo, VkImageView &imageView);

    //Gets the correct memory type for image/buffer.
    bool getMemoryType(VkPhysicalDeviceMemoryProperties memoryProperties,
                       VkMemoryRequirements memReq,
                       VkFlags requiredProperties,
                       uint32_t &memoryTypeIndex);

    //Allocates memory for buffers and images.
    bool allocateMemory(const VkDevice logicalDevice, VkMemoryAllocateInfo allocInfo, VkDeviceMemory &memory);

    //Sets buffer memory barriers.
    void setBufferMemoryBarriers(VkCommandBuffer commandBuffer,
                                 const VkPipelineStageFlags generatingStages,
                                 const VkPipelineStageFlags consumingStages,
                                 std::vector<BufferTransition> bufferTransitions);

    //Sets image memory barriers.
    void setImageMemoryBarriers(VkCommandBuffer commandBuffer,
                                const VkPipelineStageFlags generatingStages,
                                const VkPipelineStageFlags consumingStages,
                                std::vector<ImageTransition> imageTransitions);
}
