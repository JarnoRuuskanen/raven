#include "RavenEngine.h"

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
    {
        std::cout << "Failed to initialize Vulkan, exiting." << std::endl;
        return false;
    }

    //Query all available instance extensions
    std::vector<VkExtensionProperties> extensions;
    if(!checkAvailableInstanceExtensions(extensions))
    {
        std::cout << "Failed to check for available instance extensions!" << std::endl;
        return false;
    }

    //List desired extensions
    std::vector<const char*> desiredExtensions =
    {
        VK_KHR_SURFACE_EXTENSION_NAME
    };

    //Create a new vulkan instance
    if(!createVulkanInstance(desiredExtensions, "Raven", vulkanInstance))
    {
        std ::cout << "Failed to create a vulkan instance!" << std::endl;
        return false;
    }

    //After instance has been created, load instance level funcions
    if(!loadInstanceLevelVulkanFunctions(vulkanInstance, desiredExtensions))
    {
        std::cout << "Failed to load instance level functions!" << std::endl;
        return false;
    }

    //Next it is time to choose which Vulkan device (usually a gpu) we are going to use
    return true;
}

//Initializes the vulkan library
bool RavenEngine::initializeVulkan()
{
    if(!loadVulkanLibrary(vulkanLibrary))
        return false;
    //Load vulkan functions that are defined in ListOfVulkanFunctions.inl
    if(!loadFunctionExportedFromVulkanLoaderLibrary(vulkanLibrary))
        return false;
    if(!loadGlobalLevelFunctions())
        return false;

    return true;
}
