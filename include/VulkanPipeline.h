#pragma once
#include "Headers.h"

namespace Raven
{
    //A struct for holding shader stage parameters.
    struct ShaderStageParameters
    {
        //What stage the rest of the parameters are specified for.
        VkShaderStageFlagBits shaderStage;
        //The shader module holding the SPIR-V source code.
        VkShaderModule module;
        //Function entry point in the shader.
        char const *entryPointName;
        //Allows values of the constant variables defined in the shader code
        //to be modified at runtime during pipeline creation.
        VkSpecializationInfo const *specializationInfo;
    };

    //A struct for holding viewport information.
    struct ViewportInfo
    {
        //Parameters for a set of viewports.
        std::vector<VkViewport> viewports;
        //Scissor tests corresponding to each viewport.
        std::vector<VkRect2D> scissors;
    };

    //A struct for holding rasterization information.
    struct RasterizationInfo
    {
        VkBool32 depthClampEnable;
        VkBool32 rasterizerDiscardEnable;
        VkPolygonMode polygonMode;
        VkCullModeFlags cullingMode;
        VkFrontFace frontFace;
        VkBool32 depthBiasEnable;
        float depthBiasConstantFactor;
        float depthBiasClamp;
        float depthBiasSlopeFactor;
        float lineWidth;
    };

    struct MultisamplingInfo
    {
        VkSampleCountFlagBits rasterizationSamples;
        VkBool32 sampleShadingEnable;
        float minSampleShading;
        const VkSampleMask *sampleMask;
        VkBool32 alphaToCoverageEnable;
        VkBool32 alphaToOneEnable;
    };

    struct DepthStencilInfo
    {
        VkBool32 depthTestEnable;
        VkBool32 depthWriteEnable;
        VkCompareOp depthCompareOp;
        VkBool32 depthBoundsTestEnable;
        VkBool32 stencilTestEnable;
        VkStencilOpState front;
        VkStencilOpState back;
        float minDepthBounds;
        float maxDepthBounds;
    };

    //A class for both compute and graphics pipelines in Vulkan.
    class VulkanPipeline
    {
        public:
            VulkanPipeline();
            ~VulkanPipeline();
            //Builds the pipeline.
            bool buildGraphicsPipeline(const std::vector<ShaderStageParameters> &stages,
                                       const std::vector<VkVertexInputBindingDescription> &bindings,
                                       const std::vector<VkVertexInputAttributeDescription> &attributes,
                                       const VkPrimitiveTopology &topology,
                                       const VkBool32 restartEnabled,
                                       ViewportInfo viewportInfo,
                                       RasterizationInfo rasterizerInfo,
                                       MultisamplingInfo multisamplingInfo,
                                       DepthStencilInfo depthStencilInfo) noexcept;
        private:
            //Describes shader stages.
            void describePipelineShaderStages(std::vector<ShaderStageParameters> const &stages,
                                              std::vector<VkPipelineShaderStageCreateInfo> &stageInfos) noexcept;

    };
}
