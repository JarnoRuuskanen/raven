#include "VulkanWindow.h"
#include "VulkanStructures.h"

using namespace Raven;

VulkanWindow::VulkanWindow()
{
}

VulkanWindow::~VulkanWindow()
{
    xcb_disconnect (windowParameters.connection);
}

//Creates a presentation surface for VulkanWindow.
bool VulkanWindow::createWindowSurface(VkInstance instance)
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

//Creates the window frame.
bool VulkanWindow::createWindowFrame(uint16_t windowWidth, uint16_t windowHeight)
{
    //Linux xcb
    #ifdef VK_USE_PLATFORM_XCB_KHR
        // First open connection to the X server.
        windowParameters.connection = xcb_connect(NULL, NULL);
        if(xcb_connection_has_error(windowParameters.connection) > 0)
        {
            std::cerr << "Failed to connect to X server!" << std::endl;
            return false;
        }

        // Use first available screen.
        const xcb_setup_t *setup = xcb_get_setup(windowParameters.connection);
        xcb_screen_iterator_t iterator = xcb_setup_roots_iterator(setup);
        xcb_screen_t *screen = iterator.data;

        // Create the window.
        windowParameters.window = xcb_generate_id(windowParameters.connection);
        xcb_create_window(windowParameters.connection,
                          XCB_COPY_FROM_PARENT,
                          windowParameters.window,
                          screen->root,
                          0,0,
                          windowWidth,windowHeight,
                          10,
                          XCB_WINDOW_CLASS_INPUT_OUTPUT,
                          screen->root_visual,
                          0, NULL);

        // Map the window on the screen.
        xcb_map_window(windowParameters.connection, windowParameters.window);

        // Make sure that commands are sent.
        xcb_flush(windowParameters.connection);

        return true;
    #endif

    return false;
}
