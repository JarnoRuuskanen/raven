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
        instanceInfo.ppEnabledExtensionNames = desiredExtensions.size() > 0 ? desiredExtensions.data() : 0;

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

    //Enumerates through all physical, vulkan-capable devices and stores them inside a vector
    bool loadPhysicalDevices(VkInstance &instance, std::vector<VkPhysicalDevice> &physicalDevices)
    {
        uint32_t deviceCount = 0;
        VkResult result;
        //First enumerate physical devices to find out how many devices there are
        result = vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

        //Check that enumeration was successful.
        if(result != VK_SUCCESS || deviceCount == 0)
        {
            std::cout << "Failed to enumerate physical device count!" << std::endl;
            return false;
        }

        //deviceCount has now been updated, resize physicalDevies accordingly
        physicalDevices.resize(deviceCount);

        //Next get the physical devices by calling the same function again
        result = vkEnumeratePhysicalDevices(instance, &deviceCount, physicalDevices.data());

        //Check that enumeration was successful.
        if(result != VK_SUCCESS || physicalDevices.empty())
        {
            std::cout << "Failed to get physical devices!" << std::endl;
            return false;
        }

        return true;
    }

    //Checks wether a physical device extension is supported or not
    bool arePhysicalDeviceExtensionsSupported(VkPhysicalDevice &physicalDevice,
                                              std::vector<char const*> const& desiredExtensions)
    {
        uint32_t extensionCount = 0;
        std::vector<VkExtensionProperties> availableExtensions;
        VkResult result;

        //First find out how many extensions are supported by the physical device
        result = vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);
        if(result != VK_SUCCESS || extensionCount == 0)
            return false;

        //Next resize the container and enumerate again to get the extensions
        availableExtensions.resize(extensionCount);
        result = vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, availableExtensions.data());

        if(result != VK_SUCCESS || availableExtensions.empty())
            return false;

        //Now that we have the array of extensions check if the desiredProperty is supported
        for(auto& extension : desiredExtensions)
        {
            if(!isExtensionSupported(availableExtensions, extension))
            {
                std::cout << "Extension: " << extension << " not available!" << std::endl;
                return false;
            }
        }
        return true;
    }

    //Checks physical device features.
    void getPhysicalDeviceFeaturesAndProperties(VkPhysicalDevice &physicalDevice,
                                                VkPhysicalDeviceFeatures &features,
                                                VkPhysicalDeviceProperties &properties)
    {
        //Get the features
        vkGetPhysicalDeviceFeatures(physicalDevice, &features);
        //Get the properties
        vkGetPhysicalDeviceProperties(physicalDevice, &properties);
    }

    //Gets the index of a desired queue family/families
    bool getQueueFamilyIndex(std::vector<VkQueueFamilyProperties> &queueFamilies,
                             VkQueueFlags desiredQueueFamily,
                             uint32_t &queueFamilyIndex)
    {
        //After acquiring all the queue families, find the correct one for our needs
        for(uint32_t i = 0; i < static_cast<uint32_t>(queueFamilies.size()); i++)
        {
            if((queueFamilies[i].queueCount > 0) && (queueFamilies[i].queueFlags & desiredQueueFamily))
            {
                queueFamilyIndex = i;
                return true;
            }
        }
        std::cout << "Failed to find correct queue family/families!" << std::endl;
        return false;
    }

    //Gets all physical device queues and their properties
    bool getPhysicalDeviceQueuesWithProperties(VkPhysicalDevice &physicalDevice,
                                               std::vector<VkQueueFamilyProperties> &queueFamilies)
    {
        uint32_t queueFamilyCount = 0;
        //First get the count of queue families
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

        //Check wether there are any queue families or not
        if(queueFamilyCount == 0)
        {
            std::cout << "Failed to acquire the number of queue families!" << std::endl;
            return false;
        }

        //Get the actual queue families
        queueFamilies.resize(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());
        if(queueFamilies.empty())
        {
            std::cout << "Failed to acquire queue family properties!" << std::endl;
            return false;
        }

        //There are currently four kinds of graphics families:
        //graphics, compute, transfer and sprase.
        return true;
    }

    //Creates a logical vulkan device
    bool createLogicalDevice(VkPhysicalDevice &physicalDevice,
                             VkDeviceCreateInfo createInfo,
                             VkDevice &logicalDevice)
    {
        VkResult result;
        result = vkCreateDevice(physicalDevice, &createInfo, nullptr, &logicalDevice);
        if((result != VK_SUCCESS) && (logicalDevice != nullptr))
        {
            std::cout << "Failed to create a logical device!" << std::endl;
            return false;
        }
        return true;
    }
}
