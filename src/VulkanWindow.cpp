#include "VulkanWindow.h"
#include "VulkanStructures.h"
#include "VulkanUtility.h"

namespace Raven
{
    /**
     * @brief VulkanWindow::VulkanWindow
     */
    VulkanWindow::VulkanWindow()
    {
    }

    /**
     * @brief VulkanWindow::~VulkanWindow
     */
	VulkanWindow::~VulkanWindow()
	{
		#ifdef UNIX
				if (connectionEstablished == true)
					xcb_disconnect(windowParameters.connection);
		#endif
	}

#ifdef VK_USE_PLATFORM_WIN32_KHR
		LRESULT CALLBACK WindowsEventHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
		{
			VulkanWindow * window = reinterpret_cast<VulkanWindow*>(
				GetWindowLongPtrW(hWnd, GWLP_USERDATA));

			switch (uMsg) {
			case WM_CLOSE:
				return 0;
			case WM_SIZE:
				// we get here if the window has changed size, we should rebuild most
				// of our window resources before rendering to this window again.
				// ( no need for this because our window sizing by hand is disabled )
				break;
			default:
				break;
			}
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		}
#endif

    /**
     * @brief Creates a presentation surface for VulkanWindow.
     * @param instance
     * @return
     */
    bool VulkanWindow::createWindowSurface(VkInstance instance)
    {
        VkResult result;
        WINDOW_SURFACE_CREATE_INFO createInfo = VulkanStructures::surfaceCreateInfo();
        presentationSurface = VK_NULL_HANDLE;

        //Define functionality for each platform.
        #ifdef VK_USE_PLATFORM_WIN32_KHR
            createInfo.hinstance = windowParameters.hInstance;
            createInfo.hwnd = windowParameters.hwnd;
            result = vkCreateWin32SurfaceKHR(instance, &createInfo, nullptr, &presentationSurface);
        #elif defined VK_USE_PLATFORM_XCB_KHR
            createInfo.connection = windowParameters.connection;
            createInfo.window = windowParameters.window;
            result = vkCreateXcbSurfaceKHR(instance, &createInfo, nullptr, &presentationSurface);
        #elif defined VK_USE_PLATFORM_XLIB_KHR
            createInfo.display = windowParameter.dpy;
            createInfo.window = windowParameters.window;
            result = vkCreateXlibSurfaceKHR(instance, &createInfo, nullptr, &windowSurface);
        #endif

        //Make sure that the presentation surface creation was successful.
        if((result != VK_SUCCESS) || (presentationSurface == VK_NULL_HANDLE))
        {
            std::cerr << "Failed to create a presentation surface!" << std::endl;
            return false;
        }
        return true;
    }

    /**
     * @brief Creates the window frame.
     * @param windowWidth
     * @param windowHeight
     * @return
     */
	int counter = 0;
    bool VulkanWindow::createWindowFrame(uint16_t windowWidth, uint16_t windowHeight)
    {
		#ifdef VK_USE_PLATFORM_WIN32_KHR
		counter++;
		WNDCLASSEX windowClass{};
		assert(windowWidth > 0);
		assert(windowHeight > 0);

		std::string className = "Raven Engine window " + std::to_string(counter);
		std::string windowName = "Raven";
		windowParameters.hInstance = GetModuleHandle(nullptr);

		windowClass.cbSize = sizeof(WNDCLASSEX);
		windowClass.style = CS_HREDRAW | CS_VREDRAW;
		windowClass.lpfnWndProc = WindowsEventHandler;
		windowClass.cbClsExtra = 0;
		windowClass.cbWndExtra = 0;
		windowClass.hInstance = windowParameters.hInstance; // hInstance
		windowClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		windowClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
		windowClass.lpszMenuName = NULL;
		windowClass.lpszClassName = className.c_str();
		windowClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);

		//Register the window class.
		if (!RegisterClassEx(&windowClass))
		{
			assert(0 && "Failed to create a window for drawing!");
			return false;
		}

		DWORD exStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

		RECT wr = {0,0,LONG(windowWidth), LONG(windowHeight)};
		AdjustWindowRectEx(&wr, style, FALSE, exStyle);

		windowParameters.hwnd = CreateWindowEx(
			0,							//Optional window styles.
			className.c_str(),			//Window class.
			windowName.c_str(),			//Window text.
			style,						//Window style.
			CW_USEDEFAULT,				//Size and position
			CW_USEDEFAULT,				//
			wr.right - wr.left,			//
			wr.bottom - wr.top,			//
			NULL,						//Parent window.
			NULL,						//Menu.
			windowParameters.hInstance,	//Instance handle.
			NULL);						//Additional application data.

		if (!windowParameters.hwnd)
		{
			assert(0, "Create Window failed!");
			return false;
		}

		SetWindowLongPtr(windowParameters.hwnd, GWLP_USERDATA, (LONG_PTR)this);

		ShowWindow(windowParameters.hwnd, SW_SHOW);
		SetForegroundWindow(windowParameters.hwnd);
		SetFocus(windowParameters.hwnd);

		return true;

        //Linux xcb
        #elif VK_USE_PLATFORM_XCB_KHR
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

            // Create the window. Also describe the masks for event handling.
            uint32_t mask = XCB_CW_EVENT_MASK;
            uint32_t events [1];
            events[0] = XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_EXPOSURE;

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
                              mask, events);

            // Map the window on the screen.
            xcb_map_window(windowParameters.connection, windowParameters.window);

            // Make sure that commands are sent.
            xcb_flush(windowParameters.connection);
            connectionEstablished = true;
            return true;
        #endif

        return false;
    }

    /**
     * @brief Creates a swapchain and generates images and image views
     *        for the VulkanWindow for rendering.
     * @param logicalDevice
     * @param queueFamilyIndex
     * @param physicalDevice
     * @param desiredImageUsage
     * @param presentationMode
     * @param oldSwapchain
     * @return False if any of the operations fails.
     */
    bool VulkanWindow::createWindowSwapchain(VkDevice &logicalDevice,
                                             uint32_t queueFamilyIndex,
                                             VkPhysicalDevice &physicalDevice,
                                             VkImageUsageFlags desiredImageUsage,
                                             VkPresentModeKHR &presentationMode,
                                             VkSwapchainKHR &oldSwapchain)
    {
        //Since each window can only have one swapchain at a time,
        //we need to destroy the old swapchain if one was present.)
        if(oldSwapchain != VK_NULL_HANDLE)
        {
            vkDestroySwapchainKHR(logicalDevice, oldSwapchain, nullptr);
            oldSwapchain = VK_NULL_HANDLE;
        }

        //Swapchain:
        //Make sure that the VulkanDevice's current queue family supports
        //image presentation.
        VkBool32 presentationSupported = VK_FALSE;
        VkResult result = vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice,
                                                               queueFamilyIndex,
                                                               presentationSurface,
                                                               &presentationSupported);
        if((result != VK_SUCCESS) || (presentationSupported != VK_TRUE))
            return false;

        //Check if the desired presentation mode is supported. If not, select a default presentation mode.
        if(!isPresentationModeSupported(physicalDevice,
                                        presentationSurface, presentationMode))
        {
            presentationMode = SETTINGS_DEFAULT_PRESENTATION_MODE;
        }

        //Next create the required information for a swapchain creation.
        VkSurfaceCapabilitiesKHR surfaceCapabilities;
        if(!getSurfaceCapabilities(physicalDevice, presentationSurface, surfaceCapabilities))
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
        desiredSurfaceFormat.format = VK_FORMAT_R8G8B8A8_UNORM;
        desiredSurfaceFormat.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

        //Image format and color space values will be saved to these two variables.
        VkFormat imageFormat;
        VkColorSpaceKHR colorSpace;
        if(!selectSwapchainImageFormat(physicalDevice,
                                       presentationSurface,
                                       desiredSurfaceFormat,
                                       imageFormat, colorSpace))
        {
            return false;
        }

        //Build the swapchain.
        VkSwapchainCreateInfoKHR swapchainInfo = VulkanStructures::swapchainCreateInfo();
        swapchainInfo.surface = presentationSurface;
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
        if(!createSwapchain(logicalDevice, swapchainInfo, swapchain))
        {
            return false;
        }

        //Get the swapchain images.
        if(!getSwapchainImages(logicalDevice, swapchain, swapchainImages))
            return false;

        //Lastly create the color image views for the swapchain images.
        if(!createSwapchainColorImageViews(logicalDevice, imageFormat))
            return false;

        return true;
    }

    /**
     * @brief Creates the image views for swapchain images.
     * @return False if the color image views could not be created.
     */
    bool VulkanWindow::createSwapchainColorImageViews(const VkDevice &logicalDevice, VkFormat imageFormat)
    {
        //Clear old image views if any exist and resize the vector.
        swapchainImageViews.clear();
        swapchainImageViews.resize(swapchainImages.size());

        //For each swapchain image, create a new image view.
        for(size_t i = 0; i < swapchainImages.size(); ++i)
        {
            VkImageViewCreateInfo createInfo =
                    VulkanStructures::imageViewCreateInfo(swapchainImages[i],
                                                          imageFormat,
                                                          VK_IMAGE_ASPECT_COLOR_BIT,
                                                          VK_IMAGE_VIEW_TYPE_2D);

            //Create the actual image view.
            if(!createImageView(logicalDevice, createInfo, swapchainImageViews[i]))
                return false;
        }

        return true;
    }

	/**
	* @brief Displays the rendered content.
	*/
	int VulkanWindow::play()
	{
#ifdef VK_USE_PLATFORM_XCB_KHR
		xcb_generic_event_t *event;
		while ((event = xcb_wait_for_event(windowParameters.connection)))
		{
			switch (event->response_type & ~0x80)
			{
			case XCB_EXPOSE:
			{
				std::cout << "Window exposed!" << std::endl;
				break;
			}

			case XCB_BUTTON_PRESS:
			{
				std::cout << "Button pressed!" << std::endl;
				break;
			}

			default:
			{
				//By default, render content.
				break;
			}
			}

			free(event);
		}
#elif defined VK_USE_PLATFORM_WIN32_KHR
		MSG msg;
		while (GetMessage(&msg, NULL, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		return (int)msg.wParam;
#endif
		return 0;
	}
}
