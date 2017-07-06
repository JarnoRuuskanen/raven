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
     * @param viewportInfo
     */
    bool VulkanPipeline::buildPipeline(const std::vector<ShaderStageParameters> &stages,
                                       const std::vector<VkVertexInputBindingDescription> &bindings,
                                       const std::vector<VkVertexInputAttributeDescription> &attributes,
                                       const VkPrimitiveTopology &topology,
                                       const VkBool32 restartEnabled,
                                       ViewportInfo viewportInfo,
                                       RasterizationInfo rasterizationInfo,
                                       MultisamplingInfo multisamplingInfo,
                                       DepthStencilInfo depthStencilInfo) noexcept
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
            VkPipelineTessellationStateCreateInfo tessellationStateInfo =
                    VulkanStructures::pipelineTessellationStateCreateInfo(8);
        }

        //Specify viewports and scissor tests.
        VkPipelineViewportStateCreateInfo viewportStateInfo =
                VulkanStructures::pipelineViewportStateCreateInfo(viewportInfo.viewports,
                                                                  viewportInfo.scissors);

        //Rasterization.
        VkPipelineRasterizationStateCreateInfo rasterizationStateInfo =
                VulkanStructures::pipelineRasterizationStateCreateInfo(rasterizationInfo.depthClampEnable,
                                                                       rasterizationInfo.rasterizerDiscardEnable,
                                                                       rasterizationInfo.polygonMode,
                                                                       rasterizationInfo.cullingMode,
                                                                       rasterizationInfo.frontFace,
                                                                       rasterizationInfo.depthBiasEnable,
                                                                       rasterizationInfo.depthBiasConstantFactor,
                                                                       rasterizationInfo.depthBiasClamp,
                                                                       rasterizationInfo.depthBiasSlopeFactor,
                                                                       rasterizationInfo.lineWidth);

        //Define multisampling (anti-aliasing).
        VkPipelineMultisampleStateCreateInfo multisamplingStateInfo =
                VulkanStructures::pipelineMultisampleStateCreateInfo(multisamplingInfo.rasterizationSamples,
                                                                     multisamplingInfo.sampleShadingEnable,
                                                                     multisamplingInfo.minSampleShading,
                                                                     multisamplingInfo.sampleMask,
                                                                     multisamplingInfo.alphaToCoverageEnable,
                                                                     multisamplingInfo.alphaToOneEnable);

        //Depth stencil definition.
        VkPipelineDepthStencilStateCreateInfo depthStencilStateInfo =
                VulkanStructures::pipelineDepthStencilStateCreateInfo(depthStencilInfo.depthTestEnable,
                                                                      depthStencilInfo.depthWriteEnable,
                                                                      depthStencilInfo.depthCompareOp,
                                                                      depthStencilInfo.depthBoundsTestEnable,
                                                                      depthStencilInfo.stencilTestEnable,
                                                                      depthStencilInfo.front,
                                                                      depthStencilInfo.back,
                                                                      depthStencilInfo.minDepthBounds,
                                                                      depthStencilInfo.maxDepthBounds);

        //Specify the pipeline color blend state.
        std::vector<VkPipelineColorBlendAttachmentState> attachmentBlendStates =
        {
            {
                false,
                VK_BLEND_FACTOR_ONE,
                VK_BLEND_FACTOR_ONE,
                VK_BLEND_OP_ADD,
                VK_BLEND_FACTOR_ONE,
                VK_BLEND_FACTOR_ONE,
                VK_BLEND_OP_ADD,
                VK_COLOR_COMPONENT_R_BIT |
                VK_COLOR_COMPONENT_G_BIT |
                VK_COLOR_COMPONENT_B_BIT |
                VK_COLOR_COMPONENT_A_BIT
            }
        };

        VkPipelineColorBlendStateCreateInfo colorBlendStateInfo =
                VulkanStructures::pipelineColorBlendStateCreateInfo(false, VK_LOGIC_OP_COPY,
                                                                    attachmentBlendStates,
                                                                    {1.0f,1.0f,1.0f,1.0f});

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
