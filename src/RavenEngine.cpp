#include "RavenEngine.h"
#include "VulkanUtility.h"
#include "VulkanStructures.h"
#include "Settings.h"
#include "CommandBufferManager.h"
#include "VulkanBuffer.h"

using namespace Raven;

//List desired instance extensions. Instance level extensions
//will be responsible for managing a presentation surface.
std::vector<const char*> desiredInstanceExtensions =
{
    VK_KHR_SURFACE_EXTENSION_NAME,
    PLATFORM_SURFACE_EXTENSION
};

//List of desired device extensions
std::vector<const char*> desiredDeviceExtensions =
{
    //Enable swapchains to be able to display images on screen
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

RavenEngine::RavenEngine()
{
    selectedInstance = VK_NULL_HANDLE;
    selectedPhysicalDevice = VK_NULL_HANDLE;
}

RavenEngine::~RavenEngine()
{

    //Destroy the application window
    if(appWindow != nullptr)
    {
        //Destroy window swapchain.
        destroySwapchain(vulkanDevice->getLogicalDevice(), appWindow->getSwapchain());

        //Destroy the presentation surface.
        destroyPresentationSurface(selectedInstance, appWindow->getPresentationSurface());

        delete appWindow;
    }

    //Destroy the VulkanDevice
    if(vulkanDevice != nullptr)
    {
        //Wait until the device/devices are idle before proceeding to deletion.
        waitUntilDeviceIdle(vulkanDevice->getLogicalDevice());
        delete vulkanDevice;
    }

    //Destroy the vulkan instance
    if(selectedInstance != VK_NULL_HANDLE)
    {
        vkDestroyInstance(selectedInstance, nullptr);
        selectedInstance = VK_NULL_HANDLE;
    }

    //Lastly free the dynamically loaded vulkan library
    if(libraryInitialized)
        freeVulkanLibrary(vulkanLibrary);
}

/**
 * @brief Starts the raven engine.
 * @param appName Name of the application.
 * @return Returns false if something went wrong, true when the app closes.
 */
bool RavenEngine::start(const char* appName)
{
    if(!appName || strlen(appName) == 0)
    {
        std::cerr << "Application requires a name!" << std::endl;
        return false;
    }

    //First initialize vulkan if it has not been initialized yet.
    if(!initializeVulkan())
        return false;

    //After vulkan dynamic library, exported- and global-level functions have been loaded,
    //create a new vulkan instance.
    if(!createVulkanInstance(desiredInstanceExtensions, appName, selectedInstance))
        return false;

    //After instance has been created, load instance level funcions
    if(!loadInstanceLevelVulkanFunctions(selectedInstance, desiredInstanceExtensions))
        return false;

    //Next it is time to choose which Vulkan device (usually a gpu) we are going to use.
    //First check what options we have
    if(!loadPhysicalDevices(selectedInstance, physicalDevices))
        return false;

    //Select the physical device that the program will be using. Test that
    //the physical device supports desired device extensions, which will be used
    //when creating a logical device.
    if(!selectPhysicalDevice(physicalDevices, selectedPhysicalDevice, desiredDeviceExtensions))
        return false;

    VkPhysicalDeviceFeatures features;
    VkPhysicalDeviceProperties properties;
    getPhysicalDeviceFeaturesAndProperties(selectedPhysicalDevice, features, properties);

    //Create a logical device out of the selected physical device.
    //Raven will be built to support multiple queues, that will all be used simultaneously
    //with added synchronization. This is to not tax one queue unecessarily
    vulkanDevice = new VulkanDevice();
    if(!createVulkanDevice(selectedPhysicalDevice, desiredDeviceExtensions))
        return false;

    //After the vulkan device has been created we need to create a window
    //for the application. This window will display our rendering content.
    appWindow = new VulkanWindow();
    if(!openNewWindow(windowWidth, windowHeight, appWindow))
        return false;

    VkPresentModeKHR presentationMode = SETTINGS_DEFAULT_PRESENTATION_MODE;
    VkImageUsageFlags desiredImageUsages = SETTINGS_IMAGE_USAGE_FLAGS;
    VkSwapchainKHR oldSwapchain = VK_NULL_HANDLE;
    //Create a swapchain for the window to use.
    if(!buildSwapchain(desiredImageUsages,presentationMode, oldSwapchain, appWindow))
        return false;

    //Acquire the swapchain images.
    if(!getSwapchainImages(vulkanDevice->getLogicalDevice(), appWindow->getSwapchain(), appWindow->getImages()))
        return false;

    //Create vertex buffers.
    createVertexBuffers();

    //Build the command buffers.
    buildCommandBuffers();
    //Start rendering.
    render();

    return true;
}

/**
 * @brief Renders to the application window.
 * @return False if something went wrong, true when the rendering has finished.
 */
bool RavenEngine::render()
{

    //This is just a test case for submitting commands to device queues.
    //This function does nothing of value other than works as an example for now.
    VkFence submitFence;
    createFence(vulkanDevice->getLogicalDevice(), VK_FALSE, submitFence);
    VkSemaphore signaledSemaphore;
    createSemaphore(vulkanDevice->getLogicalDevice(), signaledSemaphore);

    VkSubmitInfo submitInfo = VulkanStructures::submitInfo();
    submitInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());
    submitInfo.pCommandBuffers = commandBuffers.data();
    submitInfo.pSignalSemaphores = &signaledSemaphore;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pWaitDstStageMask = nullptr;
    submitInfo.waitSemaphoreCount = 0;
    submitInfo.pWaitSemaphores = nullptr;

    //Submit commands to the vulkan device for execution:
    if(!vulkanDevice->executeCommands(submitInfo, submitFence))
        return false;

    //Wait for the fence to be signaled and delete the synchronization objects.
    //Note that in a normal case the application shouldn't stop to wait for the fence
    //to be signaled but should instead do other tasks and check the fence status
    //every now and then and when the fence is signaled, then continue with the task.
    if(!waitForFences(vulkanDevice->getLogicalDevice(), 100000000, VK_TRUE, {submitFence}))
        return false;

    //Check the fence status:
    if(!isFenceSignaled(vulkanDevice->getLogicalDevice(), submitFence))
        return false;

    destroyFence(vulkanDevice->getLogicalDevice(), submitFence);
    destroySemaphore(vulkanDevice->getLogicalDevice(), signaledSemaphore);

    //Destroying the command pool will destroy all the command buffers allocated from it.
    CommandBufferManager::destroyCommandPool(vulkanDevice->getLogicalDevice(), cmdPool);

    return true;
}

/**
 * @brief Initializes the vulkan dynamic library.
 * @return Returns false if something went wrong.
 */
bool RavenEngine::initializeVulkan()
{
    if(!loadVulkanLibrary(vulkanLibrary))
    {
        std::cerr << "Failed to load vulkan dynamic library!" << std::endl;
        return false;
    }
    //Load vulkan functions that are defined in ListOfVulkanFunctions.inl
    if(!loadFunctionExportedFromVulkanLoaderLibrary(vulkanLibrary))
    {
        std::cerr << "Failed to load functions exported from vulkan loader library!" << std::endl;
        return false;
    }
    if(!loadGlobalLevelFunctions())
    {
        std::cerr << "Failed to load global level functions!" << std::endl;
        return false;
    }
    libraryInitialized = true;
    return true;
}

/**
 * @brief Selects the first physical device that fills all given requirements.
 * @param physicalDevices A vector containing all physical devices available.
 * @param selectedDevice The selected physical device.
 * @param desiredDeviceExtensions Desired device level extensions.
 * @return False if something went wrong.
 */
bool RavenEngine::selectPhysicalDevice(std::vector<VkPhysicalDevice> &physicalDevices,
                                       VkPhysicalDevice &selectedDevice,
                                       std::vector<const char*> &desiredDeviceExtensions)
{
    //Check if desired extensions are supported by the physical devices. Ditch those
    //that don't support the required extensions.
    for(int i = 0; i < physicalDevices.size(); i++)
    {
        if(arePhysicalDeviceExtensionsSupported(physicalDevices[i], desiredDeviceExtensions))
        {
            //We will be using the first physical device that passes all requirements given to it.
            selectedDevice = physicalDevices[i];
            return true;
        }
    }
    std::cerr << "Failed to find a physical device that supports all the required device extensions!" << std::endl;
    return false;
}

/**
 * @brief Creates a VulkanDevice from given physical device.
 * @param physicalDevice The physical device used to create the logical device.
 * @param desiredExtensions Desired device extensions for the logical device.
 * @return Returns false if something went wrong.
 */
bool RavenEngine::createVulkanDevice(VkPhysicalDevice &physicalDevice,
                                     std::vector<const char*>  &desiredExtensions)
{
    if(!vulkanDevice->initializeDevice(physicalDevice, desiredExtensions))
        return false;
    return true;
}

/**
 * @brief Creates a new VulkanWindow. VulkanWindow will hold swapchain and all closely
 *        to a presentation window related objects.
 * @param windowWidth
 * @param windowHeight
 * @param window Handle to the VulkanWindow.
 * @return Returns false if something went wrong.
 */
bool RavenEngine::openNewWindow(uint16_t windowWidth,
                                uint16_t windowHeight,
                                VulkanWindow *window)
{
        //First create the window frame.
        if(!window->createWindowFrame(windowWidth, windowHeight))
            return false;

        //Next create the window surface.
        if(!window->createWindowSurface(selectedInstance))
            return false;

        return true;
}

/**
 * @brief Builds a new swapchain to be used for rendering.
 * @param desiredImageUsage Desired image usage flags.
 * @param presentationMode Presentation mode.
 * @param window Handle to the VulkanWindow.
 * @return Returns false if something went wrong.
 */
bool RavenEngine::buildSwapchain(VkImageUsageFlags desiredImageUsage,
                                  VkPresentModeKHR &presentationMode,
                                  VkSwapchainKHR &oldSwapchain,
                                  VulkanWindow *window)
{
    //Since each window can only have one swapchain at a time,
    //we need to destroy the old swapchain if one was present.)
    if(oldSwapchain != VK_NULL_HANDLE)
    {
        vkDestroySwapchainKHR(vulkanDevice->getLogicalDevice(), oldSwapchain, nullptr);
        oldSwapchain = VK_NULL_HANDLE;
    }

    //Swapchain:
    //Make sure that the VulkanDevice's current queue family supports
    //image presentation.
    VkBool32 presentationSupported = VK_FALSE;
    VkResult result = vkGetPhysicalDeviceSurfaceSupportKHR(selectedPhysicalDevice,
                                                           vulkanDevice->getPrimaryQueueFamilyIndex(),
                                                           window->getPresentationSurface(),
                                                           &presentationSupported);
    if((result != VK_SUCCESS) || (presentationSupported != VK_TRUE))
        return false;

    //Check if the desired presentation mode is supported. If not, select a default presentation mode.
    if(!isPresentationModeSupported(selectedPhysicalDevice, window->getPresentationSurface(), presentationMode))
    {
        presentationMode = SETTINGS_DEFAULT_PRESENTATION_MODE;
    }

    //Next create the required information for a swapchain creation.
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    if(!getSurfaceCapabilities(selectedPhysicalDevice,
                               window->getPresentationSurface(),
                               surfaceCapabilities))
    {
        return false;
    }

    //Choose the amount of swapchain images.
    uint32_t imageCount = surfaceCapabilities.minImageCount + 1;
    if(surfaceCapabilities.maxImageCount > 0 && imageCount > surfaceCapabilities.maxImageCount)
            imageCount = surfaceCapabilities.maxImageCount;

    //Choose the size of the swapchain images. Remember that some OS determine window size by the size
    //of the swapchain images, so it requires some extra work to determine the correct size.
    //Here, however, I use the simplest possible solution.
    VkExtent2D imageSizes = surfaceCapabilities.currentExtent;

    //Select swapchain image usage. The available usages can be found from
    //surfaceCapabilities.supportedUsageFlags.
    VkImageUsageFlags imageUsage = 0;
    if(!selectSwapchainImageUsage(surfaceCapabilities, desiredImageUsage, imageUsage))
    {
        std::cerr << "Desired image usage flags were not supported!" << std::endl;
        return false;
    }

    //Select swapchain transformation.
    VkSurfaceTransformFlagBitsKHR desiredTransform = SETTINGS_TRANSFORM_FLAGS;
    VkSurfaceTransformFlagBitsKHR surfaceTransform;
    selectSwapchainSurfaceTransform(surfaceCapabilities, desiredTransform, surfaceTransform);

    //Select swapchain image format and colorspace.
    VkSurfaceFormatKHR desiredSurfaceFormat;
    desiredSurfaceFormat.format = SETTINGS_IMAGE_FORMAT;
    desiredSurfaceFormat.colorSpace = SETTINGS_IMAGE_COLOR_SPACE;

    //Image format and color space values will be saved to these two variables.
    VkFormat imageFormat;
    VkColorSpaceKHR colorSpace;
    if(!selectSwapchainImageFormat(selectedPhysicalDevice,
                                   appWindow->getPresentationSurface(),
                                   desiredSurfaceFormat,
                                   imageFormat, colorSpace))
    {
        return false;
    }

    //Build the swapchain.
    VkSwapchainCreateInfoKHR swapchainInfo = VulkanStructures::swapchainCreateInfo();
    swapchainInfo.surface = appWindow->getPresentationSurface();
    swapchainInfo.preTransform = surfaceTransform;
    swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainInfo.clipped = VK_TRUE;
    swapchainInfo.minImageCount = imageCount;
    swapchainInfo.presentMode = presentationMode;
    swapchainInfo.imageUsage = imageUsage;
    swapchainInfo.imageExtent = imageSizes;
    swapchainInfo.imageFormat = imageFormat;
    swapchainInfo.imageColorSpace = colorSpace;
    swapchainInfo.imageArrayLayers = 1;
    swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchainInfo.queueFamilyIndexCount = 0;
    swapchainInfo.pQueueFamilyIndices = nullptr;
    swapchainInfo.oldSwapchain = oldSwapchain;

    //Create the actual swapchain for the provided window.
    if(!createSwapchain(vulkanDevice->getLogicalDevice(), swapchainInfo, window->getSwapchain()))
    {
        return false;
    }

    return true;
}

/**
 * @brief Creates the vertex buffers.
 * @return False if vertex buffers could not be created.
 */
bool RavenEngine::createVertexBuffers()
{
    /** This function describes parts of the process of creating a vertex buffer.
        However, it is not yet fully implemented due to the lack of data. */

    /*
    //Creating a buffer:
    //After that create the buffer info. Buffer size should be the size of the data that it will hold.
    VulkanBuffer vertexBuffer;

    //Load the data from a file etc. and add it into vertexBuffer.data.

    //vertexBuffer.size should be the size of the data.
    vertexBuffer.size = 8;
    VkBufferCreateInfo bufferInfo = VulkanStructures::bufferCreateInfo(vertexBuffer.size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,VK_SHARING_MODE_EXCLUSIVE);

    //Create the actual buffer.
    createBuffer(vulkanDevice->getLogicalDevice(), bufferInfo, vertexBuffer.buffer);

    //Buffers and images don't have a memory backing so we need to allocate the memory for them.
    //Get buffer memory requirements and allocate the memory.
    VkMemoryRequirements memReq;
    vkGetBufferMemoryRequirements(vulkanDevice->getLogicalDevice(), vertexBuffer.buffer, &memReq);

    //Get the correct physical device memory type index for the buffer:
    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(selectedPhysicalDevice, &memoryProperties);

    uint32_t memoryTypeIndex;
    if(!getMemoryType(memoryProperties, memReq, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                  VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, memoryTypeIndex))
        return false;

    //Create the allocation info:
    VkMemoryAllocateInfo allocationInfo = VulkanStructures::memoryAllocateInfo(memReq.size, memoryTypeIndex);
    //Allocate the memory:
    VkDeviceMemory vertexMemory = VK_NULL_HANDLE;
    if(!allocateMemory(vulkanDevice->getLogicalDevice(), allocationInfo, vertexMemory))
        return false;

    if(!vertexBuffer.bindBufferMemory(vulkanDevice->getLogicalDevice(), vertexMemory))
        return false;
    */
    return true;
}

/**
 * @brief Allocates the application command pools, buffers and records the actions.
 * @return False if something went wrong.
 */
bool RavenEngine::buildCommandBuffers()
{
    /**
     * Create a new command buffer for each framebuffer object. This could
     * possibly be optimized by creating multiple threads and command pools.
     * For future examples I am going to implement just one command pool
     * and three buffers for now to document how it is done. I will get back
     * to this function once I have implemented the framebuffers.
    */

    //Create the command pool info.
    VkCommandPoolCreateInfo poolInfo = VulkanStructures::commandPoolCreateInfo(vulkanDevice->getPrimaryQueueFamilyIndex());
    //Create a command pool.
    if(!CommandBufferManager::createCommandPool(vulkanDevice->getLogicalDevice(), poolInfo, cmdPool))
    {
        std::cerr << "Failed to create a command pool!" << std::endl;
        return false;
    }

    VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    uint32_t bufferCount = 3;
    VkCommandBufferAllocateInfo allocInfo = VulkanStructures::commandBufferAllocateInfo(level, cmdPool, bufferCount);
    //Allocate command buffers from the pool into a vector.
    commandBuffers.clear();
    commandBuffers.resize(bufferCount);
    if(!CommandBufferManager::allocateCommandBuffer(vulkanDevice->getLogicalDevice(), allocInfo, commandBuffers))
    {
        std::cerr << "Failed to allocate command buffers!" << std::endl;
        return false;
    }

    VkCommandBufferUsageFlags flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    VkCommandBufferBeginInfo beginInfo = VulkanStructures::commandBufferBeginInfo(flags);
    //Record actions to each individual buffer:
    for(int i = 0; i < commandBuffers.size(); i++)
    {
        if(!CommandBufferManager::beginCommandBuffer(commandBuffers[i], beginInfo))
        {
            std::cerr << "Failed to start command buffer recording for buffer number: " <<i <<std::endl;
            return false;
        }

        //Commands that you want to record should be placed here.

        if(!CommandBufferManager::endCommandBuffer(commandBuffers[i]))
        {
            std::cerr << "Failed to end command buffer number: " << i << std::endl;
            return false;
        }
    }

    return true;
}
