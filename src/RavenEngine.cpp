#include "RavenEngine.h"

using namespace Raven;

RavenEngine::RavenEngine()
{

}

RavenEngine::~RavenEngine()
{

}

//Starts the raven engine
void RavenEngine::start()
{
    //First initialize vulkan
    bool success;
    success = initializeVulkan();

    std::string result = success ? "Successfully initialized Vulkan!" : "Failed to initialize Vulkan!";
    std::cout << result << std::endl;

    //Create a new instance
    //First query all available extensions
    std::vector<VkExtensionProperties> extensions;
    if(!checkAvailableInstanceExtensions(extensions))
        std::cout << "Failed to check for available instance extensions!" << std::endl;

    //List desired extensions
    std::vector<const char*> desiredExtensions =
    {
        VK_KHR_SURFACE_EXTENSION_NAME
    };

    //Create a new vulkan instance
    VkInstance instance;
    if(!createVulkanInstance(desiredExtensions, "Raven", instance))
        std ::cout << "Failed to create a vulkan instance!" << std::endl;

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
