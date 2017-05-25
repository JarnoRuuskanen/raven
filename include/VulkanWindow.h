#pragma once
#include "Headers.h"
#include "Settings.h"

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
            xcb_connection_t * connection;
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
        bool createWindowSurface(VkInstance instance);
        bool createSwapchain();
        //Creates the window frame
        bool createWindowFrame(uint16_t width, uint16_t height);
        //Returns a reference to the window surface.
        inline VkSurfaceKHR& getPresentationSurface(){return windowPresentationSurface;}
        //Returns a reference to the window swapchain object.
        inline VkSwapchainKHR& getSwapchain(){return swapchain;}
        //Returns a reference to the window parameters.
        inline WindowParameters& getWindowParameters(){return windowParameters;}
    private:
        //The window surface
        VkSurfaceKHR windowPresentationSurface;
        //The swapchain of this window. Swapchain describes image formats, number
        //of images used for rendering (image buffering), presentation mode etc.
        VkSwapchainKHR swapchain;
        WindowParameters windowParameters;
        bool connectionEstablished = false;


};
