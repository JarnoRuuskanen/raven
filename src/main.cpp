#include <iostream>
#include "VulkanInitializer.h"
#include "VulkanStructures.h"

using namespace Raven;
bool init(LIBRARY_TYPE vulkanLibrary)
{
    if(!loadVulkanLibrary(vulkanLibrary))
        return false;
    //Load vulkan functions that are defined in ListOfVulkanFunctions.inl
    if(!loadFunctionExportedFromVulkanLoaderLibrary(vulkanLibrary))
        return false;
    if(!loadGlobalLevelFunctions())
        return false;

    std::vector<VkExtensionProperties> extensions;
    if(!checkAvailableInstanceExtensions(extensions))
        return false;

    return true;
}

int main()
{
    //List desired instance extensions
    std::vector<char const*> desiredInstanceExtensions =
    {
        ""
    };

    //First load the vulkan library dynamically
    LIBRARY_TYPE vulkanLibrary;
    bool success = init(vulkanLibrary);
    std::string result = success ? "Successfully initialized Vulkan!" : "Failed to initialize Vulkan!";
    std::cout << result << std::endl;
    return 0;
}
