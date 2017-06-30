#pragma once
#include "Headers.h"

//Namespace for all of the vulkan structures used in object creation
namespace VulkanStructures
{
    inline VkApplicationInfo applicationInfo()
    {
        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pNext = nullptr;
        return appInfo;
    }

    inline VkInstanceCreateInfo instanceCreateInfo()
    {
        VkInstanceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        return createInfo;
    }

    inline VkDeviceCreateInfo deviceCreateInfo()
    {
        VkDeviceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        return createInfo;
    }

    inline VkDeviceQueueCreateInfo deviceQueueCreateInfo(uint32_t familyIndex,
                                                         std::vector<float> priorities)
    {
        VkDeviceQueueCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.queueFamilyIndex = familyIndex;
        createInfo.queueCount = static_cast<uint32_t>(priorities.size());
        createInfo.pQueuePriorities = priorities.data();
        return createInfo;
    }

    inline WINDOW_SURFACE_CREATE_INFO surfaceCreateInfo()
    {
        WINDOW_SURFACE_CREATE_INFO createInfo = {};
        createInfo.sType = WINDOW_SURFACE_CREATE_INFO_STRUCTURE_TYPE;
        createInfo.flags = 0;
        createInfo.pNext = nullptr;
        return createInfo;
    }

    inline VkSwapchainCreateInfoKHR swapchainCreateInfo()
    {
        VkSwapchainCreateInfoKHR createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        return createInfo;
    }

    inline VkSemaphoreCreateInfo semaphoreCreateInfo()
    {
        VkSemaphoreCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        createInfo.flags = 0;
        createInfo.pNext = nullptr;
        return createInfo;
    }

    inline VkPresentInfoKHR presentInfoKHR()
    {
        VkPresentInfoKHR presentInfo = {};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.pNext = nullptr;
        return presentInfo;
    }

    inline VkCommandPoolCreateInfo commandPoolCreateInfo(uint32_t familyIndex)
    {
        VkCommandPoolCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.queueFamilyIndex = familyIndex;
        return createInfo;
    }

    inline VkCommandBufferAllocateInfo commandBufferAllocateInfo(VkCommandBufferLevel level,
                                                                 VkCommandPool cmdPool,
                                                                 uint32_t bufferCount)
    {
        VkCommandBufferAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.pNext = nullptr;
        allocInfo.level = level;
        allocInfo.commandPool = cmdPool;
        allocInfo.commandBufferCount = bufferCount;
        return allocInfo;
    }

    inline VkCommandBufferBeginInfo commandBufferBeginInfo(VkCommandBufferUsageFlags usageFlags)
    {
        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.pNext = nullptr;
        beginInfo.flags = usageFlags;
        //For primary command buffers this value should be nullptr.
        beginInfo.pInheritanceInfo = nullptr;
        return beginInfo;
    }

    inline VkCommandBufferInheritanceInfo commandBufferInheritanceInfo()
    {
        VkCommandBufferInheritanceInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
        info.pNext = nullptr;
        return info;
    }

    inline VkFenceCreateInfo fenceCreateInfo(VkBool32 initializeSignaled)
    {
        VkFenceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = initializeSignaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;
        return createInfo;
    }

    inline VkSubmitInfo submitInfo()
    {
        VkSubmitInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        info.pNext = nullptr;
        return info;
    }

    inline VkBufferCreateInfo bufferCreateInfo(VkDeviceSize size,
                                               VkBufferUsageFlags usage,
                                               VkSharingMode sharingMode)
    {
        VkBufferCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        createInfo.flags = 0;
        createInfo.pNext = nullptr;
        createInfo.size = size;
        createInfo.usage = usage;
        createInfo.sharingMode = sharingMode;
        return createInfo;
    }

    inline VkMemoryAllocateInfo memoryAllocateInfo(VkDeviceSize size, uint32_t typeIndex)
    {
        VkMemoryAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.pNext = nullptr;
        allocInfo.allocationSize = size;
        allocInfo.memoryTypeIndex = typeIndex;
        return allocInfo;
    }

    inline VkBufferMemoryBarrier bufferMemoryBarrier(VkAccessFlags currentAccess,
                                                     VkAccessFlags newAccess,
                                                     uint32_t currentQueueFamily,
                                                     uint32_t newQueueFamily,
                                                     VkBuffer buffer,
                                                     VkDeviceSize offset,
                                                     VkDeviceSize size)
    {
        VkBufferMemoryBarrier memoryBarrier = {};
        memoryBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
        memoryBarrier.pNext = nullptr;
        memoryBarrier.srcAccessMask = currentAccess;
        memoryBarrier.dstAccessMask = newAccess;
        memoryBarrier.srcQueueFamilyIndex = currentQueueFamily;
        memoryBarrier.dstQueueFamilyIndex = newQueueFamily;
        memoryBarrier.buffer = buffer;
        memoryBarrier.offset = offset;
        memoryBarrier.size = size;
        return memoryBarrier;
    }

    inline VkBufferViewCreateInfo bufferViewCreateInfo(VkBuffer buffer,
                                                       VkFormat format,
                                                       VkDeviceSize offset,
                                                       VkDeviceSize range)
    {
        VkBufferViewCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.buffer = buffer;
        createInfo.format = format;
        createInfo.offset = offset;
        createInfo.range = range;
        return createInfo;
    }

    inline VkImageCreateInfo imageCreateInfo(VkImageUsageFlags usage,
                                             VkImageType imageType,
                                             VkFormat format,
                                             VkExtent3D extent,
                                             uint32_t layerCount,
                                             VkSampleCountFlagBits samples,
                                             VkImageLayout initialLayout,
                                             VkSharingMode sharingMode,
                                             uint32_t mipLevels,
                                             bool cubemap)
    {
        VkImageCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = cubemap ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT : 0u;
        createInfo.usage = usage;
        createInfo.imageType = imageType;
        createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        createInfo.format = format;
        createInfo.extent = extent;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;
        createInfo.arrayLayers = cubemap ? 6 * layerCount : layerCount;
        createInfo.samples = samples;
        createInfo.initialLayout = initialLayout;
        createInfo.sharingMode = sharingMode;
        createInfo.mipLevels = mipLevels;
        return createInfo;
    }

    inline VkImageViewCreateInfo imageViewCreateInfo(VkImage image,
                                                     VkFormat format,
                                                     VkImageAspectFlags aspect,
                                                     VkImageViewType viewType)
    {
        VkImageViewCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.image = image;
        createInfo.format = format;
        createInfo.viewType = viewType;
        createInfo.subresourceRange = { aspect, 0 , VK_REMAINING_MIP_LEVELS,
                                        0, VK_REMAINING_ARRAY_LAYERS };
        createInfo.components = { VK_COMPONENT_SWIZZLE_IDENTITY,
                                  VK_COMPONENT_SWIZZLE_IDENTITY,
                                  VK_COMPONENT_SWIZZLE_IDENTITY,
                                  VK_COMPONENT_SWIZZLE_IDENTITY };
        return createInfo;
    }

    inline VkImageMemoryBarrier imageMemoryBarrier(VkImage image,
                                                   VkAccessFlags currentAccess,
                                                   VkAccessFlags newAccess,
                                                   uint32_t oldQueueFamily,
                                                   uint32_t newQueueFamily,
                                                   VkImageLayout oldLayout,
                                                   VkImageLayout newLayout,
                                                   VkImageSubresourceRange subresourceRange)
    {
        VkImageMemoryBarrier memoryBarrier = {};
        memoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        memoryBarrier.pNext = nullptr;
        memoryBarrier.image = image;
        memoryBarrier.srcAccessMask = currentAccess;
        memoryBarrier.dstAccessMask = newAccess;
        memoryBarrier.srcQueueFamilyIndex = oldQueueFamily;
        memoryBarrier.dstQueueFamilyIndex = newQueueFamily;
        memoryBarrier.oldLayout = oldLayout;
        memoryBarrier.newLayout = newLayout;
        memoryBarrier.subresourceRange = subresourceRange;
        return memoryBarrier;
    }

    inline std::vector<VkMappedMemoryRange> mappedMemoryRanges(VkDeviceMemory memoryObject,
                                                               VkDeviceSize offset,
                                                               VkDeviceSize size)
    {
        std::vector<VkMappedMemoryRange> ranges =
        {
            {
                VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
                nullptr,
                memoryObject,
                offset,
                size
            }
        };
        return ranges;
    }

    inline VkSamplerCreateInfo samplerCreateInfo(VkFilter minFilter,
                                                 VkFilter magFilter,
                                                 VkSamplerMipmapMode mipmapMode,
                                                 VkSamplerAddressMode addressModeU,
                                                 VkSamplerAddressMode addressModeV,
                                                 VkSamplerAddressMode addressModeW,
                                                 float mipLodBias,
                                                 VkBool32 antisotropyEnable,
                                                 float maxAnisotropy,
                                                 VkBool32 compareEnable,
                                                 VkCompareOp compareOp,
                                                 float minLod,
                                                 float maxLod,
                                                 VkBorderColor borderColor,
                                                 VkBool32 unnormalizedCoordinates)
    {
        VkSamplerCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.minFilter = minFilter;
        createInfo.magFilter = magFilter;
        createInfo.mipmapMode = mipmapMode;
        createInfo.addressModeU = addressModeU;
        createInfo.addressModeV = addressModeV;
        createInfo.addressModeW = addressModeW;
        createInfo.mipLodBias = mipLodBias;
        createInfo.anisotropyEnable = antisotropyEnable;
        createInfo.maxAnisotropy = maxAnisotropy;
        createInfo.compareEnable = compareEnable;
        createInfo.compareOp = compareOp;
        createInfo.minLod = minLod;
        createInfo.maxLod = maxLod;
        createInfo.borderColor = borderColor;
        createInfo.unnormalizedCoordinates = unnormalizedCoordinates;
        return createInfo;
    }

    inline VkDescriptorSetLayoutCreateInfo
        descriptorSetLayoutCreateInfo(const uint32_t bindingCount,
                                      const std::vector<VkDescriptorSetLayoutBinding> bindings)
    {
        VkDescriptorSetLayoutCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.bindingCount = bindingCount;
        createInfo.pBindings = bindings.data();
        return createInfo;
    }

    inline VkDescriptorPoolCreateInfo
        descriptorPoolCreateInfo(VkBool32 freeIndividualSets,
                                 uint32_t maxSets,
                                 std::vector<VkDescriptorPoolSize> descriptorTypes)
    {
        VkDescriptorPoolCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = freeIndividualSets ?
                                VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT : 0;
        createInfo.maxSets = maxSets;
        createInfo.poolSizeCount = static_cast<uint32_t>(descriptorTypes.size());
        createInfo.pPoolSizes = descriptorTypes.data();
        return createInfo;
    }

    inline VkDescriptorSetAllocateInfo
        descriptorSetAllocateInfo(VkDescriptorPool descriptorPool,
                                  std::vector<VkDescriptorSetLayout> descriptorSetLayouts)
    {
        VkDescriptorSetAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.pNext = nullptr;
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(descriptorSetLayouts.size());
        allocInfo.pSetLayouts = descriptorSetLayouts.data();
        return allocInfo;
    }

    inline VkRenderPassCreateInfo
        renderPassCreateInfo(const std::vector<VkAttachmentDescription> attachmentDescriptions,
                             const std::vector<VkSubpassDescription> subpassDescriptions,
                             const std::vector<VkSubpassDependency> subpassDepedencies)
    {
        VkRenderPassCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.attachmentCount = static_cast<uint32_t>(attachmentDescriptions.size());
        createInfo.pAttachments = attachmentDescriptions.data();
        createInfo.subpassCount = static_cast<uint32_t>(subpassDescriptions.size());
        createInfo.pSubpasses = subpassDescriptions.data();
        createInfo.dependencyCount = static_cast<uint32_t>(subpassDepedencies.size());
        createInfo.pDependencies = subpassDepedencies.data();
        return createInfo;
    }

    inline VkFramebufferCreateInfo framebufferCreateInfo(VkRenderPass renderPass,
                                                         std::vector<VkImageView> attachments,
                                                         uint32_t width,
                                                         uint32_t height,
                                                         uint32_t layers)
    {
        VkFramebufferCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.renderPass = renderPass;
        createInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        createInfo.pAttachments = attachments.data();
        createInfo.width = width;
        createInfo.height = height;
        createInfo.layers = layers;
        return createInfo;
    }
}
