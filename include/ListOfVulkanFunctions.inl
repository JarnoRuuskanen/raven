//Enabled vulkan functions.
//This file was written following the VulkanCookbook

//Exported vulkan functions
#ifndef EXPORTED_VULKAN_FUNCTION
#define EXPORTED_VULKAN_FUNCTION( function )
#endif

EXPORTED_VULKAN_FUNCTION(vkGetInstanceProcAddr)

#undef EXPORTED_VULKAN_FUNCTION

//Global vulkan functions
#ifndef GLOBAL_LEVEL_VULKAN_FUNCTION
#define GLOBAL_LEVEL_VULKAN_FUNCTION( function )
#endif

GLOBAL_LEVEL_VULKAN_FUNCTION(vkEnumerateInstanceExtensionProperties)
GLOBAL_LEVEL_VULKAN_FUNCTION(vkEnumerateInstanceLayerProperties)
GLOBAL_LEVEL_VULKAN_FUNCTION(vkCreateInstance)

#undef GLOBAL_LEVEL_VULKAN_FUNCTION

//Instance level vulkan functions. Used mainly for operations on physical devices.
#ifndef INSTANCE_LEVEL_VULKAN_FUNCTION
#define INSTANCE_LEVEL_VULKAN_FUNCTION( function )
#endif

INSTANCE_LEVEL_VULKAN_FUNCTION(vkEnumeratePhysicalDevices)
INSTANCE_LEVEL_VULKAN_FUNCTION(vkEnumerateDeviceExtensionProperties)
INSTANCE_LEVEL_VULKAN_FUNCTION(vkGetPhysicalDeviceProperties)
INSTANCE_LEVEL_VULKAN_FUNCTION(vkGetPhysicalDeviceFeatures)
INSTANCE_LEVEL_VULKAN_FUNCTION(vkCreateDevice)
INSTANCE_LEVEL_VULKAN_FUNCTION(vkGetDeviceProcAddr)

#undef INSTANCE_LEVEL_VULKAN_FUNCTION

//Instance level vulkan functions from extensions
#ifndef INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION
#define INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION( function, extension )
#endif

INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(vkGetPhysicalDeviceSurfaceSupportKHR, VK_KHR_SURFACE_EXTENSION_NAME)
INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(vkGetPhysicalDeviceSurfaceCapabilitiesKHR, VK_KHR_SURFACE_EXTENSION_NAME)
INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(vkGetPhysicalDeviceSurfaceFormatsKHR, VK_KHR_SURFACE_EXTENSION_NAME)

//What platform are we using?
#ifdef VK_USE_PLATFORM_WIN32_KHR
INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(vkCreateWin32SurfaceKHR, VK_KHR_WIN32_SURFACE_EXTENSION_NAME)
#elif defined VK_USE_PLATFORM_XCB_KHR
INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(vkCreateXcbSurfaceKHR, VK_KHR_XCB_SURFACE_EXTENSION_NAME)
#elif defined VK_USE_PLATFORM_XLIB_KHR
INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(vkCreateXlibSurfaceKHR, VK_KHR_XLIB_SURFACE_EXTENSION_NAME)
#endif

#undef INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION

//Device level vulkan functions.
//All device level functions have one of the following as the first parameter:
//VkDevice, VkQueue or VkCommandBuffer.
#ifndef DEVICE_LEVEL_VULKAN_FUNCTION
#define DEVICE_LEVEL_VULKAN_FUNCTION( function )
#endif

#undef DEVICE_LEVEL_VULKAN_FUNCTION

//Device level vulkan functions from extensions
#ifndef DEVICE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION
#define DEVICE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION( function, extension )
#endif

#undef DEVICE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION
