#include "VulkanPipeline.h"
#include "VulkanStructures.h"
#include "FileIO.h"
#include "VulkanUtility.h"

namespace Raven
{
    VulkanPipeline::VulkanPipeline()
    {

    }

    VulkanPipeline::~VulkanPipeline()
    {

    }

    /**
     * @brief Builds the pipeline information required for the actual pipeline creation.
     *        This function is mostly a copy from VulkanCookbook 08 - 21.
     * @param logicalDevice
     * @param additionalOptions
     * @param vertexShaderFilename
     * @param fragmentShaderFilename
     * @param vertexInputBindings
     * @param vertexAttributes
     * @param primitiveTopology
     * @param primitiveRestartEnabled
     * @param polygonMode
     * @param cullMode
     * @param frontFace
     * @param logicOpEnable
     * @param logicOp
     * @param attachments
     * @param blendConstants
     * @param layout
     * @param renderPass
     * @param subpass
     * @param parentPipeline
     * @param pipelineCache
     * @param graphicsPipelines
     * @return False if any of the operations fails.
     */
    bool VulkanPipeline::
        buildBasicGraphicsPipelines(const VkDevice logicalDevice,
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
                                    const std::vector<VkPipelineColorBlendAttachmentState> &blendAttachments,
                                    const std::array<float,4> &blendConstants,
                                    VkPipelineLayout layout,
                                    VkRenderPass renderPass,
                                    uint32_t subpass,
                                    VkPipeline parentPipeline,
                                    VkPipelineCache pipelineCache,
                                    std::vector<VkPipeline> &graphicsPipelines) noexcept
    {
        //Read the vertex shader info.
        std::vector<char> vertexShaderSourceCode =
                FileIO::readShaderFile(vertexShaderFilename);

        if(vertexShaderSourceCode.empty())
            return false;

        //Create a vertex shader module.
        VkShaderModule vertexShaderModule;
        if(!createShaderModule(logicalDevice, vertexShaderSourceCode, vertexShaderModule))
            return false;

        //Read the fragment shader information and create a shader module as well.
        std::vector<char> fragmentShaderSourceCode =
                FileIO::readShaderFile(fragmentShaderFilename);

        if(fragmentShaderSourceCode.empty())
            return false;

        VkShaderModule fragmentShaderModule;
        if(!createShaderModule(logicalDevice, fragmentShaderSourceCode, fragmentShaderModule))
            return false;



        //Create all the important information for the pipelineCreateInfo:

        //Describe the shader stages.
        //Vertex shader stage.
        VkPipelineShaderStageCreateInfo vertexStage =
                VulkanStructures::pipelineShaderStageCreateInfo(VK_SHADER_STAGE_VERTEX_BIT,
                                                                vertexShaderModule,
                                                                "main",
                                                                nullptr);

        //Fragment shader stage.
        VkPipelineShaderStageCreateInfo fragmentStage =
                VulkanStructures::pipelineShaderStageCreateInfo(VK_SHADER_STAGE_FRAGMENT_BIT,
                                                                fragmentShaderModule,
                                                                "main",
                                                                nullptr);

        std::vector<VkPipelineShaderStageCreateInfo> shaderStageInfos = {vertexStage, fragmentStage};

        //Describe the pipeline vertex input state.
        VkPipelineVertexInputStateCreateInfo vertexStateInfo =
                VulkanStructures::pipelineVertexInputStateCreateInfo(vertexInputBindings,
                                                                     vertexAttributes);

        //Describe the pipeline input assembly information.
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo =
                VulkanStructures::pipelineInputAssemblyStateCreateInfo(primitiveTopology,
                                                                       primitiveRestartEnabled);

        //Describe the viewport information.
        VkViewport viewport =
        {
            0.0f,
            0.0f,
            800.0f,
            800.0f,
            0.0f,
            1.0f
        };

        VkRect2D scissorOffset = { 0, 0};
        VkRect2D scissorExtent = {800,800};

        VkPipelineViewportStateCreateInfo viewportStateInfo =
                VulkanStructures::pipelineViewportStateCreateInfo({viewport},
                                                                  {scissorOffset, scissorExtent});

        //Describe the rasterization state information.
        VkPipelineRasterizationStateCreateInfo rasterizationStateInfo =
                VulkanStructures::pipelineRasterizationStateCreateInfo(VK_FALSE, VK_FALSE, polygonMode,
                                                                       cullMode, frontFace, VK_FALSE,
                                                                       0.0f, 1.0f, 0.0f, 1.0f);

        //Describe the multisampling state information.
        VkPipelineMultisampleStateCreateInfo multisampleStateInfo =
                VulkanStructures::pipelineMultisampleStateCreateInfo(VK_SAMPLE_COUNT_1_BIT, VK_FALSE,
                                                                     0.0f, nullptr, VK_FALSE, VK_FALSE);

        //Describe the depth stencil state information.
        VkStencilOpState stencilTestParameters =
        {
            VK_STENCIL_OP_KEEP,
            VK_STENCIL_OP_KEEP,
            VK_STENCIL_OP_KEEP,
            VK_COMPARE_OP_ALWAYS,
            0,0,0
        };

        VkPipelineDepthStencilStateCreateInfo depthStencilInfo =
                VulkanStructures::pipelineDepthStencilStateCreateInfo(VK_TRUE, VK_TRUE,
                                                                      VK_COMPARE_OP_LESS_OR_EQUAL,
                                                                      VK_FALSE, VK_FALSE,
                                                                      stencilTestParameters,
                                                                      stencilTestParameters,
                                                                      0.0f,
                                                                      1.0f);

        //Describe the color blending state information.
        VkPipelineColorBlendStateCreateInfo colorBlendStateInfo =
                VulkanStructures::pipelineColorBlendStateCreateInfo(logicOpEnable, logicOp,
                                                                    blendAttachments, blendConstants);

        //Describe the dynamic states.

        std::vector<VkDynamicState> dynamicStates =
        {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };

        VkPipelineDynamicStateCreateInfo dynamicStateInfo =
                VulkanStructures::pipelineDynamicStateCreateInfo(dynamicStates);

        //Create the pipeline create information.
        VkGraphicsPipelineCreateInfo createInfo =
                VulkanStructures::graphicsPipelineCreateInfo(additionalOptions, shaderStageInfos,
                                                             vertexStateInfo, inputAssemblyInfo,
                                                             nullptr, viewportStateInfo,
                                                             rasterizationStateInfo, multisampleStateInfo,
                                                             depthStencilInfo, colorBlendStateInfo,
                                                             dynamicStateInfo, layout, renderPass,
                                                             subpass, parentPipeline, -1);

        //Create the pipeline.
        if(!createGraphicsPipelines(logicalDevice, pipelineCache, {createInfo}, graphicsPipelines))
            return false;

        //Destroy the shader modules as they are no longer needed.
        destroyShaderModule(logicalDevice, vertexShaderModule);
        destroyShaderModule(logicalDevice, fragmentShaderModule);

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
