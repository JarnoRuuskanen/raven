#include "VulkanRenderer.h"

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
