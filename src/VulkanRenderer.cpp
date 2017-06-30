#include "VulkanRenderer.h"
#include "VulkanStructures.h"
#include "VulkanUtility.h"
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
 * @param subpassDependencies
 * @param renderPass
 * @return False if render pass creation fails.
 */
bool VulkanRenderer::createRenderPass(const VkDevice logicalDevice,
                                      const std::vector<VkAttachmentDescription> &attachmentDescriptions,
                                      const std::vector<SubpassParameters> &subpassParameters,
                                      const std::vector<VkSubpassDependency> &subpassDependencies,
                                      VkRenderPass &renderPass)
{
    //First specify attachment descriptions if not already specified.

    //Also specify subpass descriptions.
    std::vector<VkSubpassDescription> subpassDescriptions;
    specifySubpassDescriptions(subpassParameters, subpassDescriptions);

    //Specify dependencies between subpasses if not already specified.

    //After that, create a new render pass.
    VkRenderPassCreateInfo createInfo =
            VulkanStructures::renderPassCreateInfo(attachmentDescriptions,
                                                   subpassDescriptions,
                                                   subpassDependencies);

    VkResult result = vkCreateRenderPass(logicalDevice, &createInfo, nullptr, &renderPass);
    if(result != VK_SUCCESS)
    {
        std::cerr << "Failed to create a render pass!" << std::endl;
        return false;
    }
    return true;
}

/**
 * @brief Creates a new framebuffer for the given render pass.
 * @param logicalDevice
 * @param renderPass
 * @param attachments
 * @param width
 * @param height
 * @param layers
 * @param framebuffer
 * @return False if framebuffer creation fails.
 */
bool VulkanRenderer::createFramebuffer(const VkDevice logicalDevice,
                                       VkRenderPass renderPass,
                                       std::vector<VkImageView> const &attachments,
                                       uint32_t width,
                                       uint32_t height,
                                       uint32_t layers,
                                       VkFramebuffer &framebuffer)
{
    VkFramebufferCreateInfo createInfo =
            VulkanStructures::framebufferCreateInfo(renderPass,
                                                    attachments,
                                                    width, height, layers);

    //Create the framebuffer.
    VkResult result = vkCreateFramebuffer(logicalDevice, &createInfo, nullptr, &framebuffer);
    if(result != VK_SUCCESS)
    {
        std::cerr << "Failed to create a framebuffer!" << std::endl;
        return false;
    }
    return true;
}

/**
 * @brief Builds a basic render pass with two subpasses. One subpass drawing
 *        geometry and one for post processing. The post processing subpass will
 *        draw the finished results into a swapchain image. This function is almost a direct
 *        copy from Vulkan Cookbook.
 * @param logicalDevice
 * @param renderPass
 * @return False if the render pass could not be created.
 */
bool VulkanRenderer::buildGeometryAndPostProcessingRenderPass(const VkDevice logicalDevice,
                                                              VkRenderPass &renderPass)
{
    //First specify attachment descriptions (what sort of data will we be using).
    std::vector<VkAttachmentDescription> attachmentDescriptions =
    {
        {
            0,                                                // VkAttachmentDescriptionFlags
            VK_FORMAT_R8G8B8A8_UNORM,                         // VkFormat
            VK_SAMPLE_COUNT_1_BIT,                            // VkSampleCountFlagBits
            VK_ATTACHMENT_LOAD_OP_CLEAR,                      // VkAttachmentLoadOp
            VK_ATTACHMENT_STORE_OP_DONT_CARE,                 // VkAttachmentStoreOp
            VK_ATTACHMENT_LOAD_OP_DONT_CARE,                  // VkAttachmentLoadOp
            VK_ATTACHMENT_STORE_OP_DONT_CARE,                 // VkAttachmentStoreOp
            VK_IMAGE_LAYOUT_UNDEFINED,                        // VkImageLayout
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,         // VkImageLayout
          },
          {
            0,                                                // VkAttachmentDescriptionFlags
            VK_FORMAT_D16_UNORM,                              // VkFormat
            VK_SAMPLE_COUNT_1_BIT,                            // VkSampleCountFlagBits
            VK_ATTACHMENT_LOAD_OP_CLEAR,                      // VkAttachmentLoadOp
            VK_ATTACHMENT_STORE_OP_DONT_CARE,                 // VkAttachmentStoreOp
            VK_ATTACHMENT_LOAD_OP_DONT_CARE,                  // VkAttachmentLoadOp
            VK_ATTACHMENT_STORE_OP_DONT_CARE,                 // VkAttachmentStoreOp
            VK_IMAGE_LAYOUT_UNDEFINED,                        // VkImageLayout
            VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, // VkImageLayout
          },
          {
            0,                                                // VkAttachmentDescriptionFlags
            VK_FORMAT_R8G8B8A8_UNORM,                         // VkFormat
            VK_SAMPLE_COUNT_1_BIT,                            // VkSampleCountFlagBits
            VK_ATTACHMENT_LOAD_OP_CLEAR,                      // VkAttachmentLoadOp
            VK_ATTACHMENT_STORE_OP_STORE,                     // VkAttachmentStoreOp
            VK_ATTACHMENT_LOAD_OP_DONT_CARE,                  // VkAttachmentLoadOp
            VK_ATTACHMENT_STORE_OP_DONT_CARE,                 // VkAttachmentStoreOp
            VK_IMAGE_LAYOUT_UNDEFINED,                        // VkImageLayout
            VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,                  // VkImageLayout
          }
    };

    //Attachment reference for the depth stencil attachment.
    VkAttachmentReference depthStencilAttachment =
    {
        1,                                                    //attachment
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL      //layout
    };

    std::vector<SubpassParameters> subpassParameters =
    {
        //First subpass:
        {
            VK_PIPELINE_BIND_POINT_GRAPHICS,                // VkPipelineBindPoint                  PipelineType
            {},                                             // std::vector<VkAttachmentReference>   InputAttachments
            {                                               // std::vector<VkAttachmentReference>   ColorAttachments
              {
                0,                                          // uint32_t                             attachment
                VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL    // VkImageLayout                        layout
              }
            },
            {},                                             // std::vector<VkAttachmentReference>   ResolveAttachments
            &depthStencilAttachment,                        // const VkAttachmentReference        * DepthStencilAttachment
            {}                                              // std::vector<uint32_t>                PreserveAttachments
          },
          //Second
          {
            VK_PIPELINE_BIND_POINT_GRAPHICS,                // VkPipelineBindPoint                  PipelineType
            {                                               // std::vector<VkAttachmentReference>   InputAttachments
              {
                0,                                          // uint32_t                             attachment
                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL    // VkImageLayout                        layout
              }
            },
            {                                               // std::vector<VkAttachmentReference>   ColorAttachments
              {
                2,                                          // uint32_t                             attachment
                VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL    // VkImageLayout                        layout
              }
            },
            {},                                             // std::vector<VkAttachmentReference>   ResolveAttachments
            nullptr,                                        // const VkAttachmentReference        * DepthStencilAttachment
            {}                                              // std::vector<uint32_t>                PreserveAttachments
        }
    };

    //Dependencies between subpasses:
    std::vector<VkSubpassDependency> subpassDependencies =
    {
        {
            0,                                              //srcSubpass
            1,                                              //dstSubpass
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,  //srcStageMask
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,          //dstStageMask
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,           //srcAccessMask
            VK_ACCESS_COLOR_ATTACHMENT_READ_BIT,            //dstAccessMask
            VK_DEPENDENCY_BY_REGION_BIT                     //dependencyFlags
        }
    };

    //Create the render pass.
    if(!createRenderPass(logicalDevice, attachmentDescriptions, subpassParameters, subpassDependencies,
                         renderPass))
    {
        return false;
    }
    return true;
}

/**
 * @brief Builds a render pass and a framebuffer with color and depth attachments.
 *        Parts of this function were copied from VulkanCookbook. They are marked with comments.
 * @param memoryProperties
 * @param logicalDevice
 * @param width
 * @param height
 * @param colorImageObject
 * @param colorImageMemory
 * @param depthImageObject
 * @param depthImageMemory
 * @param renderPass
 * @param framebuffer
 * @return False if some of the operations fails.
 */
bool VulkanRenderer::
    buildRendererWithColorAndDepthAttachments(VkPhysicalDeviceMemoryProperties memoryProperties,
                                               const VkDevice logicalDevice,
                                               uint32_t width,
                                               uint32_t height,
                                               VulkanImage &colorImageObject,
                                               VkDeviceMemory &colorImageMemory,
                                               VulkanImage &depthImageObject,
                                               VkDeviceMemory &depthImageMemory,
                                               VkRenderPass &renderPass,
                                               VkFramebuffer &framebuffer)
{
    //First create the color image + image view.
    VkExtent3D imageExtent = {width, height, 1};
    VkImageCreateInfo colorImageInfo =
            VulkanStructures::imageCreateInfo(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
                                              VK_IMAGE_USAGE_SAMPLED_BIT, VK_IMAGE_TYPE_2D,
                                              VK_FORMAT_R8G8B8A8_UNORM,
                                              imageExtent, 1, VK_SAMPLE_COUNT_1_BIT,
                                              VK_IMAGE_LAYOUT_UNDEFINED, VK_SHARING_MODE_EXCLUSIVE,
                                              1, false);

    VkImageViewCreateInfo colorImageViewInfo =
            VulkanStructures::imageViewCreateInfo(colorImageObject.image, VK_FORMAT_R8G8B8A8_UNORM,
                                                  VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_2D);

    if(!createImageWithImageView(logicalDevice, memoryProperties, colorImageInfo, colorImageViewInfo,
                                 colorImageObject, colorImageMemory))
    {
        return false;
    }

    //Next create the depth image + image view.
    VkImageCreateInfo depthImageInfo =
        VulkanStructures::imageCreateInfo(VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT |
                                          VK_IMAGE_USAGE_SAMPLED_BIT, VK_IMAGE_TYPE_2D,
                                          VK_FORMAT_D16_UNORM, imageExtent, 1, VK_SAMPLE_COUNT_1_BIT,
                                          VK_IMAGE_LAYOUT_UNDEFINED, VK_SHARING_MODE_EXCLUSIVE,
                                          1, false);

    VkImageViewCreateInfo depthImageViewInfo =
        VulkanStructures::imageViewCreateInfo(depthImageObject.image, VK_FORMAT_D16_UNORM,
                                              VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_VIEW_TYPE_2D);

    if(!createImageWithImageView(logicalDevice, memoryProperties, depthImageInfo, depthImageViewInfo,
                                 depthImageObject, depthImageMemory))
    {
        return false;
    }

    //Create the render pass. I copied this part from VulkanCookbook because I
    //didn't want to make any uneccessary mistakes.
    std::vector<VkAttachmentDescription> attachmentDescriptions =
    {
      {
        0,                                                // VkAttachmentDescriptionFlags     flags
        VK_FORMAT_R8G8B8A8_UNORM,                         // VkFormat                         format
        VK_SAMPLE_COUNT_1_BIT,                            // VkSampleCountFlagBits            samples
        VK_ATTACHMENT_LOAD_OP_CLEAR,                      // VkAttachmentLoadOp               loadOp
        VK_ATTACHMENT_STORE_OP_STORE,                     // VkAttachmentStoreOp              storeOp
        VK_ATTACHMENT_LOAD_OP_DONT_CARE,                  // VkAttachmentLoadOp               stencilLoadOp
        VK_ATTACHMENT_STORE_OP_DONT_CARE,                 // VkAttachmentStoreOp              stencilStoreOp
        VK_IMAGE_LAYOUT_UNDEFINED,                        // VkImageLayout                    initialLayout
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,         // VkImageLayout                    finalLayout
      },
      {
        0,                                                // VkAttachmentDescriptionFlags     flags
        VK_FORMAT_D16_UNORM,                              // VkFormat                         format
        VK_SAMPLE_COUNT_1_BIT,                            // VkSampleCountFlagBits            samples
        VK_ATTACHMENT_LOAD_OP_CLEAR,                      // VkAttachmentLoadOp               loadOp
        VK_ATTACHMENT_STORE_OP_STORE,                     // VkAttachmentStoreOp              storeOp
        VK_ATTACHMENT_LOAD_OP_DONT_CARE,                  // VkAttachmentLoadOp               stencilLoadOp
        VK_ATTACHMENT_STORE_OP_DONT_CARE,                 // VkAttachmentStoreOp              stencilStoreOp
        VK_IMAGE_LAYOUT_UNDEFINED,                        // VkImageLayout                    initialLayout
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,  // VkImageLayout                    finalLayout
      }
    };

    VkAttachmentReference depthStencilAttachment = {
      1,                                                  // uint32_t                             attachment
      VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,   // VkImageLayout                        layout
    };

    std::vector<SubpassParameters> subpassParameters = {
      {
        VK_PIPELINE_BIND_POINT_GRAPHICS,                  // VkPipelineBindPoint                  PipelineType
        {},                                               // std::vector<VkAttachmentReference>   InputAttachments
        {                                                 // std::vector<VkAttachmentReference>   ColorAttachments
          {
            0,                                              // uint32_t                             attachment
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL        // VkImageLayout                        layout
          }
        },
        {},                                               // std::vector<VkAttachmentReference>   ResolveAttachments
        &depthStencilAttachment,                        // const VkAttachmentReference        * DepthStencilAttachment
        {}                                                // std::vector<uint32_t>                PreserveAttachments
      }
    };

    std::vector<VkSubpassDependency> subpassDependencies = {
      {
        0,                                                // uint32_t                 srcSubpass
        VK_SUBPASS_EXTERNAL,                              // uint32_t                 dstSubpass
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,    // VkPipelineStageFlags     srcStageMask
        VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,            // VkPipelineStageFlags     dstStageMask
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,             // VkAccessFlags            srcAccessMask
        VK_ACCESS_SHADER_READ_BIT,                        // VkAccessFlags            dstAccessMask
        0                                                 // VkDependencyFlags        dependencyFlags
      }
    };

    //Create the render pass.
    if(!createRenderPass(logicalDevice, attachmentDescriptions, subpassParameters,
                         subpassDependencies, renderPass))
    {
        return false;
    }

    //Lastly create the framebuffer.
    std::vector<VkImageView> attachments = {colorImageObject.imageView, depthImageObject.imageView};
    if(!createFramebuffer(logicalDevice, renderPass, attachments, width, height, 1, framebuffer))
        return false;

    return true;
}
