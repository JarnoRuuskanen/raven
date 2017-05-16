#include "RavenEngine.h"
#include "VulkanDevice.h"

using namespace Raven;

RavenEngine::RavenEngine()
{

}

RavenEngine::~RavenEngine()
{

}

//Starts the raven engine
bool RavenEngine::start()
{
    //First initialize vulkan
    if(!initializeVulkan())
        return false;

    //List desired instance extensions
    std::vector<const char*> desiredInstanceExtensions =
    {
        VK_KHR_SURFACE_EXTENSION_NAME
    };

    //After vulkan dynamic library, exported- and global-level functions have been loaded,
    //create a new vulkan instance.
    if(!createInstance(selectedInstance, desiredInstanceExtensions))
        return false;

    //After instance has been created, load instance level funcions
    if(!loadInstanceLevelVulkanFunctions(selectedInstance, desiredInstanceExtensions))
        return false;

    //Next it is time to choose which Vulkan device (usually a gpu) we are going to use.
    //First check what options we have
    if(!loadPhysicalDevices(selectedInstance, physicalDevices))
        return false;

    //List of desired device extensions
    std::vector<const char*> desiredDeviceExtensions =
    {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    //Select the physical device that the program will be using
    if(!selectPhysicalDevice(physicalDevices, selectedPhysicalDevice, desiredDeviceExtensions))
        return false;

    //Create a logical device out of the selected physical device.
    //Raven will be built to support multiple queues, that will all be used simultaneously
    //with added synchronization. This is to not tax one queue unecessarily
    if(!createVulkanDevice(selectedPhysicalDevice, selectedLogicalDevice))
        return false;

    return true;
}

//Initializes the vulkan library
bool RavenEngine::initializeVulkan()
{
    if(!loadVulkanLibrary(vulkanLibrary))
    {
        std::cout << "Failed to load vulkan dynamic library!" << std::endl;
        return false;
    }
    //Load vulkan functions that are defined in ListOfVulkanFunctions.inl
    if(!loadFunctionExportedFromVulkanLoaderLibrary(vulkanLibrary))
    {
        std::cout << "Failed to load functions exported from vulkan loader library!" << std::endl;
        return false;
    }
    if(!loadGlobalLevelFunctions())
    {
        std::cout << "Failed to load global level functions!" << std::endl;
        return false;
    }
    return true;
}

//Creates a new vulkan instance with the given desired instance extensions
bool RavenEngine::createInstance(VkInstance &instance, std::vector<const char*> &desiredInstanceExtensions)
{
    //Query all available instance extensions
    std::vector<VkExtensionProperties> availableInstanceExtensions;
    if(!checkAvailableInstanceExtensions(availableInstanceExtensions))
    {
        std::cout << "Failed to check for available instance extensions!" << std::endl;
        return false;
    }

    //Create a new vulkan instance
    if(!createVulkanInstance(desiredInstanceExtensions, "Raven", instance))
    {
        std ::cout << "Failed to create a vulkan instance!" << std::endl;
        return false;
    }

    return true;
}

//Selects the first physical device that fills all given requirements
bool RavenEngine::selectPhysicalDevice(std::vector<VkPhysicalDevice> &physicalDevices,
                                       VkPhysicalDevice &selectedDevice,
                                       std::vector<const char*> &desiredDeviceExtensions)
{
    //Check if desired extensions are supported by the physical devices. Ditch those
    //that don't support the required extensions.
    for(int i = 0; i < physicalDevices.size(); i++)
    {
        if(arePhysicalDeviceExtensionsSupported(physicalDevices[i], desiredDeviceExtensions))
        {
            //We will be using the first physical device that passes all requirements given to it.
            selectedDevice = physicalDevices[i];
            return true;
        }
    }
    std::cout << "Failed to find a physical device that supports all the required device extensions!" << std::endl;
    return false;
}

//Creates a logical vulkan device from given physical device
bool RavenEngine::createVulkanDevice(VkPhysicalDevice &physicalDevice, VkDevice &device)
{
    VulkanDevice logicalDevice(physicalDevice);
    if(!logicalDevice.initializeDevice())
        return false;
    return true;
}
