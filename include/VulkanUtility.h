#pragma once
#include "Headers.h"

//List of all vulkan utility functions used by the Raven application
namespace Raven
{
    //Checks available instance extensions.
    bool checkAvailableInstanceExtensions(std::vector<VkExtensionProperties> &availableExtensions);
    //Checks if a given extension is found in a list of extensions.
    bool isExtensionSupported(std::vector<VkExtensionProperties> &availableExtensions,
                              const char *desiredProperty);
    //Creates a new vulkan instance.
    bool createVulkanInstance(std::vector<char const*> const& desiredExtensions,
                              char const* const appName, VkInstance &instance);
    //Enumerates through all physical, vulkan-capable devices and stores them inside a vector.
    bool loadPhysicalDevices(VkInstance &instance, std::vector<VkPhysicalDevice> &physicalDevices);
    //Checks if a physical device supports a given extension.
    bool arePhysicalDeviceExtensionsSupported(VkPhysicalDevice &physicalDevice,
                                              std::vector<char const*> const& desiredExtensions);
    //Creates a vulkan logical device.
    bool createLogicalDevice(VkPhysicalDevice &physicalDevice,
                             VkDeviceCreateInfo createInfo,
                             VkDevice &logicalDevice);
    //Checks physical device features.
    void getPhysicalDeviceFeaturesAndProperties(VkPhysicalDevice &physicalDevice,
                                                VkPhysicalDeviceFeatures& features,
                                                VkPhysicalDeviceProperties& properties);
    //Gets the index of a desired queue family/families
    bool getQueueFamilyIndex(std::vector<VkQueueFamilyProperties> &queueFamilies,
                             VkQueueFlags desiredQueueFamily,
                             uint32_t &queueFamilyIndex);
    //Gets all physical device queues and their properties
    bool getPhysicalDeviceQueuesWithProperties(VkPhysicalDevice &physicalDevice,
                                               std::vector<VkQueueFamilyProperties> &queueFamilies);
}
