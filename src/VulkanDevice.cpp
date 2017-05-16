#include "VulkanDevice.h"
#include "VulkanInitializer.h"
#include "VulkanStructures.h"

//Remember to use the Raven namespace
using namespace Raven;

VulkanDevice::VulkanDevice(VkPhysicalDevice &device)
{
    physicalDevice = device;
}

VulkanDevice::~VulkanDevice()
{

}

//Initializes the vulkan device
bool VulkanDevice::initializeDevice()
{
    //Get the device features and properties. Note that features must be implicitly enabled,
    //while creting the logical device, they are not enabled by default.
    VkPhysicalDeviceFeatures features;
    VkPhysicalDeviceProperties properties;
    getPhysicalDeviceFeaturesAndProperties(physicalDevice, features, properties);

    //After this we query every available queue family and find the one that suits our needs.

    //Get all queue families of the physical device.
    if(!getPhysicalDeviceQueuesWithProperties(physicalDevice, queueFamilies))
        return false;

    //The graphics queue family
    if(!getQueueFamilyIndex(queueFamilies, VK_QUEUE_GRAPHICS_BIT, graphicsFamilyIndex))
        return false;

    //Compute queue family index
    if(!getQueueFamilyIndex(queueFamilies, VK_QUEUE_COMPUTE_BIT, computeFamilyIndex))
        return false;

    //Transfer queue family index
    if(!getQueueFamilyIndex(queueFamilies, VK_QUEUE_TRANSFER_BIT, transferFamilyIndex))
        return false;

    //Sparse queue family index
    if(!getQueueFamilyIndex(queueFamilies, VK_QUEUE_SPARSE_BINDING_BIT, sparseFamilyIndex))
        return false;

    if(graphicsFamilyIndex == computeFamilyIndex == transferFamilyIndex == sparseFamilyIndex)
    {
        std::cout << "Same queue index for every operation: " << graphicsFamilyIndex << std::endl;
        std::cout << "Available queues:" << queueFamilies[graphicsFamilyIndex].queueCount << std::endl;
    }

    VkDeviceCreateInfo createInfo = VulkanStructures::deviceCreateInfo();
    createInfo.enabledExtensionCount = 0;
    createInfo.ppEnabledExtensionNames = nullptr;
    createInfo.enabledLayerCount = 0;
    createInfo.ppEnabledLayerNames = nullptr;

    if(!createLogicalDevice(physicalDevice, createInfo, logicalDevice))
        return false;

    return true;
}
