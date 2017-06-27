#pragma once
#include "Headers.h"

namespace Raven
{
    //Structs for updating descriptors.
    //These are from VulkanCookbook:
    //Descriptor Sets - Updating descriptor sets.

    //Samplers and all kinds of image resources:
    struct ImageDescriptorInfo
    {
        VkDescriptorSet targetDescriptorSet;
        uint32_t targetDescriptorBinding;
        uint32_t targetArrayElement;
        VkDescriptorType targetDescriptorType;
        std::vector<VkDescriptorImageInfo> imageInfos;
    };

    //Uniform and storage buffers (dynamic and non-dynamic):
    struct BufferDescriptorInfo
    {
        VkDescriptorSet targetDescriptorSet;
        uint32_t targetDescriptorBinding;
        uint32_t targetArrayElement;
        VkDescriptorType targetDescriptorType;
        std::vector<VkDescriptorBufferInfo> bufferInfos;
    };

    //Uniform and storag texel buffers:
    struct TexelBufferDescriptorInfo
    {
        VkDescriptorSet                     targetDescriptorSet;
        uint32_t                            targetDescriptorBinding;
        uint32_t                            targetArrayElement;
        VkDescriptorType                    targetDescriptorType;
        std::vector<VkBufferView>           texelBufferViews;
    };

    //For copying descriptor data from other, already updated sets.
    struct CopyDescriptorInfo
    {
        VkDescriptorSet targetDescriptorSet;
        uint32_t targetDescriptorBinding;
        uint32_t targetArrayElement;
        VkDescriptorSet sourceDescriptorSet;
        uint32_t sourceDescriptorBinding;
        uint32_t sourceArrayElement;
        uint32_t descriptorCount;
    };

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

        //Updates descriptor sets.
        void updateDescriptorSets(VkDevice logicalDevice,
                                  const std::vector<ImageDescriptorInfo> &imageDescriptorInfos,
                                  const std::vector<BufferDescriptorInfo> &bufferDescriptorInfos,
                                  const std::vector<TexelBufferDescriptorInfo> &texelBufferDescriptorInfos,
                                  const std::vector<CopyDescriptorInfo> &copyDescriptorInfos) noexcept;

        //Binds descriptor sets to a command buffer.
        void bindDescriptorSets(VkCommandBuffer cmdBuffer,
                                VkPipelineBindPoint pipelineType,
                                VkPipelineLayout pipelineLayout,
                                uint32_t indexForFirstSet,
                                std::vector<VkDescriptorSet> const &descriptorSets,
                                std::vector<uint32_t> const &dynamicOffsets) noexcept;

        //Frees descriptor sets.
        bool freeDescriptorSets(const VkDevice logicalDevice,
                                VkDescriptorPool &pool,
                                std::vector<VkDescriptorSet> &descriptorSets) noexcept;
    }
}
