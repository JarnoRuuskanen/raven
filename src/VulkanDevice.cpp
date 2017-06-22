#include "VulkanDevice.h"
#include "VulkanInitializer.h"
#include "VulkanStructures.h"
#include "VulkanUtility.h"
#include "CommandBufferManager.h"

//Using the Raven namespace.
using namespace Raven;

/**
 * @brief VulkanDevice::VulkanDevice
 */
VulkanDevice::VulkanDevice()
{
    logicalDevice = VK_NULL_HANDLE;
}

/**
 * @brief VulkanDevice::~VulkanDevice
 */
VulkanDevice::~VulkanDevice()
{
    //Destroy the logical device. After this we can no longer
    //use the device level functions that were loaded when this
    //logical device was created.
    if(logicalDevice != VK_NULL_HANDLE)
    {
        vkDestroyDevice(logicalDevice, nullptr);
        logicalDevice = VK_NULL_HANDLE;
    }
}

/**
 * @brief Initializes the vulkan device.
 * @param device
 * @param desiredDeviceExtensions
 * @return
 */
bool VulkanDevice::initializeDevice(VkPhysicalDevice &device,
                                    std::vector<const char*>  &desiredDeviceExtensions)
{
    if(device == VK_NULL_HANDLE)
    {
        std::cerr << "Failed to initialize VulkanDevice due to no valid physical device being presented!" << std::endl;
        return false;
    }

    //Save the physical device and query it's memory properties.
    physicalDevice = device;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &physicalDeviceMemoryProperties);

    //Initialize all device queues.
    if(!initializeQueues(queueFamilyInfo))
        return false;

    //Create a new queue create info -structure for each queue in the chosen family.
    VulkanQueueInfo chosenQueueFamily = queueFamilyInfo[primaryQueueFamilyIndex];
    VkDeviceQueueCreateInfo queueInfo = {};
    queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueInfo.flags = 0;
    queueInfo.pNext = nullptr;
    queueInfo.pQueuePriorities = chosenQueueFamily.priorities.data();
    queueInfo.queueFamilyIndex = chosenQueueFamily.queueFamilyIndex;
    queueInfo.queueCount = static_cast<uint32_t>(chosenQueueFamily.priorities.size());

    //Get the device features and properties. Note that features must be implicitly enabled,
    //while creating the logical device, they are not enabled by default.
    VkPhysicalDeviceFeatures features;
    //Physical device properties variable won't be used at this point.
    VkPhysicalDeviceProperties properties;
    getPhysicalDeviceFeaturesAndProperties(physicalDevice, features, properties);

    //Build the device create info
    VkDeviceCreateInfo createInfo = VulkanStructures::deviceCreateInfo();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(desiredDeviceExtensions.size());
    createInfo.ppEnabledExtensionNames = desiredDeviceExtensions.size() > 0 ?
                                            desiredDeviceExtensions.data() : nullptr;
    createInfo.enabledLayerCount = 0;
    createInfo.ppEnabledLayerNames = nullptr;
    //Enable all features the graphics card supports for now. This is not ideal for optimization.
    createInfo.pEnabledFeatures = &features;
    //Device queues are created when the logical device is created.
    createInfo.queueCreateInfoCount = 1;
    createInfo.pQueueCreateInfos = &queueInfo;

    if(!createLogicalDevice(physicalDevice, createInfo, logicalDevice))
        return false;

    //Now that we have a logical device, we should load the device level functions.
    //The logical device will be responsible for performing most of the vulkan application's
    //tasks.
    if(!loadDeviceLevelFunctions(logicalDevice, desiredDeviceExtensions))
        return false;

    //After device level functions have been loaded, save the logical device queue handles so
    //that the vulkan device can actually be used to submit commands into the graphics card.
    getQueueFamilyQueues(logicalDevice, chosenQueueFamily.queueFamilyIndex,
                         static_cast<uint32_t>(chosenQueueFamily.priorities.size()),
                         deviceQueueHandles);
    if(deviceQueueHandles.size() < chosenQueueFamily.priorities.size())
    {
        std::cerr << "Failed to store logical device queue handles!" << std::endl;
        return false;
    }

    return true;
}

//Sends commands to the gpu for computing. This function also chooses the
//queue which the commands will be submitted to.
bool VulkanDevice::executeCommands(VkSubmitInfo &submitInfo, VkFence &submitFence)
{
    //Submit:
    VkQueue selectedQueue = deviceQueueHandles[0];
    if(!CommandBufferManager::submitCommandBuffers(selectedQueue, 1, submitInfo, submitFence))
    {
        return false;
    }
    return true;
}

/**
 * @brief Creates a sampled image.
 * @param imageType
 * @param format
 * @param size
 * @param numMipmaps
 * @param numLayers
 * @param usage
 * @param viewType
 * @param aspect
 * @param linearFiltering
 * @param sampledImageObject
 * @param memoryObject
 * @return
 */
bool VulkanDevice::createSampledImage(VkImageType imageType,
                                      VkFormat format,
                                      VkExtent3D size,
                                      uint32_t numMipmaps,
                                      uint32_t numLayers,
                                      VkImageUsageFlags usage,
                                      VkImageViewType viewType,
                                      VkImageAspectFlags aspect,
                                      VkBool32 linearFiltering,
                                      VulkanImage &sampledImageObject,
                                      VkDeviceMemory &memoryObject)
{
    //Check that the given format supports image sampling.
    VkFormatProperties formatProperties;
    if(!doesFormatSupportRequiredFeature(physicalDevice,
                                         format,
                                         VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT,
                                         formatProperties))
    {
        return false;
    }

    if(linearFiltering && !(formatProperties.optimalTilingFeatures
                            & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
    {
        std::cerr << "Linear image filtering does not support provided format." << std::endl;
        return false;
    }

    VkImageCreateInfo imageCreateInfo =
            VulkanStructures::imageCreateInfo(usage | VK_IMAGE_USAGE_SAMPLED_BIT,
                                              imageType, format, size, numLayers,
                                              VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_LAYOUT_UNDEFINED,
                                              VK_SHARING_MODE_EXCLUSIVE, numMipmaps, false);
    if(!createImage(logicalDevice, imageCreateInfo, sampledImageObject.image))
        return false;

    //Find correct type of memory for the image and bind it.
    VkMemoryRequirements memReq;
    vkGetImageMemoryRequirements(logicalDevice, sampledImageObject.image, &memReq);

    allocateMemory(logicalDevice,physicalDeviceMemoryProperties,
                   memReq, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, memoryObject);

    if(!sampledImageObject.bindMemoryObject(logicalDevice, memoryObject))
        return false;

    VkImageViewCreateInfo imageViewInfo =
            VulkanStructures::imageViewCreateInfo(sampledImageObject.image, format, aspect, viewType);

    //Lastly create the image view.
    if(!createImageView(logicalDevice, imageViewInfo, sampledImageObject.imageView))
        return false;

    return true;
}

/**
 * @brief Creates a sampler and a sampled image.
 * @param samplerInfo
 * @param imageType
 * @param format
 * @param imageSize
 * @param numMipmaps
 * @param numLayers
 * @param usage
 * @param viewType
 * @param aspect
 * @param sampler
 * @param sampledImageMemory
 * @param sampledImageObject
 * @return False if either one of the operations fails to complete.
 */
bool VulkanDevice::createCombinedImageSampler(VkSamplerCreateInfo samplerInfo,
                                              VkImageType imageType,
                                              VkFormat format,
                                              VkExtent3D imageSize,
                                              uint32_t numMipmaps,
                                              uint32_t numLayers,
                                              VkImageUsageFlags usage,
                                              VkImageViewType viewType,
                                              VkImageAspectFlags aspect,
                                              VkSampler &sampler,
                                              VkDeviceMemory &sampledImageMemory,
                                              VulkanImage &sampledImageObject)
{
    //First create a sampler.
    if(!createSampler(logicalDevice, samplerInfo, sampler))
        return false;

    //Check if we are using linear filtering.
    bool linearFiltering = (samplerInfo.magFilter == VK_FILTER_LINEAR) ||
                           (samplerInfo.minFilter == VK_FILTER_LINEAR) ||
                           (samplerInfo.mipmapMode == VK_SAMPLER_MIPMAP_MODE_LINEAR);

    //Create the sampled image.
    if(!createSampledImage(imageType, format, imageSize, numMipmaps, numLayers, usage,
                           viewType, aspect, linearFiltering, sampledImageObject, sampledImageMemory))
    {
        return false;
    }
    return true;
}

/**
 * @brief Creates a storage image, which can be used for loading unfiltered data
 *        from pipeline-bound images or for storing data from shaders.
 * @param imageType
 * @param format The storage format.
 * @param imageSize
 * @param numMipmaps
 * @param numLayers
 * @param usage
 * @param viewType
 * @param aspect
 * @param atomicOperations
 * @param storageImage
 * @param memoryObject
 * @return False if the storage image could not be created.
 */
bool VulkanDevice::createStorageImage(VkImageType imageType,
                                      VkFormat format,
                                      VkExtent3D imageSize,
                                      uint32_t numMipmaps,
                                      uint32_t numLayers,
                                      VkImageUsageFlags usage,
                                      VkImageViewType viewType,
                                      VkImageAspectFlags aspect,
                                      VkBool32 atomicOperations,
                                      VulkanImage &storageImage,
                                      VkDeviceMemory &memoryObject)
{
    //Check that the selected format supports storaging feature.
    VkFormatProperties formatProperties;
    if(!doesFormatSupportRequiredFeature(physicalDevice, format, VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT,
                                         formatProperties))
    {
        return false;
    }

    //If we want to use atomic operations, check that atomic feature is supported.
    if(atomicOperations &&
            !(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT))
    {
        std::cerr << "Provided format is not supported for atomic operations on storage images!"
                  << std::endl;
        return false;
    }

    //Create the image.
    VkImageCreateInfo imageCreateInfo =
            VulkanStructures::imageCreateInfo(usage | VK_IMAGE_USAGE_STORAGE_BIT,
                                              imageType, format, imageSize, numLayers,
                                              VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_LAYOUT_UNDEFINED,
                                              VK_SHARING_MODE_EXCLUSIVE, numMipmaps, false);

    if(!createImage(logicalDevice, imageCreateInfo, storageImage.image))
        return false;

    //Allocate memory for the image.
    VkMemoryRequirements memReq;
    vkGetImageMemoryRequirements(logicalDevice, storageImage.image, &memReq);

   if(!allocateMemory(logicalDevice, physicalDeviceMemoryProperties, memReq,
                      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, memoryObject))
   {
       return false;
   }

   //Bind the image memory to use.
   storageImage.bindMemoryObject(logicalDevice, memoryObject);

   //Create the image view.
   VkImageViewCreateInfo viewCreateInfo =
           VulkanStructures::imageViewCreateInfo(storageImage.image, format, aspect, viewType);

   if(!createImageView(logicalDevice, viewCreateInfo, storageImage.imageView))
        return false;

   return true;
}

/**
 * @brief Creates a storage buffer, which is used for reading from buffers
          inside shaders and for storing data.
 * @param usage
 * @param bufferSize
 * @param storageBuffer
 * @param storageMemoryObject
 * @return False if storage buffer creation fails for any reason.
 */
bool VulkanDevice::createStorageBuffer(VkBufferUsageFlags usage,
                                       VkDeviceSize bufferSize,
                                       VulkanBuffer &storageBuffer,
                                       VkDeviceMemory &storageMemoryObject)
{
    //Create the storage buffer.
    VkBufferCreateInfo createInfo =
            VulkanStructures::bufferCreateInfo(bufferSize,
                                               usage | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                                               VK_SHARING_MODE_EXCLUSIVE);
    if(!createBuffer(logicalDevice, createInfo, storageBuffer.buffer))
        return false;

    //Allocate memory.
    VkMemoryRequirements memReq;
    vkGetBufferMemoryRequirements(logicalDevice, storageBuffer.buffer, &memReq);

    if(!allocateMemory(logicalDevice, physicalDeviceMemoryProperties,
                       memReq, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, storageMemoryObject))
    {
        return false;
    }
    //Bind the memory.
    if(!storageBuffer.bindMemoryObject(logicalDevice, storageMemoryObject, 0))
        return false;

    return true;

}

/**
 * @brief Creates a uniform texel buffer.
 * @param format
 * @param bufferSize
 * @param usage
 * @param uniformTexelBufferObject
 * @param memoryObject
 * @return False if the texel buffer could not be created.
 */
bool VulkanDevice::createUniformTexelBuffer(VkFormat format,
                                            VkDeviceSize bufferSize,
                                            VkImageUsageFlags usage,
                                            VulkanBuffer &uniformTexelBufferObject,
                                            VkDeviceMemory &memoryObject)
{
    //Check that the chosen format supports the required feature.
    VkFormatProperties formatProperties;
    if(!doesFormatSupportRequiredFeature(physicalDevice,
                                         format,
                                         VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT,
                                         formatProperties))
    {
        return false;
    }

    //Create the uniform texel buffer.
    VkBufferCreateInfo bufferInfo =
            VulkanStructures::bufferCreateInfo(bufferSize,
                                               usage | VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT,
                                               VK_SHARING_MODE_EXCLUSIVE);

    if(!createBuffer(logicalDevice, bufferInfo, uniformTexelBufferObject.buffer))
        return false;

    //Allocate and bind the memory for the buffer.
    VkMemoryRequirements memReq;
    vkGetBufferMemoryRequirements(logicalDevice, uniformTexelBufferObject.buffer, &memReq);

    if(!allocateMemory(logicalDevice, physicalDeviceMemoryProperties,
                       memReq, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, memoryObject))
    {
        return false;
    }
    //Bind the buffer.
    if(!uniformTexelBufferObject.bindMemoryObject(logicalDevice, memoryObject, 0))
        return false;

    //Create the buffer view.
    VkBufferViewCreateInfo viewCreateInfo =
            VulkanStructures::bufferViewCreateInfo(uniformTexelBufferObject.buffer, format,
                                                   0, VK_WHOLE_SIZE);

    if(!createBufferView(logicalDevice, viewCreateInfo, uniformTexelBufferObject.bufferView))
        return false;

    return true;
}

/**
 * @brief Creates a storage texel buffer, which can be used for reading and storing
          large amounts of image-like data among other things.
 * @param format
 * @param bufferSize
 * @param usage
 * @param atomicOperations
 * @param storageTexelBuffer
 * @param memoryObject
 * @return False if storage texel buffer could not be created.
 */
bool VulkanDevice::createStorageTexelBuffer(VkFormat format,
                                            VkDeviceSize bufferSize,
                                            VkImageUsageFlags usage,
                                            VkBool32 atomicOperations,
                                            VulkanBuffer &storageTexelBuffer,
                                            VkDeviceMemory &memoryObject)
{
    //First check if the format supports required feature.
    VkFormatProperties formatProperties;
    if(!doesFormatSupportRequiredFeature(physicalDevice, format,
                                         VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT,
                                         formatProperties))
    {
        return false;
    }

    //If atomic operations are required, check if format supports required feature.
    if(atomicOperations &&
            !(formatProperties.bufferFeatures & VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT))
    {
        std::cerr << "Given format is not supported for atomic operations on "
                     "storage texel buffers!" << std::endl;
        return false;
    }

    //Create the storage texel buffer.
    VkBufferCreateInfo bufferInfo =
            VulkanStructures::bufferCreateInfo(bufferSize,
                                               usage | VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT,
                                               VK_SHARING_MODE_EXCLUSIVE);
    if(!createBuffer(logicalDevice, bufferInfo, storageTexelBuffer.buffer))
        return false;

    //Allocate memory for the buffer and bind it.
    VkMemoryRequirements memReq;
    vkGetBufferMemoryRequirements(logicalDevice, storageTexelBuffer.buffer, &memReq);

    if(!allocateMemory(logicalDevice, physicalDeviceMemoryProperties,
                       memReq, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                       memoryObject))
    {
        return false;
    }
    //Bind the memory.
    storageTexelBuffer.bindMemoryObject(logicalDevice, memoryObject, 0);

    //Create the buffer view.
    VkBufferViewCreateInfo viewInfo =
            VulkanStructures::bufferViewCreateInfo(storageTexelBuffer.buffer,
                                                   format, 0, VK_WHOLE_SIZE);
    if(!createBufferView(logicalDevice, viewInfo, storageTexelBuffer.bufferView))
        return false;

    return true;
}

/**
 * @brief Creates an uniform buffer, which is used to
          provide values for read-only uniform variables inside shaders.
 * @param bufferSize
 * @param usage
 * @param uniformBufferObject
 * @param memoryObject
 * @return
 */
bool VulkanDevice::createUniformBuffer(VkDeviceSize bufferSize,
                                       VkBufferUsageFlags usage,
                                       VulkanBuffer &uniformBufferObject,
                                       VkDeviceMemory &memoryObject)
{
    //First create the uniform buffer.
    VkBufferCreateInfo bufferInfo =
            VulkanStructures::bufferCreateInfo(bufferSize,
                                               usage | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                               VK_SHARING_MODE_EXCLUSIVE);
    if(!createBuffer(logicalDevice, bufferInfo, uniformBufferObject.buffer))
        return false;

    //Allocate memory for the uniform buffer.
    VkMemoryRequirements memReq;
    vkGetBufferMemoryRequirements(logicalDevice, uniformBufferObject.buffer, &memReq);

    if(!allocateMemory(logicalDevice, physicalDeviceMemoryProperties,
                       memReq, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                       memoryObject))
    {
        return false;
    }
    //Bind the memory.
    if(!uniformBufferObject.bindMemoryObject(logicalDevice, memoryObject, 0))
        return false;

    return true;
}

/**
 * @brief Initializes the queues this logical vulkan device will be using.
 * @param familyInfo
 * @return
 */
bool VulkanDevice::initializeQueues(std::vector<VulkanQueueInfo> &familyInfo)
{
    familyInfo.clear();
    //Get all queue families of the physical device.
    if(!getPhysicalDeviceQueuesWithProperties(physicalDevice, queueFamilies))
        return false;

    //Find a queue family that suits our needs.
    if(!getQueueFamilyIndex(queueFamilies,
                            VK_QUEUE_GRAPHICS_BIT |
                            VK_QUEUE_COMPUTE_BIT  |
                            VK_QUEUE_TRANSFER_BIT |
                            VK_QUEUE_SPARSE_BINDING_BIT,
                            primaryQueueFamilyIndex))
        return false;

    //Store the chosen queue family information into a vector.
    VulkanQueueInfo  newFamily;
    newFamily.queueFamilyIndex = primaryQueueFamilyIndex;
    //Track all queues in the family.
    for(int i = 0; i < queueFamilies[primaryQueueFamilyIndex].queueCount; i++)
    {
        //Every queue is as valuable.
        newFamily.priorities.push_back(1.0f);
    }
    familyInfo.push_back(newFamily);

    return true;
}
