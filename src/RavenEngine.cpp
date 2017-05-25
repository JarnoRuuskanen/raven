#include "RavenEngine.h"
#include "VulkanUtility.h"
#include "VulkanStructures.h"
#include "Settings.h"
#include <string.h>

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
    selectedLogicalDevice = VK_NULL_HANDLE;
    selectedPhysicalDevice = VK_NULL_HANDLE;
    vulkanDevice = new VulkanDevice();
    appWindow = new VulkanWindow();
}

RavenEngine::~RavenEngine()
{
    //Destroy the application window
    if(appWindow != nullptr)
        delete appWindow;
    //Destroy the VulkanDevice
    if(vulkanDevice != nullptr)
        delete vulkanDevice;
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

    //First initialize vulkan
    if(!initializeVulkan())
        return false;

    //After vulkan dynamic library, exported- and global-level functions have been loaded,
    //create a new vulkan instance.
    if(!createInstance(selectedInstance, appName, desiredInstanceExtensions))
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
    if(!createVulkanDevice(selectedPhysicalDevice, desiredDeviceExtensions))
        return false;

    //After the vulkan device has been created we need to create a window
    //for the application. This window will display our rendering content.
    if(!openNewWindow(windowWidth, windowHeight, appWindow))
        return false;

    VkPresentModeKHR presentationMode = SETTINGS_DEFAULT_PRESENTATION_MODE;
    VkImageUsageFlags desiredImageUsages = SETTINGS_IMAGE_USAGE_FLAGS;
    //Create a swapchain for the window to use.
    if(!buildSwapchain(desiredImageUsages,presentationMode, appWindow))
        return false;

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

//Creates a new vulkan instance with the given desired instance extensions
bool RavenEngine::createInstance(VkInstance &instance, char const* const appName , std::vector<const char*> &desiredInstanceExtensions)
{
    //Query all available instance extensions
    std::vector<VkExtensionProperties> availableInstanceExtensions;
    if(!checkAvailableInstanceExtensions(availableInstanceExtensions))
    {
        std::cerr << "Failed to check for available instance extensions!" << std::endl;
        return false;
    }

    //Create a new vulkan instance
    if(!createVulkanInstance(desiredInstanceExtensions, appName, instance))
    {
        std ::cerr << "Failed to create a vulkan instance!" << std::endl;
        return false;
    }

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
    vulkanDevice = new VulkanDevice();
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
                                  VulkanWindow *window)
{
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
    swapchainInfo.oldSwapchain = nullptr;

    //Create the actual swapchain for the provided window.
    if(!createSwapchain(vulkanDevice->getLogicalDevice(), swapchainInfo, window->getSwapchain()))
    {
        return false;
    }

    return true;
}
