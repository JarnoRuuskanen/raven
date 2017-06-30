#pragma once
#include "Headers.h"
#include "VulkanWindow.h"
#include "VulkanImage.h"

namespace Raven
{
    struct SubpassParameters
    {
        //Pipeline bind point (graphics/compute).
        VkPipelineBindPoint pipelineType;
        //The source of data during the subpass.
        std::vector<VkAttachmentReference> inputAttachments;
        //Where the application will render during the subpass.
        std::vector<VkAttachmentReference> colorAttachments;
        //Which color attachments should be resolved (changed from a multisampled image
        //to a non-multisampled/single sampled image.
        std::vector<VkAttachmentReference> resolveAttachments;
        //Which attachment, if any, is used as a depth and/or stencil attachment during
        //the subpass.
        VkAttachmentReference const *depthStencilAttachment;
        //Attachments that are not used in the subpass but whose contents must be preserved
        //during the whole subpass.
        std::vector<uint32_t> preserveAttachments;
    };
}
using namespace Raven;
//A class in charge of rendering content into window/windows provided by the application.
class VulkanRenderer
{
    public:
        VulkanRenderer();
        ~VulkanRenderer();
        //Specifies subpass descriptions.
        void specifySubpassDescriptions(std::vector<SubpassParameters> const &subpassParameters,
                                        std::vector<VkSubpassDescription> &subpassDescriptions);
        //Creates a new render pass, which describes how rendering operations should be run.
        bool createRenderPass(const VkDevice logicalDevice,
                              std::vector<VkAttachmentDescription> const &attachmentDescriptions,
                              std::vector<SubpassParameters> const &subpassParameters,
                              std::vector<VkSubpassDependency> const &subpassDependencies,
                              VkRenderPass &renderPass);

        //Creates a new framebuffer for the given render pass.
        bool createFramebuffer(const VkDevice logicalDevice,
                               VkRenderPass renderPass,
                               std::vector<VkImageView> const &attachments,
                               uint32_t width,
                               uint32_t height,
                               uint32_t layers,
                               VkFramebuffer &framebuffer);

        //Builds a basic render pass for geometry and post processing subpasses.
        bool buildGeometryAndPostProcessingRenderPass(const VkDevice logicalDevice,
                                                      VkRenderPass &renderPass);

        //Builds a render pass and a framebuffer with color and depth attachments.
        bool buildRendererWithColorAndDepthAttachments(VkPhysicalDeviceMemoryProperties memoryProperties,
                                                       const VkDevice logicalDevice,
                                                       uint32_t width,
                                                       uint32_t height,
                                                       VulkanImage &colorImageObject,
                                                       VkDeviceMemory &colorImageMemory,
                                                       VulkanImage &depthImageObject,
                                                       VkDeviceMemory &depthImageMemory,
                                                       VkRenderPass &renderPass,
                                                       VkFramebuffer &framebuffer);

        //Begins a render pass recording.
        void beginRenderPass(VkCommandBuffer cmdBuffer,
                             VkRenderPass renderPass,
                             VkFramebuffer framebuffer,
                             VkRect2D renderArea,
                             std::vector<VkClearValue> const &clearValues,
                             VkSubpassContents subpassContents);

        //Starts the next subpass.
        void startNextSubpass(VkCommandBuffer cmdBuffer, VkSubpassContents subpassContents);

        //Ends the render pass recording.
        void endRenderPass(VkCommandBuffer cmdBuffer);

        //Destroys a framebuffer.
        void destroyFramebuffer(VkDevice logicalDevice, VkFramebuffer &framebuffer);

        //Destroys a render pass.
        void destroyRenderPass(VkDevice logicalDevice, VkRenderPass &renderPass);

    private:
        std::vector<VkRenderPass> renderPasses;
        //Pointers to the windows into which the renderer should render the contents.
        std::vector<VulkanWindow*> windows;
};
