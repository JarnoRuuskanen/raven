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

    //A class for both compute and graphics pipelines in Vulkan.
    class VulkanPipeline
    {
        public:
            VulkanPipeline();
            ~VulkanPipeline();
            //Builds the pipeline.
            bool buildPipeline(const std::vector<ShaderStageParameters> &stages,
                               const std::vector<VkVertexInputBindingDescription> &bindings,
                               const std::vector<VkVertexInputAttributeDescription> &attributes,
                               const VkPrimitiveTopology &topology,
                               const VkBool32 restartEnabled) noexcept;
        private:
            //Describes shader stages.
            void describePipelineShaderStages(std::vector<ShaderStageParameters> const &stages,
                                              std::vector<VkPipelineShaderStageCreateInfo> &stageInfos) noexcept;

    };
}
