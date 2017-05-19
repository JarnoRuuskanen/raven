#pragma once
#include "Headers.h"

//A struct for holding the window parameters for
//different platforms.

namespace Raven
{
    //This can also be done easier with glfw, but I want to know platform specific
    //way as well in case I ever happen to need the knowledge.
    struct WindowParameters
    {
        #ifdef VK_USE_PLATFORM_WIN32_KHR
            HINSTANCE hInstance;
            HWND hwnd;
        #elif defined VK_USE_PLATFORM_XCB_KHR
            xcb_connection_t *connection;
            xcb_window_t      window;
        #elif defined VK_USE_PLATFORM_XLIB_KHR
            Display *display;
            Window   window;
        #endif
    };
}

//This class is for creating a vulkan window which a renderer
//can then render content into. This class handless surface creation
//etc.
using namespace Raven;
class VulkanWindow
{
    public:
        VulkanWindow();
        ~VulkanWindow();
        //Creates a presentation surface for a VulkanWindow.
        bool createWindowSurface(VkInstance instance,
                                 WindowParameters windowParameters);
        //Returns a reference to the window surface
        inline VkSurfaceKHR& getPresentationSurface(){return windowSurface;}
    private:
        VkSurfaceKHR windowSurface;

};
