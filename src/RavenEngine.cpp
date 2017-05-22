#include "RavenEngine.h"
#include "VulkanUtility.h"
#include "VulkanStructures.h"

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
    appWindow = new VulkanWindow();
}

RavenEngine::~RavenEngine()
{
    //Destroy the application window
    delete appWindow;
    //Destroy the vulkan instance
    if(selectedInstance)
    {
        vkDestroyInstance(selectedInstance, nullptr);
        selectedInstance = VK_NULL_HANDLE;
    }

    //Lastly free the dynamically loaded vulkan library
    freeVulkanLibrary(vulkanLibrary);
}

//Starts the raven engine
bool RavenEngine::start(const char* appName)
{
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

    VkPresentModeKHR presentationMode = VK_PRESENT_MODE_FIFO_KHR;
    VkImageUsageFlags desiredImageUsages = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    //Create a swapchain for the window to use.
    if(!createSwapchain(desiredImageUsages,presentationMode, appWindow))
        return false;

    return true;
}

//Initializes the vulkan library
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

//Selects the first physical device that fills all given requirements
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

//Creates a logical vulkan device from given physical device
bool RavenEngine::createVulkanDevice(VkPhysicalDevice &physicalDevice,
                                     std::vector<const char*>  &desiredExtensions)
{
    vulkanDevice = new VulkanDevice();
    if(!vulkanDevice->initializeDevice(physicalDevice, desiredExtensions))
        return false;
    return true;
}

//Creates a new VulkanWindow. VulkanWindow will hold swapchain and all closely
//to a presentation window related objects.
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

//Creates a new swapchain to be used for rendering.
bool RavenEngine::createSwapchain(VkImageUsageFlags desiredImageUsage,
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
        presentationMode = DEFAULT_PRESENTATION_MODE;
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
    VkImageUsageFlags desiredUsages = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    VkImageUsageFlags imageUsage = 0;
    if(!selectSwapchainImageUsage(surfaceCapabilities, desiredUsages, imageUsage))
    {
        std::cerr << "Desired image usage flags were not supported!" << std::endl;
        return false;
    }

    //Select swapchain transformation.
    VkSurfaceTransformFlagBitsKHR desiredTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    VkSurfaceTransformFlagBitsKHR surfaceTransform;
    selectSwapchainSurfaceTransform(surfaceCapabilities, desiredTransform, surfaceTransform);

    //Select swapchain image format and colorspace.
    VkSurfaceFormatKHR desiredSurfaceFormat;
    desiredSurfaceFormat.format = VK_FORMAT_B8G8R8A8_SRGB;
    desiredSurfaceFormat.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

    VkFormat imageFormat;
    VkColorSpaceKHR desiredColorSpace = VK_COLOR_SPACE_DOLBYVISION_EXT;

    if(!selectSwapchainImageFormat(selectedPhysicalDevice,
                                appWindow->getPresentationSurface(),
                                desiredSurfaceFormat,
                                imageFormat, desiredColorSpace))
    {
        return false;
    }

    //Build the swapchain.
    VkSwapchainCreateInfoKHR swapchainInfo = VulkanStructures::swapchainCreateInfo();
    swapchainInfo.imageExtent = imageSizes;
}
