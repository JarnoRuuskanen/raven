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
            HINSTANCE hInstance = NULL;
            HWND hwnd = NULL;
        #elif defined VK_USE_PLATFORM_XCB_KHR
            xcb_connection_t * connection;
            xcb_window_t      window;
        #elif defined VK_USE_PLATFORM_XLIB_KHR
            Display *display;
            Window   window;
        #endif
    };

    //This class is for creating a vulkan window which a renderer
    //can then render content into. This class handless surface creation
    //etc.
    class VulkanWindow
    {
        public:
            VulkanWindow();
            ~VulkanWindow();
            //Creates a presentation surface for a VulkanWindow.
            bool createWindowSurface(VkInstance instance);
            //Creates the window frame
            bool createWindowFrame(uint16_t width, uint16_t height);
            //Creates a swapchain for the window.
            bool createWindowSwapchain(VkDevice &logicalDevice,
                                       uint32_t queueFamilyIndex,
                                       VkPhysicalDevice &physicalDevice,
                                       VkImageUsageFlags desiredImageUsage,
                                       VkPresentModeKHR &presentationMode,
                                       VkSwapchainKHR &oldSwapchain);
            //Displays the rendered content.
			int play();

            //Returns a reference to the window surface.
            inline VkSurfaceKHR& getPresentationSurface(){ return presentationSurface; }
            //Returns a reference to the window swapchain object.
            inline VkSwapchainKHR& getSwapchain(){ return swapchain; }
            //Returns a reference to the window parameters.
            inline WindowParameters& getWindowParameters(){ return windowParameters; }
            //Returns the swapchain image vector.
            inline std::vector<VkImage>& getImages(){ return swapchainImages; }
            //Returns a handle for swapchain image views.
            inline std::vector<VkImageView>& getImageViews(){ return swapchainImageViews;}
        private:

            //Creates the image views for swapchain images.
            bool createSwapchainColorImageViews(const VkDevice &logicalDevice, VkFormat imageFormat);
            //The window surface
            VkSurfaceKHR presentationSurface;
            //The swapchain of this window. Swapchain describes image formats, number
            //of images used for rendering (image buffering), presentation mode etc.
            //Each window can only have one swapchain at a time.
            VkSwapchainKHR swapchain;
            WindowParameters windowParameters;
            std::vector<VkImage> swapchainImages;
            std::vector<VkImageView> swapchainImageViews;
            bool connectionEstablished = false;


    };
}
