#include "VulkanInitializer.h"
#include "VulkanFunctions.h"
#include "VulkanStructures.h"
#include <string>

//Vulkan initialization inside the Raven namespace. This namespace
//is heavily based on VulkanCookbook examples.
namespace Raven
{
    //Loads the dynamic vulkan library.
    bool loadVulkanLibrary(LIBRARY_TYPE& vulkanLibrary)
    {
        bool success = false;
        #if defined _WIN32
            vulkan_library = LoadLibrary("vulkan-1.dll");
        #elif defined __linux
            vulkanLibrary = dlopen("libvulkan.so.1", RTLD_NOW);
        #endif

        if(vulkanLibrary != nullptr)
            success = true;

        return success;
    }

    //Loads a vulkan function
    bool loadFunctionExportedFromVulkanLoaderLibrary(LIBRARY_TYPE const &vulkanLibrary)
    {
        //First define OS specific load function, which
        //acquires a function pointer.
        #if defined _WIN32
            #define LoadFunction GetProcAddress
        #elif defined __linux
            #define LoadFunction dlsym
        #endif

        #define EXPORTED_VULKAN_FUNCTION(name)                                                      \
        name = (PFN_##name)LoadFunction(vulkanLibrary, #name);                                      \
        if(name==nullptr)                                                                           \
        {                                                                                           \
            std::cout << "Failed to load exported Vulkan function called: "                         \
            #name << std::endl;                                                                     \
            return false;                                                                           \
        }
        #include "ListOfVulkanFunctions.inl"
        return true;
    }

    //Loads global level functions. There are currently three such functions.
    bool loadGlobalLevelFunctions()
    {
        #define GLOBAL_LEVEL_VULKAN_FUNCTION(name)                                                  \
        name = (PFN_##name)vkGetInstanceProcAddr(nullptr, #name);                                   \
        if(name == nullptr)                                                                         \
        {                                                                                           \
            std::cout << "Failed to load global level Vulkan function called: "                     \
            #name << std::endl;                                                                     \
            return false;                                                                           \
        }
        #include "ListOfVulkanFunctions.inl"
        return true;
    }

    //Loads an instance level vulkan functions
    bool loadInstanceLevelVulkanFunctions(VkInstance &instance,
                                         std::vector<const char*> const& enabledExtensions)
    {
        //First load instance level functions
        #define INSTANCE_LEVEL_VULKAN_FUNCTION(name)                                                \
        name = (PFN_##name)vkGetInstanceProcAddr(instance,#name);                                   \
        if(name == nullptr)                                                                         \
        {                                                                                           \
            std::cout << "Failed to load instance level Vulkan function called: "                   \
                #name << std::endl;                                                                 \
                return false;                                                                       \
        }

        //Next load instance level functions from extensions
        #define INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(name, extension)                      \
        for(auto& enabledExtension : enabledExtensions)                                             \
        {                                                                                           \
            if(std::string(enabledExtension) == std::string(extension))                             \
            {                                                                                       \
                name = (PFN_##name)vkGetInstanceProcAddr(instance, #name);                          \
                if(name == nullptr)                                                                 \
                {                                                                                   \
                    std::cout << "Failed to load instance level function from extension called: "   \
                    #name << std::endl;                                                             \
                    return false;                                                                   \
                }                                                                                   \
            }                                                                                       \
        }
        #include "ListOfVulkanFunctions.inl"
        return true;
    }

    //Loads logical device level functions
    bool loadDeviceLevelFunctions(VkDevice &device, std::vector<const char*>  &enabledExtensions)
    {
        //First define device level functions
        #define DEVICE_LEVEL_VULKAN_FUNCTION(name)                                                  \
        name = (PFN_##name)vkGetDeviceProcAddr(device,#name);                                       \
        if(name == nullptr)                                                                         \
        {                                                                                           \
            std::cout << "Failed to load device level function called: "                            \
            #name << std::endl;                                                                     \
            return false;                                                                           \
        }

        //Next create a macro for loading the device level functions from extensions
        #define DEVICE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(name, extension)                        \
        for(auto& enabledExtension : enabledExtensions)                                             \
        {                                                                                           \
            if(std::string(enabledExtension) == std::string(extension))                             \
            {                                                                                       \
                name = (PFN_##name)vkGetDeviceProcAddr(device, #name);                              \
                if(name == nullptr)                                                                 \
                {                                                                                   \
                    std::cout << "Failed to load device level function from extension called: "     \
                    #name << std::endl;                                                             \
                    return false;                                                                   \
                }                                                                                   \
            }                                                                                       \
        }
        #include "ListOfVulkanFunctions.inl"
        return true;
    }
}
