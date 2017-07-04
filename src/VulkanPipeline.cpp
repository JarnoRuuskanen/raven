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
     * @brief Creates shader stage create infos from the stages-parameter.
     * @param stages
     * @param createInfos
     */
    void VulkanPipeline::
        describeShaderStages(const std::vector<ShaderStageParameters> &stages,
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
