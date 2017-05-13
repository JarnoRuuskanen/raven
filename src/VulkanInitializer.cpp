#include "VulkanInitializer.h"

//Loads the dynamic vulkan library.
namespace Raven
{
    bool loadVulkanLibrary(LIBRARY_TYPE& vulkan_library)
    {
        bool success = false;
        #if defined _WIN32
            vulkan_library = LoadLibrary("vulkan-1.dll");
        #elif defined __linux
            vulkan_library = dlopen("libvulkan.so.1", RTLD_NOW);
        #endif

        if(vulkan_library == nullptr)
        {
            std::cout << "Failed to load vulkan library, exiting." << std::endl;
        }
        else
        {
            std::cout << "Vulkan library loaded" << std::endl;
            success = true;
        }
        return success;
    }
}
