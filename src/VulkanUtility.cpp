#include "VulkanUtility.h"
#include "VulkanStructures.h"
#include "CommandBufferManager.h"

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
        instanceInfo.ppEnabledExtensionNames =
                desiredExtensions.size() > 0 ? desiredExtensions.data() : 0;

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
    bool isExtensionSupported(std::vector<VkExtensionProperties> &availableExtensions,
                              const char* desiredExtension)
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
        result = vkEnumerateDeviceExtensionProperties(physicalDevice,
                                                      nullptr,
                                                      &extensionCount,
                                                      availableExtensions.data());

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
                                                VkPhysicalDeviceProperties &properties) noexcept
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
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice,
                                                 &queueFamilyCount,
                                                 queueFamilies.data());
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
     * @brief Gets queues of a logical device's queue family.
     * @param logicalDevice
     * @param queueFamilyIndex
     * @param queueCount
     * @param queues
     * @return
     */
    void getQueueFamilyQueues(const VkDevice logicalDevice,
                              const uint32_t queueFamilyIndex,
                              const uint32_t queueCount,
                              std::vector<VkQueue> &queues) noexcept
    {
        for(uint32_t i = 0; i < queueCount; ++i)
        {
            VkQueue newQueue;
            vkGetDeviceQueue(logicalDevice, queueFamilyIndex, i, &newQueue);
            queues.push_back(newQueue);
        }
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
        result = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice,
                                                           presentationSurface,
                                                           &presentationModesCount, nullptr);

        if((result != VK_SUCCESS) || (presentationModesCount == 0))
        {
            std::cerr << "Failed to acquire the amount of presentation modes!" << std::endl;
            return false;
        }

        //Get all the presentation modes supported by the physical device.
        std::vector<VkPresentModeKHR> presentationModes(presentationModesCount);
        result = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice,
                                                           presentationSurface,
                                                           &presentationModesCount,
                                                           presentationModes.data());

        if((result != VK_SUCCESS) || (presentationModes.empty()))
        {
            std::cerr << "Failed to acquire the presentation modes!" << std::endl;
            return false;
        }

        //Enumerate through all presentation modes and find wether the desired presentation
        //mode is supported or not.
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
        result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice,
                                                           presentationSurface,
                                                           &surfaceCapabilities);
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
     *        This function was mostly copied from VulkanCookbook.
     * @param physicalDevice
     * @param presentationSurface
     * @param desiredSurfaceFormat
     * @param imageFormat
     * @param imageColorSpace
     * @return False if something went wrong.
     */
    bool selectSwapchainImageFormat(VkPhysicalDevice &physicalDevice,
                                    VkSurfaceKHR &presentationSurface,
                                    VkSurfaceFormatKHR desiredSurfaceFormat,
                                    VkFormat &imageFormat,
                                    VkColorSpaceKHR &imageColorSpace)
    {
        //First enumerate all supported formats.
        uint32_t formatsCount = 0;
        VkResult result = VK_SUCCESS;

        result = vkGetPhysicalDeviceSurfaceFormatsKHR( physicalDevice, presentationSurface,
                                                       &formatsCount, nullptr );
        if((VK_SUCCESS != result) ||(0 == formatsCount))
        {
          std::cout << "Failed to get the number of supported surface formats!" << std::endl;
          return false;
        }

        std::vector<VkSurfaceFormatKHR> surfaceFormats( formatsCount );
        result = vkGetPhysicalDeviceSurfaceFormatsKHR( physicalDevice, presentationSurface,
                                                       &formatsCount, surfaceFormats.data() );
        if((VK_SUCCESS != result) || (0 == formatsCount))
        {
              std::cout << "Could not enumerate supported surface formats." << std::endl;
              return false;
        }

        // Select surface format
        if((1 == surfaceFormats.size()) && (VK_FORMAT_UNDEFINED == surfaceFormats[0].format) )
        {
              imageFormat = desiredSurfaceFormat.format;
              imageColorSpace = desiredSurfaceFormat.colorSpace;
              return true;
        }

        for( auto & surfaceFormat : surfaceFormats)
        {
            if( (desiredSurfaceFormat.format == surfaceFormat.format) &&
                (desiredSurfaceFormat.colorSpace == surfaceFormat.colorSpace) )
            {
                imageFormat = desiredSurfaceFormat.format;
                imageColorSpace = desiredSurfaceFormat.colorSpace;
                return true;
            }
        }

        for(auto & surfaceFormat : surfaceFormats)
        {
            if((desiredSurfaceFormat.format == surfaceFormat.format))
            {
                imageFormat = desiredSurfaceFormat.format;
                imageColorSpace = surfaceFormat.colorSpace;
                std::cout << "Desired combination of format and colorspace is not supported. Selecting other colorspace." << std::endl;
                return true;
            }
        }

        imageFormat = surfaceFormats[0].format;
        imageColorSpace = surfaceFormats[0].colorSpace;
        std::cout << "Desired format is not supported. Selecting available format - colorspace combination." << std::endl;
        return true;
    }

    /**
     * @brief Creates a vulkan swapchain.
     * @param logicalDevice
     * @param createInfo
     * @param swapchain
     * @return False if something went wrong.
     */
    bool createSwapchain(const VkDevice logicalDevice,
                         const VkSwapchainCreateInfoKHR createInfo,
                         VkSwapchainKHR &swapchain)
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
    bool getSwapchainImages(const VkDevice logicalDevice,
                            const VkSwapchainKHR swapchain,
                            std::vector<VkImage> &swapchainImages)
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
     * @param fence A synchronization object.
     * @param imageIndex The index of the image in the swapchain that was returned.
     * @return False if something went wrong.
     */
    bool acquireSwapchainImage(const VkDevice logicalDevice,
                               const VkSwapchainKHR swapchain,
                               VkSemaphore semaphore,
                               VkFence fence,
                               uint32_t &imageIndex)
    {
        VkResult result = vkAcquireNextImageKHR(logicalDevice, swapchain, 2000000000,
                                                semaphore, fence, &imageIndex);
        switch(result)
        {
            case VK_SUCCESS:
                return true;
            case VK_SUBOPTIMAL_KHR:
                return true;
            default:
                return false;
        }
    }

    /**
     * @brief Presents an image to the screen.
     * @param queue
     * @return False if something went wrong.
     */
    bool presentImage(const VkQueue queue,
                      const VkPresentInfoKHR presentInfo)
    {
        VkResult result = vkQueuePresentKHR(queue, &presentInfo);
        //Switch case is faster than if else so:
        switch(result)
        {
            case VK_SUCCESS:
                return true;
            default:
                return false;
        }
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
     * @brief Destroys a semaphore. The semaphore must not be referenced anymore by any queues.
     * @param logicalDevice
     * @param semaphore
     */
    void destroySemaphore(const VkDevice logicalDevice, VkSemaphore &semaphore) noexcept
    {
        if(semaphore != VK_NULL_HANDLE)
        {
            vkDestroySemaphore(logicalDevice, semaphore, nullptr);
            semaphore = VK_NULL_HANDLE;
        }
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
            surface = VK_NULL_HANDLE;
        }
    }

    /**
     * @brief Destroys a swapchain.
     * @param logicalDevice
     * @param swapchain
     */
    void destroySwapchain(const VkDevice logicalDevice, VkSwapchainKHR &swapchain) noexcept
    {
        if(swapchain)
        {
            vkDestroySwapchainKHR(logicalDevice, swapchain, nullptr);
            swapchain = VK_NULL_HANDLE;
        }
    }

    /**
     * @brief Creates a fence. Note that a fence can be created
     *        already in signaled state so this needs to be specified.
     * @param logicalDevice
     * @param fence
     * @return False if something went wrong.
     */
    bool createFence(const VkDevice logicalDevice, VkBool32 initializeSignaled, VkFence &fence)
    {
        VkFenceCreateInfo createInfo = VulkanStructures::fenceCreateInfo(initializeSignaled);
        VkResult result = vkCreateFence(logicalDevice, &createInfo, nullptr, &fence);
        if(result != VK_SUCCESS)
        {
            std::cerr << "Failed to create a fence!" << std::endl;
            return false;
        }
        return true;
    }

    /**
     * @brief Destroys a fence.
     * @param logicalDevice
     * @param fence
     */
    void destroyFence(const VkDevice logicalDevice, VkFence &fence) noexcept
    {
        if(fence != VK_NULL_HANDLE)
        {
            vkDestroyFence(logicalDevice, fence, nullptr);
            fence = VK_NULL_HANDLE;
        }
    }

    /**
     * @brief Resets fences.
     * @param logicalDevice
     * @param fences
     * @return False if there were no fences in the vector or if something went
     *         wrong with the resetting.
     */
    bool resetFences(const VkDevice logicalDevice, std::vector<VkFence> &fences)
    {
        if(fences.size() > 0)
        {
            VkResult result = vkResetFences(logicalDevice, static_cast<uint32_t>(fences.size()),
                                            fences.data());
            if(result != VK_SUCCESS)
            {
                std::cerr << "Failed to reset fences!" << std::endl;
                return false;
            }
            return true;
        }
        return false;
    }

    /**
     * @brief Makes the application wait until all the fences are signaled or until timeout
     *        has been reached.
     * @param logicalDevice
     * @param timeout
     * @param waitForAll
     * @param fences
     * @return False if something went wrong.
     */
    bool waitForFences(const VkDevice logicalDevice,
                       const uint32_t timeout,
                       const VkBool32 waitForAll,
                       std::vector<VkFence>const &fences)
    {
        if(fences.size() > 0)
        {
            VkResult result = vkWaitForFences(logicalDevice, static_cast<uint32_t>(fences.size()),
                                              fences.data(), waitForAll, timeout);
            if(result != VK_SUCCESS)
            {
                std::cerr << "Failed to wait for fences!" << std::endl;
                return false;
            }
            return true;
        }
        return false;
    }

    /**
     * @brief Checks if a fence has been signaled or not.
     * @param logicalDevice
     * @param fence
     * @return
     */
    bool isFenceSignaled(const VkDevice logicalDevice, VkFence &fence)
    {
        if(vkGetFenceStatus(logicalDevice, fence) != VK_SUCCESS)
        {
            std::cerr << "Fence is not signaled!" << std::endl;
            return false;
        }
        return true;
    }

    /**
     * @brief Sets buffer memory barriers.
     * @param commandBuffer
     * @param generatingStages What stages of the pipeline have been using the buffers so far.
     * @param consumingStages What stages the buffers will be used afterwards.
     * @param bufferTransitions
     */
    void setBufferMemoryBarriers(VkCommandBuffer commandBuffer,
                                 const VkPipelineStageFlags generatingStages,
                                 const VkPipelineStageFlags consumingStages,
                                 std::vector<BufferTransition> bufferTransitions) noexcept
    {
        //Create a vector for the buffer memory barriers.
        std::vector<VkBufferMemoryBarrier> barriers;
        for(auto& bufferTransition : bufferTransitions)
        {
            //Set a barrier for each buffer.
            barriers.push_back(VulkanStructures::bufferMemoryBarrier(bufferTransition.currentAccess,
                                                                     bufferTransition.newAccess,
                                                                     bufferTransition.currentQueueFamily,
                                                                     bufferTransition.newQueueFamily,
                                                                     bufferTransition.buffer,
                                                                     0,
                                                                     VK_WHOLE_SIZE));
        }

        //Set the barriers.
        if(barriers.size() > 0)
        {
            vkCmdPipelineBarrier(commandBuffer, generatingStages, consumingStages, 0, 0,
                                 nullptr, static_cast<uint32_t>(barriers.size()),
                                 barriers.data(),0, nullptr);
        }
    }

    /**
     * @brief Sets image memory barriers. Memory barriers are used to change the
     *        use case of images/buffers/memory and to make command buffers wait
     *        for each other to finish working.
     *        It is always better to define as many images in
     *        as few barriers as possible to achieve better performance.
     * @param commandBuffer
     * @param generatingStages
     * @param consumingStages
     * @param imageTransitions
     */
    void setImageMemoryBarriers(VkCommandBuffer commandBuffer,
                                const VkPipelineStageFlags generatingStages,
                                const VkPipelineStageFlags consumingStages,
                                std::vector<ImageTransition> imageTransitions) noexcept
    {
        //Create a vector for the image memory barriers.
        std::vector<VkImageMemoryBarrier> barriers;
        for(auto& imageTransition : imageTransitions)
        {
            barriers.push_back(VulkanStructures::imageMemoryBarrier(imageTransition.image,
                                                                    imageTransition.currentAccess,
                                                                    imageTransition.newAccess,
                                                                    imageTransition.currentQueueFamily,
                                                                    imageTransition.newQueueFamily,
                                                                    imageTransition.currentLayout,
                                                                    imageTransition.newLayout,
                                                                    {
                                                                        imageTransition.aspect,
                                                                        0,
                                                                        VK_REMAINING_MIP_LEVELS,
                                                                        0,
                                                                        VK_REMAINING_ARRAY_LAYERS
                                                                    }));
        }

        //Set the barriers.
        if(barriers.size() > 0)
        {
            vkCmdPipelineBarrier(commandBuffer, generatingStages, consumingStages, 0, 0,
                                 nullptr, 0, nullptr, static_cast<uint32_t>(barriers.size()),
                                 barriers.data());
        }
    }

    /**
     * @brief Makes the application to wait until the selected device is idle.
     * @param logicalDevice
     * @return Returns false if the waiting fails for some reason.
     */
    bool waitUntilDeviceIdle(VkDevice &logicalDevice)
    {
        VkResult result = vkDeviceWaitIdle(logicalDevice);
        if(result != VK_SUCCESS)
        {
            std::cerr << "Failed to wait on logical device!" << std::endl;
            return false;
        }
        return true;
    }

    /**
     * @brief Makes the application wait until the selected queue has completed all tasks given to it.
     * @param queue
     * @return Returns false if waiting on the queue fails.
     */
    bool waitUntilQueueIdle(VkQueue &queue)
    {
        VkResult result = vkQueueWaitIdle(queue);
        if(result != VK_SUCCESS)
        {
            std::cerr << "Failed to wait until queue was idle!" << std::endl;
            return false;
        }
        return true;
    }

    /**
     * @brief Creates a buffer.
     * @param logicalDevice
     * @param createInfo
     * @param buffer
     * @return False if the buffer creation fails.
     */
    bool createBuffer(const VkDevice logicalDevice,
                      const VkBufferCreateInfo createInfo,
                      VkBuffer &buffer)
    {
        VkResult result = vkCreateBuffer(logicalDevice, &createInfo, nullptr, &buffer);
        if(result != VK_SUCCESS)
        {
            std::cerr << "Failed to create a buffer!" << std::endl;
            return false;
        }
        return true;
    }

    /**
     * @brief destroyBuffer
     * @param logicalDevice
     * @param buffer
     */
    void destroyBuffer(const VkDevice logicalDevice, VkBuffer &buffer) noexcept
    {
         if(buffer != VK_NULL_HANDLE)
         {
             vkDestroyBuffer(logicalDevice, buffer, nullptr);
             buffer = VK_NULL_HANDLE;
         }
    }

    /**
     * @brief Creates and prepares a staging buffer which can be used for
              updating device-local memory in particular.
     * @param logicalDevice
     * @param memoryProperties
     * @param allocationSize
     * @param stagingBuffer
     * @param stagingMemory
     * @return False if the staging buffer creation or memory allocation fails.
     */
    bool prepareStagingBuffer(const VkDevice logicalDevice,
                              VkPhysicalDeviceMemoryProperties memoryProperties,
                              VkDeviceSize allocationSize,
                              VulkanBuffer &stagingBufferObject,
                              VkDeviceMemory &stagingMemory)
    {
        VkBufferCreateInfo stagingInfo =
                VulkanStructures::bufferCreateInfo(allocationSize,
                                                   VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                                   VK_SHARING_MODE_EXCLUSIVE);
        //Create the staging buffer.
        if(!createBuffer(logicalDevice, stagingInfo, stagingBufferObject.buffer))
            return false;

        //Save the staging buffer size.
        stagingBufferObject.size = allocationSize;

        //Allocate memory for the staging buffer.
        VkMemoryRequirements memReq;
        vkGetBufferMemoryRequirements(logicalDevice, stagingBufferObject.buffer, &memReq);

        //Allocate the memory and bind the buffer.
        if(!allocateMemory(logicalDevice, memoryProperties, memReq,
                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, stagingMemory))
        {
            return false;
        }

        if(!stagingBufferObject.bindMemoryObject(logicalDevice, stagingMemory, 0))
            return false;

        return true;
    }

    /**
     * @brief Creates a buffer view.
     * @param logicalDevice
     * @param createInfo
     * @param bufferView
     * @return False if the buffer cretion fails.
     */
    bool createBufferView(const VkDevice logicalDevice,
                          const VkBufferViewCreateInfo createInfo,
                          VkBufferView &bufferView)
    {
        VkResult result = vkCreateBufferView(logicalDevice, &createInfo, nullptr, &bufferView);
        if(result != VK_SUCCESS)
        {
            std::cerr << "Failed to create a buffer view!" << std::endl;
            return false;
        }
        return true;
    }

    /**
     * @brief destroyBufferView
     * @param logicalDevice
     * @param bufferView
     */
    void destroyBufferView(const VkDevice logicalDevice, VkBufferView &bufferView) noexcept
    {
        if(bufferView != VK_NULL_HANDLE)
        {
            vkDestroyBufferView(logicalDevice, bufferView, nullptr);
            bufferView = VK_NULL_HANDLE;
        }
    }

    /**
     * @brief Creates an image.
     * @param logicalDevice
     * @param createInfo
     * @param image
     * @return False if the image creation fails.
     */
    bool createImage(const VkDevice logicalDevice, const VkImageCreateInfo createInfo, VkImage &image)
    {
        VkResult result = vkCreateImage(logicalDevice, &createInfo, nullptr, &image);
        if(result != VK_SUCCESS)
        {
            std::cerr << "Failed to create an image!" << std::endl;
            return false;
        }
        return true;
    }

    /**
     * @brief destroyImage
     * @param logicalDevice
     * @param image
     */
    void destroyImage(const VkDevice logicalDevice, VkImage &image) noexcept
    {
        if(image != VK_NULL_HANDLE)
        {
            vkDestroyImage(logicalDevice, image, nullptr);
            image = VK_NULL_HANDLE;
        }
    }

    /**
     * @brief Creates an image view.
     * @param logicalDevice
     * @param createInfo
     * @param imageView
     * @return
     */
    bool createImageView(const VkDevice logicalDevice,
                         const VkImageViewCreateInfo createInfo,
                         VkImageView &imageView)
    {
      VkResult result = vkCreateImageView(logicalDevice, &createInfo, nullptr, &imageView);
      if(result != VK_SUCCESS)
      {
          std::cerr << "Failed to create an image view!" << std::endl;
          return false;
      }
      return true;
    }

    /**
     * @brief destroyImageView
     * @param logicalDevice
     * @param imageView
     */
    void destroyImageView(const VkDevice logicalDevice, VkImageView &imageView) noexcept
    {
        if(imageView != VK_NULL_HANDLE)
        {
            vkDestroyImageView(logicalDevice, imageView, nullptr);
            imageView = VK_NULL_HANDLE;
        }
    }

    /**
     * @brief Creates a sampler.
     * @param logicalDevice
     * @param samplerInfo
     * @param sampler
     * @return False if sampler could not be created.
     */
    bool createSampler(const VkDevice logicalDevice, VkSamplerCreateInfo samplerInfo, VkSampler &sampler)
    {
        VkResult result = vkCreateSampler(logicalDevice, &samplerInfo, nullptr, &sampler);
        if(result != VK_SUCCESS)
        {
            std::cerr << "Failed to create a sampler!" << std::endl;
            return false;
        }
        return true;
    }

    /**
     * @brief Destroys a sampler.
     * @param logicalDevice
     * @param sampler
     */
    void destroySampler(const VkDevice logicalDevice, VkSampler &sampler)
    {
        if(sampler != VK_NULL_HANDLE)
        {
            vkDestroySampler(logicalDevice, sampler, nullptr);
            sampler = VK_NULL_HANDLE;
        }
    }

    /**
     * @brief Gets the correct memory type for image/buffer.
     * @param physicalDeviceMemoryProperties
     * @param memReq Such memory requirements as size and alignment.
     * @param memoryProperties The sort of properties the memory should have.
     * @param memoryTypeIndex The index of the correct memory, if any was found.
     * @return False if correct memory type index could not be found.
     */
    bool getMemoryType(VkPhysicalDeviceMemoryProperties memoryProperties,
                       VkMemoryRequirements memReq,
                       VkFlags requiredProperties,
                       uint32_t &memoryTypeIndex)
    {
        //Iterate through the memory properties and if the correct one is found, exit the function.
        for(uint32_t type = 0; type < memoryProperties.memoryTypeCount; ++type)
        {
            if((memReq.memoryTypeBits & (1 << type)) &&
                    ((memoryProperties.memoryTypes[type].propertyFlags & requiredProperties) == requiredProperties))
            {
                memoryTypeIndex = type;
                return true;
            }
        }
        std::cerr << "Failed to find correct memory type!" << std::endl;
        return false;
    }

    /**
     * @brief Finds the correct type of memory and allocates the memory.
     * @param logicalDevice
     * @param memReq
     * @param requiredMemoryPropertyFlags
     * @param memory
     * @return False if the memory allocation fails.
     */
    bool allocateMemory(const VkDevice logicalDevice,
                        VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties,
                        VkMemoryRequirements memReq,
                        VkFlags requiredMemoryPropertyFlags,
                        VkDeviceMemory &memory)
    {
        //Find the index of the correct memory type.
        uint32_t memoryTypeIndex;
        if(!getMemoryType(physicalDeviceMemoryProperties, memReq, requiredMemoryPropertyFlags,
                          memoryTypeIndex))
        {
            return false;
        }

        //Create the allocation info and allocate the memory.
        VkMemoryAllocateInfo allocInfo =
                VulkanStructures::memoryAllocateInfo(memReq.size,memoryTypeIndex);

        VkResult result = vkAllocateMemory(logicalDevice, &allocInfo, nullptr, &memory);
        if(result != VK_SUCCESS)
        {
            std::cerr << "Failed to allocate memory!" << std::endl;
            return false;
        }
        return true;
    }

    /**
     * @brief Frees allocated memory.
     * @param logicalDevice
     * @param memory
     */
    void freeMemory(const VkDevice logicalDevice, VkDeviceMemory &memory) noexcept
    {
        if(memory != VK_NULL_HANDLE)
        {
            vkFreeMemory(logicalDevice, memory, nullptr);
            memory = VK_NULL_HANDLE;
        }
    }

    /**
     * @brief This function takes data and pushes it into a staging resource and then
     *        gives a pointer to that data.
     *        This function is mostly a copy from
     *        "VulkanCookbook - Mapping, updating and unmapping host-visible memory".
     *        This type of memory is host-visible (CPU).
     * @param logicalDevice
     * @param deviceMemory
     * @param offset
     * @param dataSize
     * @param data
     * @param pointer
     * @param unmap
     * @return False if memory could not be mapped.
     */
    bool flushDataToHostLocalMemory(const VkDevice logicalDevice, VkDeviceMemory deviceMemory,
                                    VkDeviceSize offset, VkDeviceSize dataSize, void *data,
                                    void **pointer, bool unmap)
    {
        //Map the memory.
        void* localPointer;
        VkResult result =
            vkMapMemory(logicalDevice, deviceMemory, offset, dataSize, 0,  &localPointer);
        if(result != VK_SUCCESS)
        {
            std::cerr << "Failed to map memory object!" << std::endl;
            return false;
        }

        //Copy the raw data from "pixels" to the memory object "stagingMemory"
        //pointed to by dataPointer.
        memcpy(localPointer, data, dataSize);

        //We need to inform the driver that memory contents have been modified.
        //This operation, when done by the CPU(host), is called flushing.
        //This is done with mapped memory ranges.
        std::vector<VkMappedMemoryRange> memoryRanges =
            VulkanStructures::mappedMemoryRanges(deviceMemory, 0, VK_WHOLE_SIZE);

        //Flush the memory ranges.
        result = vkFlushMappedMemoryRanges(logicalDevice, static_cast<uint32_t>(memoryRanges.size()),
                                           memoryRanges.data());

        if(result != VK_SUCCESS)
        {
            std::cerr << "Failed to flush memory ranges!" << std::endl;
            return false;
        }

        //The data has now been uploaded. The memory can be unmapped if wanted.
        if(unmap)
        {
            vkUnmapMemory(logicalDevice, deviceMemory);
        }
        else if(pointer != nullptr)
        {
            *pointer = localPointer;
        }
        return true;
    }

    /**
     * @brief Copies data between buffers. Command buffer needs to be in recording state
     *        and the buffers need to have been created with correct usage flags and they
     *        also require the correct access flags.
     * @param cmdBuffer The command buffer used which needs to be in recording state.
     * @param sourceBuffer
     * @param dstBuffer
     * @param memoryRanges Tells the memory ranges of each source and destination buffer.
     * @return False if memory ranges were not specified.
     */
    bool copyDataBetweenBuffers(VkCommandBuffer cmdBuffer, VkBuffer sourceBuffer,
                                VkBuffer dstBuffer, std::vector<VkBufferCopy> memoryRanges)
    {
        if(memoryRanges.size() > 0)
        {
            vkCmdCopyBuffer(cmdBuffer, sourceBuffer, dstBuffer,
                            static_cast<uint32_t>(memoryRanges.size()), memoryRanges.data());
            return true;
        }
        std::cerr << "Failed to copy data between buffers due to no VkBufferCopy values!" << std::endl;
        return false;
    }

    /**
     * @brief Copies data from a buffer to an image. Used mainly when an image uses
     *        device-local memory, which needs to be updated from application.
     * @param cmdBuffer The command buffer used which needs to be in recording state.
     * @param sourceBuffer
     * @param dstImage
     * @param imageLayout
     * @param memoryRanges Memory ranges for source buffers and destination images.
     * @return False if memory ranges were not specified.
     */
    bool copyDataFromBufferToImage(VkCommandBuffer cmdBuffer, VkBuffer sourceBuffer,
                                   VkImage dstImage, VkImageLayout imageLayout,
                                   std::vector<VkBufferImageCopy> memoryRanges)

    {
        if(memoryRanges.size() > 0)
        {
            vkCmdCopyBufferToImage(cmdBuffer, sourceBuffer, dstImage, imageLayout,
                                   static_cast<uint32_t>(memoryRanges.size()), memoryRanges.data());
            return true;
        }
        std::cerr << "Failed to copy data from a buffer to an image due to no "
                     "VkBufferImageCopy values!" << std::endl;
        return false;
    }

    /**
     * @brief Copies data from an image to a buffer.
     * @param cmdBuffer
     * @param sourceImage
     * @param imageLayout
     * @param dstBuffer
     * @param memoryRangest
     * @return False if memory ranges were not specified.
     */
    bool copyDataFromImageToBuffer(VkCommandBuffer cmdBuffer, VkImage sourceImage,
                                   VkImageLayout imageLayout, VkBuffer dstBuffer,
                                   std::vector<VkBufferImageCopy> memoryRanges)
    {
        if(memoryRanges.size() > 0)
        {
            vkCmdCopyImageToBuffer(cmdBuffer, sourceImage, imageLayout, dstBuffer,
                                   static_cast<uint32_t>(memoryRanges.size()), memoryRanges.data());
            return true;
        }
        std::cerr << "Failed to copy data from an image to a buffer due to no "
                     "VkBufferImageCopy values!" << std::endl;
        return false;
    }

    /**
     * @brief Updates a buffer which uses device-local memory.
     * @param logicalDevice
     * @param data
     * @param memoryProperties
     * @param destinationBuffer
     * @param destinationOffset
     * @param destinationBufferCurrentAccess
     * @param destinationBufferNewAccess
     * @param destinationBufferGeneratingStages
     * @param destinationBufferConsumingStages
     * @param queue
     * @param cmdBuffer
     * @param signalSemaphores
     * @return
     */
    bool updateDeviceLocalMemoryBuffer(VkDevice logicalDevice,
                                       void *data,
                                       VkDeviceSize dataSize,
                                       VkPhysicalDeviceMemoryProperties memoryProperties,
                                       VkBuffer destinationBuffer,
                                       VkDeviceSize destinationOffset,
                                       VkAccessFlags destinationBufferCurrentAccess,
                                       VkAccessFlags destinationBufferNewAccess,
                                       VkPipelineStageFlags destinationBufferGeneratingStages,
                                       VkPipelineStageFlags destinationBufferConsumingStages,
                                       VkQueue queue,
                                       VkCommandBuffer cmdBuffer,
                                       std::vector<VkSemaphore> signalSemaphores)
    {
        //Create staging buffer resources.
        VulkanBuffer stagingBufferObject;
        VkDeviceMemory stagingMemory;

        //Create the staging buffer.
        if(!prepareStagingBuffer(logicalDevice, memoryProperties, dataSize,
                                 stagingBufferObject, stagingMemory))
        {
            return false;
        }

        //Map the host-visible memory.
        if(!flushDataToHostLocalMemory(logicalDevice, stagingMemory, 0, dataSize, data, nullptr, true))
            return false;

        //Now that is has been mapped, begin the command buffer recording.
        if(!CommandBufferManager::beginCommandBuffer(cmdBuffer, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT))
            return false;

        //Set buffer memory barrier so that the transfer can begin.
        BufferTransition firstTransition = { destinationBuffer, destinationBufferCurrentAccess,
                                             VK_ACCESS_TRANSFER_WRITE_BIT, VK_QUEUE_FAMILY_IGNORED,
                                             VK_QUEUE_FAMILY_IGNORED};

        setBufferMemoryBarriers(cmdBuffer, destinationBufferGeneratingStages, VK_PIPELINE_STAGE_TRANSFER_BIT,
                                {firstTransition});

        //After the destination buffer has been set to the correct usage, start the data transfer from the
        //staging buffer.
        VkBufferCopy memoryRange = {0, destinationOffset, dataSize};
        if(!copyDataBetweenBuffers(cmdBuffer, stagingBufferObject.buffer, destinationBuffer, {memoryRange}))
            return false;

        //After the transfer is complete, change the destination buffer usage again so that it can be read from.
        BufferTransition secondTransition = {destinationBuffer, VK_ACCESS_TRANSFER_WRITE_BIT,
                                             destinationBufferNewAccess, VK_QUEUE_FAMILY_IGNORED,
                                             VK_QUEUE_FAMILY_IGNORED};
        setBufferMemoryBarriers(cmdBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, destinationBufferConsumingStages,
                                {secondTransition});

        if(!CommandBufferManager::endCommandBuffer(cmdBuffer))
            return false;

        //Create a fence for the submit.
        VkFence fence;
        if(!createFence(logicalDevice, VK_FALSE, fence))
            return false;

        //Submit the task to a queue.
        VkSubmitInfo submitInfo = VulkanStructures::submitInfo({cmdBuffer}, {}, {}, signalSemaphores);
        if(!CommandBufferManager::submitCommandBuffers(queue, 1, submitInfo, fence))
            return false;

        //Wait for the fence to make sure the task was complete.
        if(!waitForFences(logicalDevice, 500000000, VK_FALSE, {fence}))
                return false;

        //Remember to clean afterwards by destroying the staging buffer and the fence.
        destroyBuffer(logicalDevice, stagingBufferObject.buffer);
        freeMemory(logicalDevice, stagingMemory);
        destroyFence(logicalDevice, fence);

        return true;
    }

    /**
     * @brief doesFormatSupportRequiredFeature
     * @param physicalDevice
     * @param format
     * @param requiredFeature
     * @return
     */
    bool doesFormatSupportRequiredOptimalTilingFeature(const VkPhysicalDevice physicalDevice,
                                                       VkFormat format,
                                                       VkFormatFeatureFlagBits requiredFeature)
    {
        VkFormatProperties formatProperties;
        //Get the properties of the selected format.
        vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &formatProperties);

        //Check that the selected format supports required feature.
        if(!(formatProperties.optimalTilingFeatures & requiredFeature))
        {
            std::cerr << "Provided format does not support "
                         "the required optimal tiling feature!" << std::endl;
            return false;
        }
        return true;
    }

    /**
     * @brief Checks wether a format supports a required buffer feature.
     * @param physicalDevice
     * @param format
     * @param requiredFeature
     * @return False if required feature was not supported.
     */
    bool doesFormatSupportRequiredBufferFeature(const VkPhysicalDevice physicalDevice,
                                                VkFormat format,
                                                VkFormatFeatureFlagBits requiredFeature)
    {
        VkFormatProperties formatProperties;
        vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &formatProperties);

        //Check that the selected format supports required feature.
        if(!(formatProperties.bufferFeatures & requiredFeature))
        {
            std::cerr << "Provided format does not support "
                         "the required buffer feature!" << std::endl;
            return false;
        }
        return true;
    }

    /**
     * @brief Creates an image with an image view.
     * @param logicalDevice
     * @param memoryProperties
     * @param usage
     * @param imageType
     * @param imageViewType
     * @param format
     * @param extent
     * @param layerCount
     * @param samples
     * @param initialLayout
     * @param sharingMode
     * @param mipLevels
     * @param cubemap
     * @param aspect
     * @param imageObject
     * @param imageMemoryObject
     * @return False if any of the operations fail.
     */
    bool createImageWithImageView(const VkDevice logicalDevice,
                                  VkPhysicalDeviceMemoryProperties memoryProperties,
                                  VkImageUsageFlags usage, VkImageType imageType,
                                  VkImageViewType imageViewType, VkFormat format, VkExtent3D extent,
                                  uint32_t layerCount, VkSampleCountFlagBits samples,
                                  VkImageLayout initialLayout, VkSharingMode sharingMode,
                                  uint32_t mipLevels, VkBool32 cubemap, VkImageAspectFlags aspect,
                                  VulkanImage &imageObject, VkDeviceMemory &imageMemoryObject)
    {
        VkImageCreateInfo imageInfo =
                VulkanStructures::imageCreateInfo(usage, imageType, format, extent, layerCount, samples,
                                                  initialLayout, sharingMode, mipLevels, cubemap);
        //First create the image.
        if(!createImage(logicalDevice, imageInfo, imageObject.image))
            return false;

        //Get image memory requirements.
        VkMemoryRequirements memReq;
        vkGetImageMemoryRequirements(logicalDevice, imageObject.image, &memReq);

        //Allocate memory for the image.
        if(!allocateMemory(logicalDevice, memoryProperties, memReq, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                           imageMemoryObject))
        {
            return false;
        }
        //Bind the memory.
        if(!imageObject.bindMemoryObject(logicalDevice, imageMemoryObject))
            return false;

        VkImageViewCreateInfo imageViewInfo =
                VulkanStructures::imageViewCreateInfo(imageObject.image, format, aspect, imageViewType);

        //Lastly create the image view.
        if(!createImageView(logicalDevice, imageViewInfo, imageObject.imageView))
            return false;

        return true;
    }

    /**
     * @brief Creates a new shader module.
     * @param logicalDevice
     * @param sourceCode
     * @param module
     * @return False if the module could not be created.
     */
    bool createShaderModule(const VkDevice logicalDevice,
                            std::vector<char> sourceCode,
                            VkShaderModule &module) noexcept
    {
        VkShaderModuleCreateInfo createInfo =
                VulkanStructures::shaderModuleCreateInfo(sourceCode);

        VkResult result = vkCreateShaderModule(logicalDevice, &createInfo, nullptr, &module);

        if(result != VK_SUCCESS)
        {
            std::cerr << "Failed to create a shader module!" << std::endl;
            return false;
        }
        return true;
    }

    /**
     * @brief Destroys a shader module. Shader modules can be destroyed after a pipeline has been
     *        created. They are not required during pipeline execution.
     * @param logicalDevice
     * @param shaderModule
     */
    void destroyShaderModule(const VkDevice logicalDevice, VkShaderModule &shaderModule) noexcept
    {
        if(shaderModule != VK_NULL_HANDLE)
        {
            vkDestroyShaderModule(logicalDevice, shaderModule, nullptr);
            shaderModule = VK_NULL_HANDLE;
        }
    }

    /**
     * @brief Creates a pipeline layout.
     * @param logicalDevice
     * @param layoutInfo
     * @param layout
     * @return False if pipeline layout creation fails.
     */
    bool createPipelineLayout(const VkDevice logicalDevice,
                              std::vector<VkDescriptorSetLayout> descriptorSetLayouts,
                              std::vector<VkPushConstantRange> pushConstantRanges,
                              VkPipelineLayout &layout) noexcept
    {
        VkPipelineLayoutCreateInfo layoutInfo =
                VulkanStructures::pipelineLayoutCreateInfo(descriptorSetLayouts,
                                                           pushConstantRanges);

        VkResult result = vkCreatePipelineLayout(logicalDevice, &layoutInfo, nullptr, &layout);

        if(result != VK_SUCCESS)
        {
            std::cerr << "Failed to create a pipeline layout!" << std::endl;
            return false;
        }
        return true;
    }

    /**
     * @brief Destroys a pipeline layout.
     * @param logicalDevice
     * @param layout
     */
    void destroyPipelineLayout(const VkDevice logicalDevice, VkPipelineLayout &layout) noexcept
    {
        if(layout != VK_NULL_HANDLE)
        {
            vkDestroyPipelineLayout(logicalDevice, layout, nullptr);
            layout = VK_NULL_HANDLE;
        }
    }

    /**
     * @brief Creates a pipeline cache.
     * @param logicalDevice
     * @param cacheData
     * @param cache
     * @return
     */
    bool createPipelineCache(const VkDevice logicalDevice,
                             std::vector<char> cacheData,
                             VkPipelineCache &cache) noexcept
    {
        VkPipelineCacheCreateInfo createInfo =
                VulkanStructures::pipelineCacheCreateInfo(cacheData);

        VkResult result = vkCreatePipelineCache(logicalDevice, &createInfo, nullptr, &cache);

        if(result != VK_SUCCESS)
        {
            std::cerr << "Failed to create a pipeline cache!" << std::endl;
            return false;
        }
        return true;
    }

    /**
     * @brief Destroys a pipeline cache. Pipeline caches are not used in any commands recorded
     *        in a command buffer, so they can be destroyed after all the required
     *        pipelines have been created.
     * @param logicalDevice
     * @param cache
     */
    void destroyPipelineCache(const VkDevice logicalDevice, VkPipelineCache &cache) noexcept
    {
        if(cache != VK_NULL_HANDLE)
        {
            vkDestroyPipelineCache(logicalDevice, cache, nullptr);
            cache = VK_NULL_HANDLE;
        }
    }

    /**
     * @brief Gets pipeline cache data. Creating pipelines from cache data greatly
     *        increases application performance.
     * @param logicalDevice
     * @param cache
     * @param cacheData
     * @return False if pipeline cache data could not be fetched.
     */
    bool getPipelineCacheData(const VkDevice logicalDevice,
                              VkPipelineCache cache,
                              std::vector<char> &cacheData) noexcept
    {
        size_t dataSize = 0;
        //First get the size of the data by leaving the last parameter as a nullptr.
        VkResult result = vkGetPipelineCacheData(logicalDevice, cache, &dataSize, nullptr);

        //Make sure that the operation was a success and that the cache had contents.
        if((result != VK_SUCCESS) || (dataSize == 0))
        {
            std::cerr << "Failed to acquire cache size!" << std::endl;
            return false;
        }

        //After success, resize the data container and query for the cache data by
        //calling the same function again.
        cacheData.resize(dataSize);

        result = vkGetPipelineCacheData(logicalDevice, cache, &dataSize, cacheData.data());

        if((result != VK_SUCCESS) || (dataSize == 0))
        {
            std::cerr << "Failed to acquire cache data!" << std::endl;
            return false;
        }
        return true;
    }

    /**
     * @brief Merges multiple pipeline caches. This could be used after, for an example, multiple
     *        pipelines (with each owning their own cache object) have been created with threads.
     *        Remember that the destination cache cannot be one of the source caches!
     * @param logicalDevice
     * @param combinedCaches
     * @param sourceCaches
     * @return False if caches could not be combined.
     */
    bool mergePipelineCaches(const VkDevice logicalDevice,
                             VkPipelineCache &combinedCaches,
                             std::vector<VkPipelineCache> sourceCaches) noexcept
    {
        VkResult result = vkMergePipelineCaches(logicalDevice,
                                                combinedCaches,
                                                static_cast<uint32_t>(sourceCaches.size()),
                                                sourceCaches.data());

        if(result != VK_SUCCESS)
        {
            std::cerr << "Failed to merge pipeline caches!" << std::endl;
            return false;
        }
        return true;
    }

    /**
     * @brief Creates graphics pipelines.
     * @param logicalDevice
     * @param cache
     * @param createInfos
     * @param graphicsPipelines
     * @return False if graphics pipelines could not be created or if there were no
     *         create infos presented.
     */
    bool createGraphicsPipelines(const VkDevice logicalDevice,
                                 VkPipelineCache cache,
                                 const std::vector<VkGraphicsPipelineCreateInfo> &createInfos,
                                 std::vector<VkPipeline> &graphicsPipelines) noexcept
    {
        //Make sure that there are create infos to be used with the pipeline creation.
        if(createInfos.size() > 0)
        {
            graphicsPipelines.resize(createInfos.size());

            VkResult result = vkCreateGraphicsPipelines(logicalDevice,
                                                        cache,
                                                        static_cast<uint32_t>(createInfos.size()),
                                                        createInfos.data(), nullptr,
                                                        graphicsPipelines.data());

            if(result != VK_SUCCESS)
            {
                std::cerr << "Failed to create graphics pipelines!" << std::endl;
                return false;
            }
            return true;
        }
        std::cerr << "Failed to create graphics pipelines due to missing create infos!" << std::endl;
        return false;
    }

    /**
     * @brief createComputePipelines
     * @param logicalDevice
     * @param cache
     * @param createInfos
     * @param computePipelines
     * @return
     */
    bool createComputePipelines(const VkDevice logicalDevice,
                                VkPipelineCache cache,
                                const std::vector<VkComputePipelineCreateInfo> &createInfos,
                                std::vector<VkPipeline> &computePipelines) noexcept
    {
        //Make sure that there are create infos to be used with the pipeline creation.
        if(createInfos.size() > 0)
        {
            computePipelines.resize(createInfos.size());

            VkResult result  = vkCreateComputePipelines(logicalDevice,
                                                        cache,
                                                        static_cast<uint32_t>(createInfos.size()),
                                                        createInfos.data(), nullptr,
                                                        computePipelines.data());

            if(result != VK_SUCCESS)
            {
                std::cerr << "Failed to create compute pipelines!" << std::endl;
                return false;
            }
            return true;
        }
        std::cerr << "Failed to create compute pipelines due to missing create infos!" << std::endl;
        return false;
    }

    /**
     * @brief Destroys both graphics and compute pipelines.
     * @param logicalDevice
     * @param pipeline
     */
    void destroyPipeline(const VkDevice logicalDevice, VkPipeline &pipeline) noexcept
    {
        if(pipeline != VK_NULL_HANDLE)
        {
            vkDestroyPipeline(logicalDevice, pipeline, nullptr);
            pipeline = VK_NULL_HANDLE;
        }
    }

    /**
     * @brief Binds vertex buffers for geometry drawing using a custom struct.
     * @param cmdBuffer
     * @param binding
     * @param bufferParams
     */
    void bindVertexBuffers(VkCommandBuffer cmdBuffer,
                           uint32_t binding,
                           const std::vector<VertexBufferParameters> &bufferParams)
    {
        if(bufferParams.size() > 0)
        {
            std::vector<VkBuffer> buffers;
            std::vector<VkDeviceSize> memoryOffsets;

            //Read all the buffers and buffer offsets into vectors and bind them to the command buffer.
            for(auto &parameter : bufferParams)
            {
                buffers.push_back(parameter.buffer);
                memoryOffsets.push_back(parameter.memoryOffset);
            }

            vkCmdBindVertexBuffers(cmdBuffer,
                                  binding,
                                  static_cast<uint32_t>(bufferParams.size()),
                                  buffers.data(),
                                  memoryOffsets.data());
        }
    }

    /**
     * @brief Sets dynamic viewports.
     * @param cmdBuffer
     * @param firstViewport
     * @param viewports
     */
    void setViewportState(VkCommandBuffer cmdBuffer, uint32_t firstViewport, const std::vector<VkViewport> &viewports)
    {
        vkCmdSetViewport(cmdBuffer, firstViewport, static_cast<uint32_t>(viewports.size()), viewports.data());
    }

    //Sets dynamic scissors.
    void setScissorState(VkCommandBuffer cmdBuffer, uint32_t firstScissor, const std::vector<VkRect2D> &scissors)
    {
        vkCmdSetScissor(cmdBuffer, firstScissor, static_cast<uint32_t>(scissors.size()), scissors.data());
    }
}
