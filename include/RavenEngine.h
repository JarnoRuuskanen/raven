#pragma once
#include "VulkanInitializer.h"
//The main class for Raven
class VulkanDevice;
class RavenEngine
{
    public:
        RavenEngine();
        ~RavenEngine();
        bool start();
        void stop();
    private:
        //Initializes vulkan dynamic library and loads global-level functions
        bool initializeVulkan();
        //Creates a new vulkan instance
        bool createInstance(VkInstance &instance, std::vector<const char*> &desiredInstanceExtensions);
        //Selects the physical device that the program is going to use
        bool selectPhysicalDevice(std::vector<VkPhysicalDevice> &physicalDevices,
                                  VkPhysicalDevice &selectedDevice,
                                  std::vector<const char*> &desiredDeviceExtensions);
        //Creates a logical device
        bool createVulkanDevice(VkPhysicalDevice &physicalDevice, std::vector<const char*>  &desiredExtensions);

        //Private objects:

        //The vulkan dynamically loaded library
        LIBRARY_TYPE vulkanLibrary;

        //Vulkan variables, the ones starting with "selected"
        //are the ones that the software will be using to complete tasks.
        VkInstance selectedInstance;
        std::vector<VkPhysicalDevice> physicalDevices;
        VkPhysicalDevice selectedPhysicalDevice;
        VkDevice selectedLogicalDevice;
};
