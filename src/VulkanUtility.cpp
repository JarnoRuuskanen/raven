#include "VulkanUtility.h"
#include "VulkanStructures.h"

//All vulkan utility functions will be implemented here.
namespace Raven
{
    /**
     * @brief Checks all available instance extensions and adds them into a vector.
     * @param availableExtensions
     * @return False if something went wrong.
     */
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

    /**
     * @brief Creates a new vulkan instance.
     * @param desiredExtensions
     * @param appName
     * @param instance
     * @return False if something went wrong.
     */
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

    /**
     * @brief Checks wether a desired extension is supported or not.
     * @param availableExtensions
     * @param desiredExtension
     * @return 
     */
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

    /**
     * @brief Checks wether a physical device extension is supported or not.
     * @param physicalDevice
     * @param desiredExtensions
     * @return False if something went wrong.
     */
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

    /**
     * @brief Gets physical device features and properties.
     * @param physicalDevice
     * @param features
     * @param properties
     */
    void getPhysicalDeviceFeaturesAndProperties(VkPhysicalDevice &physicalDevice,
                                                VkPhysicalDeviceFeatures &features,
                                                VkPhysicalDeviceProperties &properties)
    {
        //Get the features
        vkGetPhysicalDeviceFeatures(physicalDevice, &features);
        //Get the properties
        vkGetPhysicalDeviceProperties(physicalDevice, &properties);
    }

    /**
     * @brief Gets the index of a desired queue family/families.
     * @param queueFamilies
     * @param desiredQueueFamily
     * @param queueFamilyIndex
     * @return False if something went wrong.
     */
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

    /**
     * @brief Gets all physical device queues and their properties
     * @param physicalDevice
     * @param queueFamilies
     * @return False if something went wrong.
     */
    bool getPhysicalDeviceQueuesWithProperties(VkPhysicalDevice &physicalDevice,
                                               std::vector<VkQueueFamilyProperties> &queueFamilies)
    {
        if(physicalDevice == VK_NULL_HANDLE)
        {
            std::cerr << "No valid physical device for queue family information fetching!" << std::endl;
            return false;
        }

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

    /**
     * @brief Creates a logical vulkan device
     * @param physicalDevice
     * @param createInfo
     * @param logicalDevice
     * @return False if something went wrong.
     */
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

    /**
     * @brief Checks if the preferred presentation mode is supported by the physical device.
     * @param physicalDevice
     * @param presentationSurface
     * @param desiredPresentMode
     * @return False if something went wrong.
     */
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

    /**
     * @brief Gets physical device presentation surface capabilities.
     * @param physicalDevice
     * @param presentationSurface
     * @param surfaceCapabilities
     * @return False if something went wrong.
     */
    bool getSurfaceCapabilities(const VkPhysicalDevice &physicalDevice,
                                const VkSurfaceKHR presentationSurface,
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

    /**
     * @brief Selects swapchain image usage flags based on desired and supported usages.
     * @param surfaceCapabilities
     * @param desiredUsages
     * @param imageUsage
     * @return False if something went wrong.
     */
    bool selectSwapchainImageUsage(const VkSurfaceCapabilitiesKHR &surfaceCapabilities,
                                   const VkImageUsageFlags desiredUsages,
                                   VkImageUsageFlags &imageUsage)
    {
        //Determine image usage. imageUsage gets filled with the data from
        //supportedUsageFlags that matches desiredUsages-flags.
        imageUsage = desiredUsages & surfaceCapabilities.supportedUsageFlags;
        //Check wether desired usage cases were supported or not.
        return desiredUsages == imageUsage;
    }

    /**
     * @brief Selects swapchain surface transformation based on desired and supported transforms.
     * @param surfaceCapabilities
     * @param desiredTransforms
     * @param surfaceTransforms
     */
    void selectSwapchainSurfaceTransform(const VkSurfaceCapabilitiesKHR &surfaceCapabilities,
                                         const VkSurfaceTransformFlagBitsKHR desiredTransforms,
                                         VkSurfaceTransformFlagBitsKHR &surfaceTransforms)
    {
        if(surfaceCapabilities.supportedTransforms & desiredTransforms)
        {
            surfaceTransforms = desiredTransforms;
        }
        else
        {
            surfaceTransforms = surfaceCapabilities.currentTransform;
        }
    }

    /**
     * @brief Selects the format for swapchain images based on desired and supported formats.
     * @param physicalDevice
     * @param presentationSurface
     * @param desiredSurfaceFormat
     * @param imageFormat
     * @param imageColorSpace
     * @return False if something went wrong.
     */
    bool selectSwapchainImageFormat(const VkPhysicalDevice &physicalDevice,
                                    const VkSurfaceKHR &presentationSurface,
                                    const VkSurfaceFormatKHR desiredSurfaceFormat,
                                    VkFormat &imageFormat,
                                    VkColorSpaceKHR &imageColorSpace)
    {
        //First enumerate all supported formats.
        uint32_t formatCount = 0;
        VkResult result;

        result = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, presentationSurface, &formatCount, nullptr);
        if(result != VK_SUCCESS || formatCount == 0)
        {
            std::cerr << "Failed to get the number of supported surface formats!" << std::endl;
            return false;
        }

        //Next get the supported surface formats.
        std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
        result = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, presentationSurface, &formatCount, surfaceFormats.data());
        if(result != VK_SUCCESS || surfaceFormats.empty())
        {
            std::cerr << "Failed to get supported surface formats!" << std::endl;
            return false;
        }

        //Now that we have all the supported surface formats, select the correct format.
        for(auto& surfaceFormat : surfaceFormats)
        {
            if(desiredSurfaceFormat.format == surfaceFormat.format &&
                    desiredSurfaceFormat.colorSpace == surfaceFormat.colorSpace)
            {
                imageFormat = surfaceFormat.format;
                imageColorSpace = surfaceFormat.colorSpace;
                return true;
            }
        }

        //If there was no surface format that supports both the image format and
        //color space that we want, choose the first color space from the supported
        //surface format.
        for(auto& surfaceFormat : surfaceFormats)
        {
            if(desiredSurfaceFormat.format == surfaceFormat.format)
            {
                imageFormat = surfaceFormat.format;
                imageColorSpace = surfaceFormat.colorSpace;
                std::cout << "Could not find a supported color space, "
                             "using first supported one from the desired "
                             "(and supported) surface format." << std::endl;
                return true;
            }
        }

        //If no surface was found that supports both the desired colorspace and format then
        //we use the default supported format and colorspace.
        imageFormat = surfaceFormats[0].format;
        imageColorSpace = surfaceFormats[0].colorSpace;
        std::cout << "Failed to find a supported surface "
                     "format/color space. Using default values." << std::endl;
        return true;

    }

    /**
     * @brief Creates a vulkan swapchain.
     * @param logicalDevice
     * @param createInfo
     * @param swapchain
     * @return False if something went wrong.
     */
    bool createSwapchain(const VkDevice logicalDevice, const VkSwapchainCreateInfoKHR createInfo, VkSwapchainKHR &swapchain)
    {
        if(logicalDevice == VK_NULL_HANDLE)
            return false;

        VkResult result;
        result = vkCreateSwapchainKHR(logicalDevice, &createInfo, nullptr, &swapchain);
        if(result != VK_SUCCESS || swapchain == VK_NULL_HANDLE)
        {
            std::cerr << "Failed to create a swapchain!" << std::endl;
            return false;
        }
        return true;
    }

    /**
     * @brief Gathers the swapchain images from a given swapchain to a vector.
     * @param logicalDevice
     * @param swapchain
     * @param swapchainImages
     * @return False if something went wrong.
     */
    bool getSwapchainImages(const VkDevice logicalDevice, const VkSwapchainKHR swapchain, std::vector<VkImage> &swapchainImages)
    {
        uint32_t imageCount = 0;
        VkResult result = VK_SUCCESS;

        result = vkGetSwapchainImagesKHR(logicalDevice, swapchain, &imageCount, nullptr);
        if(result != VK_SUCCESS || imageCount == 0)
        {
            std::cerr << "Failed to get the number of swapchain images!" << std::endl;
            return false;
        }

        swapchainImages.resize(imageCount);
        result = vkGetSwapchainImagesKHR(logicalDevice, swapchain, &imageCount, swapchainImages.data());
        if(result != VK_SUCCESS || swapchainImages.empty())
        {
            std::cerr << "Failed to get swapchain images!" << std::endl;
            return false;
        }
        return true;
    }

    /**
     * @brief Gets a swapchain image for drawing from a given swapchain.
     * @param logicalDevice
     * @param swapchain The swapchain where images are being queried from.
     * @param semaphore A synchronization object.
     * @param imageIndex The index of the image in the swapchain that was returned.
     * @return False if something went wrong.
     */
    bool getSwapchainImageForDrawing(const VkDevice logicalDevice, const VkSwapchainKHR swapchain, VkSemaphore semaphore, uint32_t &imageIndex)
    {
        VkResult result = vkAcquireNextImageKHR(logicalDevice, swapchain, 3000000000, semaphore, nullptr, &imageIndex);
        if(result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR)
            return true;
        else
            return false;
    }

    /**
     * @brief Presents an image to the screen.
     * @param queue
     * @param swapchain
     * @param presentationSemaphore
     * @return False if something went wrong.
     */
    bool presentImage(const VkQueue queue, const VkSwapchainKHR swapchain, const VkPresentInfoKHR presentInfo, VkSemaphore &presentationSemaphore)
    {

    }

    /**
     * @brief Creates a new swapchain synchronization object.
     * @param logicalDevice
     * @param semaphore
     * @return False if someething went wrong.
     */
    bool createSemaphore(const VkDevice logicalDevice, VkSemaphore &semaphore)
    {
        VkSemaphoreCreateInfo createInfo = VulkanStructures::semaphoreCreateInfo();
        if(vkCreateSemaphore(logicalDevice, &createInfo, nullptr, &semaphore) != VK_SUCCESS)
        {
            std::cerr << "Failed to create a semaphore!" << std::endl;
            return false;
        }
        return true;
    }

    /**
     * @brief Destroys a presentation surface.
     * @param instance
     * @param surface
     */
    void destroyPresentationSurface(const VkInstance instance, VkSurfaceKHR &surface)
    {
        if(surface)
        {
            vkDestroySurfaceKHR(instance, surface, nullptr);
        }
        surface = VK_NULL_HANDLE;
    }

    /**
     * @brief Destroys a swapchain.
     * @param logicalDevice
     * @param swapchain
     */
    void destroySwapchain(const VkDevice logicalDevice, VkSwapchainKHR &swapchain)
    {
        if(swapchain)
        {
            vkDestroySwapchainKHR(logicalDevice, swapchain, nullptr);
            swapchain = VK_NULL_HANDLE;
        }
    }
}
