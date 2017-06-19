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
 * @brief Reads an image file and creates an image + image view from the file
 *        which can then be used as a texture over the object.
 * @param logicalDevice
 * @param memoryProperties
 * @param filename
 * @param usage
 * @param format
 * @param samples
 * @param mipLevelCount
 * @return False if something went wrong.
 */
bool GraphicsObject::addTexture(const VkDevice logicalDevice,
                                VkPhysicalDeviceMemoryProperties memoryProperties,
                                const std::string filename, VkImageUsageFlags usage, VkFormat format,
                                VkSampleCountFlagBits samples, uint32_t mipLevelCount)
{
    //First load the image data. Note that the data is not being
    //uploaded to the image yet.
    unsigned char* pixels;
    int imageHeight, imageWidth, layerCount;
    FileIO::readImageFile(filename, pixels, imageWidth, imageHeight, layerCount);

    //Create a staging buffer for the image data with host-visible memory.
    VulkanBuffer stagingBuffer;
    stagingBuffer.size = imageWidth * imageHeight * 4;

    VkBufferCreateInfo stagingBufferCreateInfo =
            VulkanStructures::bufferCreateInfo(stagingBuffer.size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                               VK_SHARING_MODE_EXCLUSIVE);
    //Create the staging buffer.
    if(!createBuffer(logicalDevice, stagingBufferCreateInfo, stagingBuffer.buffer))
        return false;

    //Get buffer memory requirements and allocate memory for the buffer.
    VkMemoryRequirements memReq;
    vkGetBufferMemoryRequirements(logicalDevice, stagingBuffer.buffer, &memReq);

    //Find the correct kind of memory.
    uint32_t memoryTypeIndex;
    if(!getMemoryType(memoryProperties, memReq, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                  VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, memoryTypeIndex))
        return false;

    VkDeviceMemory stagingMemory;
    //Allocate the memory.
    if(!allocateMemory(logicalDevice, memReq, memoryTypeIndex, stagingMemory))
        return false;

    //Bind the buffer.
    if(!stagingBuffer.bindBufferMemory(logicalDevice, stagingMemory,0))
        return false;

    //Flush the data to the memory object.
    void* dataPointer;
    flushDataToHostLocalMemory(logicalDevice, stagingMemory, 0, stagingBuffer.size,
                      pixels, &dataPointer, true);

    //Add image extent information.
    VkExtent3D extent;
    extent.width = static_cast<uint32_t>(imageWidth);
    extent.height = static_cast<uint32_t>(imageHeight);
    extent.depth = 1;

    //Next create an image of type VK_IMAGE_TYPE_2D and allocate memory for it.
    VkImageCreateInfo imageInfo =
            VulkanStructures::imageCreateInfo(usage, VK_IMAGE_TYPE_2D,
                                              format, extent,
                                              layerCount,samples, VK_IMAGE_LAYOUT_UNDEFINED,
                                              VK_SHARING_MODE_EXCLUSIVE, mipLevelCount, false);

    //Make sure that the image is possible to work as the destination of data.
    if(!(imageInfo.usage & VK_IMAGE_USAGE_TRANSFER_DST_BIT))
    {
        imageInfo.usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    }

    //Create the image.
    if(!createImage(logicalDevice, imageInfo, textureObject.image))
        return false;

    //Allocate memory for the image. Using the old memReq- and memoryTypeIndex variables.
    vkGetImageMemoryRequirements(logicalDevice, textureObject.image, &memReq);
    if(!getMemoryType(memoryProperties, memReq, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                      memoryTypeIndex))
        return false;

    if(!allocateMemory(logicalDevice, memReq, memoryTypeIndex, textureObject.imageMemory))
        return false;

    //Bind the image to use.
    if(!textureObject.bindImageMemory(logicalDevice, textureObject.imageMemory))
        return false;

    //Create an image view for the image.
}

