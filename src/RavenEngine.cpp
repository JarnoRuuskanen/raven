#include "RavenEngine.h"
#include "VulkanUtility.h"

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

}

RavenEngine::~RavenEngine()
{
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
    createWindow();

    return true;
}

//Initializes the vulkan library
bool RavenEngine::initializeVulkan()
{
    if(!loadVulkanLibrary(vulkanLibrary))
    {
        std::cout << "Failed to load vulkan dynamic library!" << std::endl;
        return false;
    }
    //Load vulkan functions that are defined in ListOfVulkanFunctions.inl
    if(!loadFunctionExportedFromVulkanLoaderLibrary(vulkanLibrary))
    {
        std::cout << "Failed to load functions exported from vulkan loader library!" << std::endl;
        return false;
    }
    if(!loadGlobalLevelFunctions())
    {
        std::cout << "Failed to load global level functions!" << std::endl;
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
        std::cout << "Failed to check for available instance extensions!" << std::endl;
        return false;
    }

    //Create a new vulkan instance
    if(!createVulkanInstance(desiredInstanceExtensions, appName, instance))
    {
        std ::cout << "Failed to create a vulkan instance!" << std::endl;
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
    std::cout << "Failed to find a physical device that supports all the required device extensions!" << std::endl;
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

//Creates a new VulkanWindow
bool RavenEngine::createWindow()
{
       appWindow = new VulkanWindow();
       //XCB window creation
       #ifdef VK_USE_PLATFORM_XCB_KHR
       {
            int screenp = 1;
            windowParameters.connection = xcb_connect("Raven", &screenp);
            windowParameters.window = xcb_generate_id(windowParameters.connection);

            if(!appWindow->createWindowSurface(selectedInstance, windowParameters))
                return false;

            //Make sure that the VulkanDevice's current queue family supports
            //image presentation
            VkBool32 presentationSupported = VK_FALSE;
            VkResult result = vkGetPhysicalDeviceSurfaceSupportKHR(selectedPhysicalDevice,
                                                                   vulkanDevice->getPrimaryQueueFamilyIndex(),
                                                                   appWindow->getPresentationSurface(),
                                                                   &presentationSupported);
            if(result == VK_SUCCESS && presentationSupported == VK_TRUE)
                return true;

            return false;
       }
       #elif defined VK_USE_PLATFORM_WIN32_KHR
       //Win32 implementation
       #elif defined VK_USE_PLATFORM_XLIB_KHR
       //Xlib implementation
       #endif
       return false;
}
