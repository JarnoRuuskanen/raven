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

    inline VkRenderPassBeginInfo renderPassBeginInfo(VkRenderPass renderPass,
                                                     VkFramebuffer framebuffer,
                                                     std::vector<VkClearValue> clearValues,
                                                     VkRect2D renderArea)
    {
        VkRenderPassBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        beginInfo.pNext = nullptr;
        beginInfo.renderPass = renderPass;
        beginInfo.framebuffer = framebuffer;
        beginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        beginInfo.pClearValues = clearValues.data();
        beginInfo.renderArea = renderArea;
        return beginInfo;
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

    inline VkShaderModuleCreateInfo shaderModuleCreateInfo (std::vector<char> sourceCode)
    {
        VkShaderModuleCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.codeSize = sourceCode.size();
        createInfo.pCode = reinterpret_cast<uint32_t const*>(sourceCode.data());
        return createInfo;
    }

    inline VkPipelineShaderStageCreateInfo
        pipelineShaderStageCreateInfo(VkShaderStageFlagBits stage,
                                      VkShaderModule module,
                                      const char *entryPointName,
                                      const VkSpecializationInfo *specializationInfo)
    {
        VkPipelineShaderStageCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.stage = stage;
        createInfo.module = module;
        createInfo.pName = entryPointName;
        createInfo.pSpecializationInfo = specializationInfo;
        return createInfo;
    }

    inline VkPipelineVertexInputStateCreateInfo
        pipelineVertexInputStateCreateInfo (const std::vector<VkVertexInputBindingDescription> bindings,
                                            const std::vector<VkVertexInputAttributeDescription> attributes)
    {
        VkPipelineVertexInputStateCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindings.size());
        createInfo.pVertexBindingDescriptions = bindings.data();
        createInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributes.size());
        createInfo.pVertexAttributeDescriptions = attributes.data();
        return createInfo;
    }

    inline VkPipelineInputAssemblyStateCreateInfo
        pipelineInputAssemblyStateCreateInfo(const VkPrimitiveTopology &topology,
                                             const VkBool32 restartEnabled)
    {
        VkPipelineInputAssemblyStateCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.topology = topology;
        createInfo.primitiveRestartEnable = restartEnabled;
        return createInfo;
    }

    inline VkPipelineTessellationStateCreateInfo
        pipelineTessellationStateCreateInfo(uint32_t controlPoints)
    {
        VkPipelineTessellationStateCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.patchControlPoints = controlPoints;
        return createInfo;
    }

    inline VkPipelineViewportStateCreateInfo
        pipelineViewportStateCreateInfo(const std::vector<VkViewport> viewports,
                                        const std::vector<VkRect2D> scissors)
    {
        VkPipelineViewportStateCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.viewportCount = static_cast<uint32_t>(viewports.size());
        createInfo.pViewports = viewports.data();
        createInfo.scissorCount = static_cast<uint32_t>(scissors.size());
        createInfo.pScissors = scissors.data();
        return createInfo;
    }

    inline VkPipelineRasterizationStateCreateInfo
        pipelineRasterizationStateCreateInfo(VkBool32 depthClampEnable,
                                             VkBool32 rasterizerDiscardEnable,
                                             VkPolygonMode polygonMode,
                                             VkCullModeFlags cullingMode,
                                             VkFrontFace frontFace,
                                             VkBool32 depthBiasEnable,
                                             float depthBiasConstantFactor,
                                             float depthBiasClamp,
                                             float depthBiasSlopeFactor,
                                             float lineWidth)
    {
        VkPipelineRasterizationStateCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        //Should fragments outside of the min/max range be clamped withing this range?
        createInfo.depthClampEnable = depthClampEnable;
        createInfo.rasterizerDiscardEnable = rasterizerDiscardEnable;
        createInfo.polygonMode = polygonMode;
        createInfo.cullMode = cullingMode;
        createInfo.frontFace = frontFace;
        createInfo.depthBiasEnable = depthBiasEnable;
        createInfo.depthBiasConstantFactor = depthBiasConstantFactor;
        createInfo.depthBiasClamp = depthBiasClamp;
        createInfo.depthBiasSlopeFactor = depthBiasSlopeFactor;
        createInfo.lineWidth = lineWidth;
        return createInfo;
    }

    inline VkPipelineMultisampleStateCreateInfo
        pipelineMultisampleStateCreateInfo(VkSampleCountFlagBits rasterizationSamples,
                                           VkBool32 sampleShadingEnable,
                                           float minSampleShading,
                                           const VkSampleMask *sampleMask,
                                           VkBool32 alphaToCoverageEnable,
                                           VkBool32 alphaToOneEnable)
    {
        VkPipelineMultisampleStateCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.rasterizationSamples = rasterizationSamples;
        createInfo.sampleShadingEnable = sampleShadingEnable;
        createInfo.minSampleShading = minSampleShading;
        createInfo.pSampleMask = sampleMask;
        createInfo.alphaToCoverageEnable = alphaToCoverageEnable;
        createInfo.alphaToOneEnable = alphaToOneEnable;
        return createInfo;
    }

    inline VkPipelineDepthStencilStateCreateInfo
        pipelineDepthStencilStateCreateInfo(VkBool32 depthTestEnable,
                                            VkBool32 depthWriteEnable,
                                            VkCompareOp depthCompareOp,
                                            VkBool32 depthBoundsTestEnable,
                                            VkBool32 stencilTestEnable,
                                            VkStencilOpState front,
                                            VkStencilOpState back,
                                            float minDepthBounds,
                                            float maxDepthBounds)
    {
        VkPipelineDepthStencilStateCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.depthTestEnable = depthTestEnable;
        createInfo.depthWriteEnable = depthWriteEnable;
        createInfo.depthCompareOp = depthCompareOp;
        createInfo.depthBoundsTestEnable = depthBoundsTestEnable;
        createInfo.stencilTestEnable = stencilTestEnable;
        createInfo.front = front;
        createInfo.back = back;
        createInfo.minDepthBounds = minDepthBounds;
        createInfo.maxDepthBounds = maxDepthBounds;
        return createInfo;
    }

    inline VkPipelineColorBlendStateCreateInfo
        pipelineColorBlendStateCreateInfo(VkBool32 logicOpEnable,
                                          VkLogicOp logicOp,
                                          std::vector<VkPipelineColorBlendAttachmentState> attachments,
                                          std::array<float,4> const blendConstants)
    {
        VkPipelineColorBlendStateCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.logicOpEnable = logicOpEnable;
        createInfo.logicOp = logicOp;
        createInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        createInfo.pAttachments = attachments.data();
        createInfo.blendConstants[0] = blendConstants[0];
        createInfo.blendConstants[1] = blendConstants[1];
        createInfo.blendConstants[2] = blendConstants[2];
        createInfo.blendConstants[3] = blendConstants[3];
        return createInfo;
    }

    inline VkPipelineDynamicStateCreateInfo
        pipelineDynamicStateCreateInfo(std::vector<VkDynamicState> const dynamicStates)
    {
        VkPipelineDynamicStateCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
        createInfo.pDynamicStates = dynamicStates.data();
        return createInfo;
    }

    inline VkPipelineLayoutCreateInfo
        pipelineLayoutCreateInfo(std::vector<VkDescriptorSetLayout> descriptorSetLayouts,
                                 std::vector<VkPushConstantRange> pushConstantRanges)
    {
        VkPipelineLayoutCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
        createInfo.pSetLayouts = descriptorSetLayouts.data();
        createInfo.pushConstantRangeCount = static_cast<uint32_t>(pushConstantRanges.size());
        createInfo.pPushConstantRanges = pushConstantRanges.data();
        return createInfo;
    }
}
