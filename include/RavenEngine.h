#pragma once
#include "VulkanInitializer.h"
#include "VulkanWindow.h"
#include "VulkanDevice.h"

//The main class for Raven
using namespace Raven;
class RavenEngine
{
    public:
        ~RavenEngine();
        bool start(const char* appName);
        void stop();
    private:
        //Initializes vulkan dynamic library and loads global-level functions
        bool initializeVulkan();
        //Creates a new vulkan instance
        bool createInstance(VkInstance &instance, char const* const appName, std::vector<const char*> &desiredInstanceExtensions);
        //Selects the physical device that the program is going to use
        bool selectPhysicalDevice(std::vector<VkPhysicalDevice> &physicalDevices,
                                  VkPhysicalDevice &selectedDevice,
                                  std::vector<const char*> &desiredDeviceExtensions);
        //Creates a logical device
        bool createVulkanDevice(VkPhysicalDevice &physicalDevice, std::vector<const char*>  &desiredExtensions);

        //Creates a new VulkanWindow
        bool createWindow(VkPresentModeKHR &presentationMode);

        //Private objects:

        //The vulkan dynamically loaded library
        LIBRARY_TYPE vulkanLibrary;
        //Vulkan variables, the ones starting with "selected"
        //are the ones that the software will be using to complete tasks.
        VkInstance selectedInstance;
        std::vector<VkPhysicalDevice> physicalDevices;
        VkPhysicalDevice selectedPhysicalDevice;
        VkDevice selectedLogicalDevice;

        //Custom objects

        //A pointer to a VulkanDevice class object. Naming this a pointer
        //quarantees correct order of object destruction.
        VulkanDevice* vulkanDevice;

        //Window parameters
        VulkanWindow* appWindow;
        WindowParameters windowParameters;
};
