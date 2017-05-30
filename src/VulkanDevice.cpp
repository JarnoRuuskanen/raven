#include "VulkanDevice.h"
#include "VulkanInitializer.h"
#include "VulkanStructures.h"
#include "VulkanUtility.h"

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

    physicalDevice = device;

    //Initialize all device queues.
    if(!initializeQueues(queueFamilyInfo))
        return false;

    //Create a new queue create info -structure for each queue in the chosen family.
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    for(VulkanQueueInfo info : queueFamilyInfo)
        queueCreateInfos.push_back(VulkanStructures::deviceQueueCreateInfo(info.queueFamilyIndex,info.priorities));

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
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();

    if(!createLogicalDevice(physicalDevice, createInfo, logicalDevice))
        return false;

    //Now that we have a logical device, we should load the device level functions.
    //The logical device will be responsible for performing most of the vulkan application's
    //tasks.
    if(!loadDeviceLevelFunctions(logicalDevice, desiredDeviceExtensions))
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
    for(int i = 0; i < queueFamilies[primaryQueueFamilyIndex].queueCount; i ++)
    {
        //Every queue is as valuable.
        newFamily.priorities.push_back(1.0f);
    }
    familyInfo.push_back(newFamily);

    return true;
}
