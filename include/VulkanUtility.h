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
                                                VkPhysicalDeviceProperties& properties) noexcept;
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
                              std::vector<VkQueue> &queues) noexcept;

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
    bool createSwapchain(const VkDevice logicalDevice, const  VkSwapchainCreateInfoKHR createInfo,
                         VkSwapchainKHR &swapchain);

    //Gathers the swapchain images from a given swapchain to a vector.
    bool getSwapchainImages(const VkDevice logicalDevice, const VkSwapchainKHR swapchain,
                            std::vector<VkImage> &swapchainImages);

    //Gets a swapchain image for drawing from a given swapchain.
    bool getSwapchainImageForDrawing(const VkDevice logicalDevice, const VkSwapchainKHR swapchain,
                                     VkSemaphore semaphore, uint32_t &imageIndex);

    //Presents an image to the screen.
    bool presentImage(const VkQueue queue, const VkSwapchainKHR swapchain,
                      const VkPresentInfoKHR presentInfo, VkSemaphore &presentationSemaphores);

    //Destroys a presentation surface.
    void destroyPresentationSurface(const VkInstance instance, VkSurfaceKHR &surface);

    //Destroys a swapchain.
    void destroySwapchain(const VkDevice logicalDevice, VkSwapchainKHR &swapchain) noexcept;

    //Creates a semaphore.
    bool createSemaphore(const VkDevice logicalDevice, VkSemaphore &semaphore);

    //Destroys a semaphore.
    void destroySemaphore(const VkDevice logicalDevice, VkSemaphore &semaphore) noexcept;

    //Creates a fence.
    bool createFence(const VkDevice logicalDevice, VkBool32 initializeSignaled, VkFence &fence);

    //Destroys a semaphore.
    void destroyFence(const VkDevice logicalDevice, VkFence &fence) noexcept;

    //Resets fences.
    bool resetFences(const VkDevice logicalDevice, std::vector<VkFence> &fences);

    //Makes the application wait until the fences are signaled or until timeout has been reached.
    bool waitForFences(const VkDevice logicalDevice, const uint32_t timeout,
                       const VkBool32 waitForAll, std::vector<VkFence>const &fences);

    //Checks if a fence has been signaled or not.
    bool isFenceSignaled(const VkDevice logicalDevice, VkFence &fence);

    //Sets buffer memory barriers.
    void setBufferMemoryBarriers(VkCommandBuffer commandBuffer,
                                 const VkPipelineStageFlags generatingStages,
                                 const VkPipelineStageFlags consumingStages,
                                 std::vector<BufferTransition> bufferTransitions) noexcept;

    //Sets image memory barriers.
    void setImageMemoryBarriers(VkCommandBuffer commandBuffer,
                                const VkPipelineStageFlags generatingStages,
                                const VkPipelineStageFlags consumingStages,
                                std::vector<ImageTransition> imageTransitions) noexcept;

    //Makes the application wait until the selected device is idle.
    bool waitUntilDeviceIdle(VkDevice &logicalDevice);

    //Makes the application wait until the selected queue has completed all tasks given to it.
    bool waitUntilQueueIdle(VkQueue &queue);

    //Creates a buffer.
    bool createBuffer(const VkDevice logicalDevice, const VkBufferCreateInfo createInfo,
                      VkBuffer &buffer);

    //Destroys a buffer.
    void destroyBuffer(const VkDevice logicalDevice, VkBuffer &buffer) noexcept;

    //Creates and prepares a staging buffer which can be used for
    //updating device-local memory in particular.
    bool prepareStagingBuffer(const VkDevice logicalDevice,
                              VkPhysicalDeviceMemoryProperties memoryProperties,
                              VkDeviceSize allocationSize,
                              VkBuffer &stagingBuffer,
                              VkDeviceMemory &stagingMemory);

    //Creates a buffer view.
    bool createBufferView(const VkDevice logicalDevice,
                          const VkBufferViewCreateInfo createInfo,
                          VkBufferView &bufferView);

    //Destroys a buffer view.
    void destroyBufferView(const VkDevice logicalDevice, VkBufferView &bufferView) noexcept;

    //Creates an image.
    bool createImage(const VkDevice logicalDevice, const VkImageCreateInfo createInfo, VkImage &image);

    //Destroys an image.
    void destroyImage(const VkDevice logicalDevice, VkImage &image) noexcept;

    //Creates an image view.
    bool createImageView(const VkDevice logicalDevice, const VkImageViewCreateInfo createInfo,
                         VkImageView &imageView);

    //Destroys an image view.
    void destroyImageView(const VkDevice logicalDevice, VkImageView &imageView) noexcept;

    //Creates a sampler.
    bool createSampler(const VkDevice logicalDevice, VkSamplerCreateInfo samplerInfo, VkSampler &sampler);

    //Destroys a sampler.
    void destroySampler(const VkDevice logicalDevice, VkSampler &sampler);

    //Gets the correct memory type for image/buffer.
    bool getMemoryType(VkPhysicalDeviceMemoryProperties memoryProperties,
                       VkMemoryRequirements memReq,
                       VkFlags requiredProperties,
                       uint32_t &memoryTypeIndex);

    //Allocates memory for buffers and images.
    bool allocateMemory(const VkDevice logicalDevice,
                        VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties,
                        VkMemoryRequirements memReq,
                        VkFlags requiredMemoryPropertyFlags,
                        VkDeviceMemory &memory);

    //Frees allocated memory.
    void freeMemory(const VkDevice logicalDevice, VkDeviceMemory &memory) noexcept;

    //This function takes data and pushes it into a staging resource and then
    //gives a pointer to that data.
    bool flushDataToHostLocalMemory(const VkDevice logicalDevice, VkDeviceMemory deviceMemory,
                           VkDeviceSize offset, VkDeviceSize dataSize, void* data,
                           void **pointer, bool unmap);

    //Copies data between two buffers.
    bool copyDataBetweenBuffers(VkCommandBuffer cmdBuffer, VkBuffer sourceBuffer,
                                VkBuffer dstBuffer, std::vector<VkBufferCopy> memoryRanges);

    //Copies data from a buffer to an image.
    bool copyDataFromBufferToImage(VkCommandBuffer cmdBuffer, VkBuffer sourceBuffer,
                                   VkImage dstImage, VkImageLayout imageLayout,
                                   std::vector<VkBufferImageCopy> memoryRanges);

    //Copies data from an image to a buffer.
    bool copyDataFromImageToBuffer(VkCommandBuffer cmdBuffer, VkImage sourceImage,
                                   VkImageLayout imageLayout, VkBuffer dstBuffer,
                                   std::vector<VkBufferImageCopy> memoryRanges);

    //Updates a buffer which uses device-local memory.
    bool updateDeviceLocalMemoryBuffer(VkDevice logicalDevice,
                                       void *data,
                                       VulkanBuffer stagingBufferObject,
                                       VkDeviceMemory stagingMemory,
                                       VkBuffer destinationBuffer,
                                       VkDeviceSize destinationOffset,
                                       VkAccessFlags destinationBufferCurrentAccess,
                                       VkAccessFlags destinationBufferNewAccess,
                                       VkPipelineStageFlags destinationBufferGeneratingStages,
                                       VkPipelineStageFlags destinationBufferConsumingStages,
                                       VkQueue queue,
                                       VkCommandBuffer cmdBuffer,
                                       std::vector<VkSemaphore> signalSemaphores);

    //Updates an image that is using device-local memory.
    bool updateDeviceLocalMemoryImage(VkDevice logicalDevice,
                                      void *data,
                                      VulkanBuffer stagingBufferObject,
                                      VkDeviceMemory stagingMemory,
                                      VkImage destinationImage,
                                      VkImageSubresourceLayers destinationImageSubresource,
                                      VkOffset3D destinationImageOffset,
                                      VkExtent3D destinationImageSize,
                                      VkAccessFlags destinationImageCurrentAccess,
                                      VkAccessFlags destinationImageNewAccess,
                                      VkImageAspectFlags destinationImageAspect,
                                      VkPipelineStageFlags destinationImageGeneratingStages,
                                      VkPipelineStageFlags destinationImageConsumingStages,
                                      VkImageLayout destinationImageCurrentLayout,
                                      VkImageLayout destinationImageNewLayout,
                                      VkQueue queue,
                                      VkCommandBuffer cmdBuffer,
                                      std::vector<VkSemaphore> signalSemaphores);

    //Checks wether a format supports a required optimal tiling feature.
    bool doesFormatSupportRequiredOptimalTilingFeature(const VkPhysicalDevice physicalDevice,
                                                       VkFormat format,
                                                       VkFormatFeatureFlagBits requiredFeature);

    //Checks wether a format supports a required buffer feature.
    bool doesFormatSupportRequiredBufferFeature(const VkPhysicalDevice physicalDevice,
                                                VkFormat format,
                                                VkFormatFeatureFlagBits requiredFeature);

    //Creates an image with an image view.
    bool createImageWithImageView(const VkDevice logicalDevice,
                                  VkPhysicalDeviceMemoryProperties memoryProperties,
                                  VkImageUsageFlags usage, VkImageType imageType,
                                  VkImageViewType imageViewType, VkFormat format, VkExtent3D extent,
                                  uint32_t layerCount, VkSampleCountFlagBits samples,
                                  VkImageLayout initialLayout, VkSharingMode sharingMode,
                                  uint32_t mipLevels, VkBool32 cubemap, VkImageAspectFlags aspect,
                                  VulkanImage &imageObject, VkDeviceMemory &imageMemoryObject);

    //Creates a new shader module.
    bool createShaderModule(const VkDevice logicalDevice,
                            std::vector<char> sourceCode,
                            VkShaderModule &module) noexcept;

    //Creates a pipeline layout.
    bool createPipelineLayout(const VkDevice logicalDevice,
                              std::vector<VkDescriptorSetLayout> descriptorSetLayouts,
                              std::vector<VkPushConstantRange> pushConstantRanges,
                              VkPipelineLayout &layout) noexcept;

    //Creates a pipeline cache.
    bool createPipelineCache(const VkDevice logicalDevice,
                             std::vector<unsigned char> cacheData,
                             VkPipelineCache &cache);

    //Gets pipeline cache data.
    bool getPipelineCacheData(const VkDevice logicalDevice,
                              VkPipelineCache cache,
                              std::vector<unsigned char> &cacheData);

    //Merges multiple pipeline caches. This could be used after, for an example, multiple
    //pipelines (with each owning their own cache object) have been created with threads.
    bool mergePipelineCaches(const VkDevice logicalDevice,
                             VkPipelineCache &combinedCaches,
                             std::vector<VkPipelineCache> sourceCaches);

    //Creates graphics pipelines.
    bool createGraphicsPipelines(const VkDevice logicalDevice,
                                 VkPipelineCache cache,
                                 const std::vector<VkGraphicsPipelineCreateInfo> &createInfos,
                                 std::vector<VkPipeline> &graphicsPipelines) noexcept;

    //Creates compute pipelines.
    bool createComputePipelines(const VkDevice logicalDevice,
                                VkPipelineCache cache,
                                const std::vector<VkComputePipelineCreateInfo> &createInfos,
                                std::vector<VkPipeline> &computePipelines) noexcept;
}
