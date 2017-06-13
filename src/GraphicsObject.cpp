#include "GraphicsObject.h"
#include "VulkanStructures.h"
#include "VulkanUtility.h"
#include "FileIO.h"

using namespace Raven;
GraphicsObject::GraphicsObject()
{

}

GraphicsObject::~GraphicsObject()
{

}

/**
 * @brief Loads the given file and creates a vertex buffer from it.
 * @param filename
 * @return False if something went wrong.
 */
bool GraphicsObject::loadFrame(const VkDevice logicalDevice, const std::string filename)
{
    return true;
}

/**
 * @brief Adds a texture for the graphics object by creating an image and
 *        and image view.
 * @param filename
 * @return
 */
bool GraphicsObject::addTexture(const VkDevice logicalDevice, const std::string filename,
                                VkImageUsageFlags usage, VkFormat format,
                                VkSampleCountFlagBits samples, uint32_t mipLevelCount)
{
    //First load the image data.
    unsigned char* data;
    int height, width, layerCount;
    FileIO::readImageFile(filename, data, width, height, layerCount);

    //Add image extent information.
    VkExtent3D extent;
    extent.width = static_cast<uint32_t>(width);
    extent.height = static_cast<uint32_t>(height);
    extent.depth = 1;

    //Next create an image of type VK_IMAGE_TYPE_2D and allocate memory for it.
    VkImageCreateInfo imageInfo =
            VulkanStructures::imageCreateInfo(usage, VK_IMAGE_TYPE_2D,
                                              format, extent,
                                              layerCount,samples, VK_IMAGE_LAYOUT_UNDEFINED,
                                              VK_SHARING_MODE_EXCLUSIVE, mipLevelCount, false);
    createImage(logicalDevice, imageInfo, textureObject.image);

    //Create an image view for the image.
}

