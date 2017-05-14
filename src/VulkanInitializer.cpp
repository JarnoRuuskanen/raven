#include "VulkanInitializer.h"
#include "VulkanFunctions.h"
#include "VulkanStructures.h"
#include <string>

//Vulkan initialization inside the Raven namespace
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

        #define EXPORTED_VULKAN_FUNCTION(name)                              \
        name = (PFN_##name)LoadFunction(vulkanLibrary, #name);              \
        if(name==nullptr)                                                   \
        {                                                                   \
            std::cout << "Failed to load exported Vulkan function called: " \
            #name << std::endl;                                             \
            return false;                                                   \
        }
        #include "ListOfVulkanFunctions.inl"
        return true;
    }

    //Loads global level functions. There are currently three such functions.
    bool loadGlobalLevelFunctions()
    {
        #define GLOBAL_LEVEL_VULKAN_FUNCTION(name)                              \
        name = (PFN_##name)vkGetInstanceProcAddr(nullptr, #name);               \
        if(name == nullptr)                                                     \
        {                                                                       \
            std::cout << "Failed to load global level Vulkan function called: " \
            #name << std::endl;                                                 \
            return false;                                                       \
        }
        #include "ListOfVulkanFunctions.inl"
        return true;
    }

    //Checks all available instance extensions and adds them into a vector
    bool checkAvailableInstanceExtensions(std::vector<VkExtensionProperties> &availableExtensions)
    {
        uint32_t extensions = 0;
        VkResult result = VK_SUCCESS;

        //If last parameter to this function is set to nullptr, the number of extensions
        //available will be stored into the extension-object.
        result = vkEnumerateInstanceExtensionProperties(nullptr, &extensions, nullptr);
        if( (result != VK_SUCCESS) || (extensions == 0))
        {
            std::cout << "Failed to load instance extensions!" << std::endl;
            return false;
        }

        //Resize the container and enumerate again to gather the data
        availableExtensions.resize(extensions);
        result = vkEnumerateInstanceExtensionProperties(nullptr, &extensions, &availableExtensions[0]);
        if(result != VK_SUCCESS)
        {
            std::cout << "Failed to enumerate instance extensions!" << std::endl;
            return false;
        }

        return true;
    }

    //Creates a new vulkan instance
    bool createVulkanInstance(std::vector<char const*> const& desiredExtensions,
                              char const* const appName,
                              VkInstance &instance)
    {
        //First check all available extensions
        std::vector<VkExtensionProperties> availableExtensions;
        if(!checkAvailableInstanceExtensions(availableExtensions))
            return false;

        //Next compare if desired extensions are available
        for(auto& extension : desiredExtensions)
        {
            if(!isExtensionSupported(availableExtensions, extension))
            {
                std::cout << "Extension: " << extension << " not available!" << std::endl;
                return false;
            }
        }

        //Create application info
        VkApplicationInfo appInfo = VulkanStructures::applicationInfo();
        appInfo.pApplicationName = appName;
        appInfo.applicationVersion = VK_MAKE_VERSION(0,1,0);
        appInfo.pEngineName = appName;
        appInfo.engineVersion = VK_MAKE_VERSION(0,1,0);
        appInfo.apiVersion = VK_MAKE_VERSION(1,0,0);

        //Create instance info
        VkInstanceCreateInfo instanceInfo = VulkanStructures::instanceCreateInfo();
        instanceInfo.pApplicationInfo = &appInfo;
        instanceInfo.enabledLayerCount = 0;
        instanceInfo.ppEnabledLayerNames = nullptr;
        instanceInfo.enabledExtensionCount = static_cast<uint32_t>(desiredExtensions.size());
        instanceInfo.ppEnabledExtensionNames = desiredExtensions.size() > 0 ? &desiredExtensions[0] : 0;

        //Create the vulkan instance
        VkResult result = vkCreateInstance(&instanceInfo, nullptr, &instance);
        if(result !=  VK_SUCCESS || instance == VK_NULL_HANDLE)
        {
            std::cout  << "Failed to create an instance!" << std::endl;
            return false;
        }

        //Return true if the operation was successful.
        return true;
    }

    //Checks wether a desired extension is supported or not
    bool isExtensionSupported(std::vector<VkExtensionProperties> &availableExtensions, const char* desiredExtension)
    {
        //Loop through the whole extension vector and check, if the desired extension is supported
        for(auto& extension : availableExtensions)
        {
            //Compare the two extension names
            if(std::strcmp(extension.extensionName,desiredExtension) == 0)
                return true;
        }
        //Otherwise the extension is not supported
        return false;
    }
}
