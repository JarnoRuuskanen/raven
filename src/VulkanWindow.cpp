#include "VulkanWindow.h"
#include "VulkanStructures.h"

using namespace Raven;

VulkanWindow::VulkanWindow()
{
}

VulkanWindow::~VulkanWindow()
{
}

//Creates a presentation surface for VulkanWindow.
bool VulkanWindow::createWindowSurface(VkInstance instance,
                                       WindowParameters windowParameters)
{
    VkResult result;
    WINDOW_SURFACE_CREATE_INFO createInfo = VulkanStructures::surfaceCreateInfo();
    windowPresentationSurface = VK_NULL_HANDLE;

    //Define functionality for each platform.
    #ifdef VK_USE_PLATFORM_WIN32_KHR
        createInfo.HINSTANCE = windowParameters.hInstance;
        createInfo.HWND = windowParameters.hwnd;
        result = vkCreateWin32SurfaceKHR(instance, &createInfo, nullptr, &windowSurface);
    #elif defined VK_USE_PLATFORM_XCB_KHR
        createInfo.connection = windowParameters.connection;
        createInfo.window = windowParameters.window;
        result = vkCreateXcbSurfaceKHR(instance, &createInfo, nullptr, &windowPresentationSurface);
    #elif defined VK_USE_PLATFORM_XLIB_KHR
        createInfo.display = windowParameter.dpy;
        createInfo.window = windowParameters.window;
        result = vkCreateXlibSurfaceKHR(instance, &createInfo, nullptr, &windowSurface);
    #endif

    //Make sure that the presentation surface creation was successful.
    if((result != VK_SUCCESS) || (windowPresentationSurface == VK_NULL_HANDLE))
    {
        std::cerr << "Failed to create a presentation surface!" << std::endl;
        return false;
    }
    return true;
}
