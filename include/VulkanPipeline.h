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

    //A class for both compute and graphics pipelines in Vulkan.
    class VulkanPipelineManager
    {
        public:
            VulkanPipelineManager();
            ~VulkanPipelineManager();
            //Creates basic vertex/fragment -stage graphics pipelines with the given information.
            bool createBasicGraphicsPipelines(const VkDevice logicalDevice,
                                             VkPipelineCreateFlags additionalOptions,
                                             const std::string  &vertexShaderFilename,
                                             const std::string &fragmentShaderFilename,
                                             const std::vector<VkVertexInputBindingDescription> &vertexInputBindings,
                                             const std::vector<VkVertexInputAttributeDescription> &vertexAttributes,
                                             VkPrimitiveTopology primitiveTopology,
                                             VkBool32 primitiveRestartEnabled,
                                             VkPolygonMode polygonMode,
                                             VkCullModeFlags cullMode,
                                             VkFrontFace frontFace,
                                             VkBool32 logicOpEnable,
                                             VkLogicOp logicOp,
                                             const std::vector<VkPipelineColorBlendAttachmentState> &attachments,
                                             const std::array<float,4> &blendConstants,
                                             VkPipelineLayout layout,
                                             VkRenderPass renderPass,
                                             uint32_t subpass,
                                             VkPipeline parentPipeline,
                                             VkPipelineCache pipelineCache,
                                             std::vector<VkPipeline> &graphicsPipelines) noexcept;
        private:
            //Describes shader stages.
            void describePipelineShaderStages(std::vector<ShaderStageParameters> const &stages,
                                              std::vector<VkPipelineShaderStageCreateInfo> &stageInfos) noexcept;
    };
}
