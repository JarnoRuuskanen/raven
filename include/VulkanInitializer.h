#pragma once
#include "Headers.h"

//Raven namespace
namespace Raven
{
    //Loads the vulkan library. This way you don't need to
    //worry about static linking. vulkan.h defined prototypes
    //are disabled and function pointers are loaded dynamically
    //into the application. This has some performance advantages
    //as we don't need to load the whole library but only those
    //parts that we need for our application.
    bool loadVulkanLibrary(LIBRARY_TYPE& vulkanLibrary);
    //Loads functions exported from vulkan loader library.
    bool loadFunctionExportedFromVulkanLoaderLibrary(LIBRARY_TYPE const &vulkanLibrary);
    //Loads the global level functions described in the list of vulkan functions.
    bool loadGlobalLevelFunctions();
    //Checks available instance extensions.
    bool checkAvailableInstanceExtensions(std::vector<VkExtensionProperties> &availableExtensions);
    //Checks if a given extension is found in a list of extensions
    bool isExtensionSupported(std::vector<VkExtensionProperties> &availableExtensions, const char *desiredProperty);
    //Creates a new vulkan instance
    bool createVulkanInstance(std::vector<char const*> const& desiredExtensions,char const* const appName, VkInstance &instance);
    //Loads instance level vulkan functions
    bool loadInstanceLevelVulkanFunctions(VkInstance &instance, std::vector<const char*> const &enabledExtensions);
    //Enumerates through all physical, vulkan-capable devices and stores them inside a vector
    bool loadPhysicalDevices(VkInstance &instance, std::vector<VkPhysicalDevice> &physicalDevices);
    //Checks if a physical device supports a given extension
    bool arePhysicalDeviceExtensionsSupported(VkPhysicalDevice &physicalDevice, std::vector<char const*> const& desiredExtensions);
    //Creates a vulkan logical device
    bool createLogicalDevice(VkInstance &instance, VkDevice &logicalDevice);
}

