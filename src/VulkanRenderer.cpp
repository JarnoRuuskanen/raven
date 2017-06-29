#include "VulkanRenderer.h"
#include "VulkanStructures.h"
/**
 * @brief VulkanRenderer::VulkanRenderer
 */
VulkanRenderer::VulkanRenderer()
{

}

/**
 * @brief VulkanRenderer::~VulkanRenderer
 */
VulkanRenderer::~VulkanRenderer()
{

}

/**
 * @brief Specifies subpass descriptions.
 * @param subpassParameters
 * @param subpassDescriptions
 */
void VulkanRenderer::specifySubpassDescriptions(const std::vector<SubpassParameters> &subpassParameters,
                                                std::vector<VkSubpassDescription> &subpassDescriptions)
{
        subpassDescriptions.clear();

        for(auto &description : subpassParameters)
        {
            subpassDescriptions.push_back(
            {
                0,                                                              //VkSubpassDescriptionFlags
                description.pipelineType,                                       //VkPipelineBindPoint
                static_cast<uint32_t>(description.inputAttachments.size()),
                description.inputAttachments.data(),                            //const VkAttachmentReference
                static_cast<uint32_t>(description.colorAttachments.size()),
                description.colorAttachments.data(),                            //const VkAttachmentReference
                description.resolveAttachments.data(),                          //const VkAttachmentReference
                description.depthStencilAttachment,                             //const VkAttachmentReference
                static_cast<uint32_t>(description.preserveAttachments.size()),
                description.preserveAttachments.data()                          //const uint32_t
            });
        }
}

/**
 * @brief Creates a new render pass, which describes how rendering operations should be run.
 * @param logicalDevice
 * @param attachmentDescriptions
 * @param subpassParameters
 * @param subpassDepedenccies
 * @param renderPass
 * @return
 */
bool VulkanRenderer::createRenderPass(const VkDevice logicalDevice,
                                      const std::vector<VkAttachmentDescription> &attachmentDescriptions,
                                      const std::vector<SubpassParameters> &subpassParameters,
                                      const std::vector<VkSubpassDependency> &subpassDepedencies,
                                      VkRenderPass &renderPass)
{
    //First specify attachment descriptions if not already specified.

    //Also specify subpass descriptions.
    std::vector<VkSubpassDescription> subpassDescriptions;
    specifySubpassDescriptions(subpassParameters, subpassDescriptions);

    //Specify depedencies between subpasses if not already specified.

    //After that, create a new render pass.
    VkRenderPassCreateInfo createInfo =
            VulkanStructures::renderPassCreateInfo(attachmentDescriptions,
                                                   subpassDescriptions,
                                                   subpassDepedencies);

    VkResult result = vkCreateRenderPass(logicalDevice, &createInfo, nullptr, &renderPass);
    if(result != VK_SUCCESS)
    {
        std::cerr << "Failed to create a render pass!" << std::endl;
        return false;
    }
    return true;
}
