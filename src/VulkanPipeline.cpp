#include "VulkanPipeline.h"
#include "VulkanStructures.h"

namespace Raven
{
    VulkanPipeline::VulkanPipeline()
    {

    }

    VulkanPipeline::~VulkanPipeline()
    {

    }

    /**
     * @brief Builds the pipeline.
     * @param stages
     * @param bindings
     * @param attributes
     * @param topology
     * @param restartEnabled
     */
    bool VulkanPipeline::buildPipeline(const std::vector<ShaderStageParameters> &stages,
                                       const std::vector<VkVertexInputBindingDescription> &bindings,
                                       const std::vector<VkVertexInputAttributeDescription> &attributes,
                                       const VkPrimitiveTopology &topology,
                                       const VkBool32 restartEnabled) noexcept
    {
        //First describe the shader stages.
        std::vector<VkPipelineShaderStageCreateInfo> stageCreateInfos;
        describePipelineShaderStages(stages, stageCreateInfos);

        //Describe pipeline vertex input state.
        VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo =
                VulkanStructures::pipelineVertexInputStateCreateInfo(bindings, attributes);

        //Describe pipeline input assembly state (what sort of polygons are we building).
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo =
                VulkanStructures::pipelineInputAssemblyStateCreateInfo(topology, restartEnabled);

        //If tessellation is enabled, define the number of vertices(control points) in patches.
        bool tessellationEnabled = false;
        if(tessellationEnabled)
        {
            VkPipelineTessellationStateCreateInfo tessellationInfo =
                    VulkanStructures::pipelineTessellationStateCreateInfo(8);
        }

        return true;
    }

    /**
     * @brief Creates shader stage create infos from the stages-parameter.
     * @param stages
     * @param createInfos
     */
    void VulkanPipeline::
        describePipelineShaderStages(const std::vector<ShaderStageParameters> &stages,
                             std::vector<VkPipelineShaderStageCreateInfo> &createInfos) noexcept
    {
        createInfos.clear();
        for(auto& stage : stages)
        {
            VkPipelineShaderStageCreateInfo stageInfo =
                    VulkanStructures::pipelineShaderStageCreateInfo(stage.shaderStage,
                                                                    stage.module,
                                                                    stage.entryPointName,
                                                                    stage.specializationInfo);
            createInfos.push_back(stageInfo);
        }
    }
}
