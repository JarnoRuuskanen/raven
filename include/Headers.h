#pragma once

//***************COMMON HEADERS*****************//
#include "VulkanDestroyer.h"
#include <iostream>
#include <vector>
#include <array>
#include <string>
#include <cstring>
#include <thread>
#include <functional>
#include <memory>
#include <future>
#include "glm/glm.hpp"

//Platform specific variables will be define here
namespace Raven
{
    //Windows specific definitions
    #if defined _WIN32
        #include <Windows.h>
        #define LIBRARY_TYPE HMODULE;
    //Linux specific definitions
    #elif defined __linux
        #include <dlfcn.h>
        #define LIBRARY_TYPE void*
    #endif

    //Platform specific definitions
    #ifdef VK_USE_PLATFORM_WIN32_KHR
        #define PLATFORM_SURFACE_EXTENSION VK_KHR_WIN32_SURFACE_EXTENSION_NAME
        #define WINDOW_SURFACE_CREATE_INFO_STRUCTURE_TYPE VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR
        #define WINDOW_SURFACE_CREATE_INFO VkWin32SurfaceCreateInfoKH
    #elif defined VK_USE_PLATFORM_XCB_KHR
        #define PLATFORM_SURFACE_EXTENSION VK_KHR_XCB_SURFACE_EXTENSION_NAME
        #define WINDOW_SURFACE_CREATE_INFO_STRUCTURE_TYPE VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR
        #define WINDOW_SURFACE_CREATE_INFO VkXcbSurfaceCreateInfoKHR
    #elif defined VK_USE_PLATFORM_XLIB_KHR
        #define PLATFORM_SURFACE_EXTENSION VK_KHR_XLIB_SURFACE_EXTENSION_NAME
        #define WINDOW_SURFACE_CREATE_INFO_STRUCTURE_TYPE VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR
        #define WINDOW_SURFACE_CREATE_INFO VkXlibSurfaceCreateInfoKHR
    #endif
}
