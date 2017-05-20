#include "VulkanUtility.h"
#include "VulkanStructures.h"

//All vulkan utility functions will be implemented here.
namespace Raven
{
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
            std::cerr << "Failed to load instance extensions!" << std::endl;
            return false;
        }

        //Resize the container and enumerate again to gather the data
        availableExtensions.resize(extensions);
        result = vkEnumerateInstanceExtensionProperties(nullptr, &extensions, &availableExtensions[0]);
        if(result != VK_SUCCESS)
        {
            std::cerr << "Failed to enumerate instance extensions!" << std::endl;
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
                std::cerr << "Extension: " << extension << " not available!" << std::endl;
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
            std::cerr  << "Failed to create an instance!" << std::endl;
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
            std::cerr << "Failed to enumerate physical device count!" << std::endl;
            return false;
        }

        //deviceCount has now been updated, resize physicalDevies accordingly
        physicalDevices.resize(deviceCount);

        //Next get the physical devices by calling the same function again
        result = vkEnumeratePhysicalDevices(instance, &deviceCount, physicalDevices.data());

        //Check that enumeration was successful.
        if(result != VK_SUCCESS || physicalDevices.empty())
        {
            std::cerr << "Failed to get physical devices!" << std::endl;
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
                std::cerr << "Extension: " << extension << " not available!" << std::endl;
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
        std::cerr << "Failed to find correct queue family/families!" << std::endl;
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
            std::cerr << "Failed to acquire the number of queue families!" << std::endl;
            return false;
        }

        //Get the actual queue families
        queueFamilies.resize(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());
        if(queueFamilies.empty())
        {
            std::cerr << "Failed to acquire queue family properties!" << std::endl;
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
        if((result != VK_SUCCESS) || (logicalDevice == VK_NULL_HANDLE))
        {
            std::cerr << "Failed to create a logical device!" << std::endl;
            return false;
        }
        return true;
    }

    //Checks if the preferred presentation mode is supported by the physical device.
    bool isPresentationModeSupported(VkPhysicalDevice &physicalDevice,
                                     VkSurfaceKHR &presentationSurface,
                                     VkPresentModeKHR &desiredPresentMode)
    {
        //First find out how many presentation modes are supported by the physical device.
        uint32_t presentationModesCount;

        VkResult result;
        result = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, presentationSurface, &presentationModesCount, nullptr);

        if((result != VK_SUCCESS) || (presentationModesCount == 0))
        {
            std::cerr << "Failed to acquire the amount of presentation modes!" << std::endl;
            return false;
        }

        //Get all the presentation modes supported by the physical device.
        std::vector<VkPresentModeKHR> presentationModes(presentationModesCount);
        result = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, presentationSurface, &presentationModesCount, presentationModes.data());

        if((result != VK_SUCCESS) || (presentationModes.empty()))
        {
            std::cerr << "Failed to acquire the presentation modes!" << std::endl;
            return false;
        }

        //Enumerate through all presentation modes and find wether the desired presentation mode is supported or not.
        for(auto& mode : presentationModes)
        {
            if(mode == desiredPresentMode)
                return true;
        }

        return false;
    }

    //Gets physical device presentation surface capabilities.
    bool getSurfaceCapabilities(VkPhysicalDevice &physicalDevice,
                                VkSurfaceKHR presentationSurface,
                                VkSurfaceCapabilitiesKHR &surfaceCapabilities)
    {
        VkResult result;
        result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, presentationSurface, &surfaceCapabilities);
        if(result != VK_SUCCESS)
        {
            std::cerr << "Failed to acquire surface capabilities!" << std::endl;
            return false;
        }
        return true;
    }
}
