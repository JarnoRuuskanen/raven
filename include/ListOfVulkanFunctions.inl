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
INSTANCE_LEVEL_VULKAN_FUNCTION(vkGetPhysicalDeviceQueueFamilyProperties)
INSTANCE_LEVEL_VULKAN_FUNCTION(vkGetPhysicalDeviceMemoryProperties)
INSTANCE_LEVEL_VULKAN_FUNCTION(vkCreateDevice)
INSTANCE_LEVEL_VULKAN_FUNCTION(vkDestroyInstance)
INSTANCE_LEVEL_VULKAN_FUNCTION(vkGetDeviceProcAddr)

#undef INSTANCE_LEVEL_VULKAN_FUNCTION

//Instance level vulkan functions from extensions
#ifndef INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION
#define INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION( function, extension )
#endif

INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(vkGetPhysicalDeviceSurfaceSupportKHR, VK_KHR_SURFACE_EXTENSION_NAME)
INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(vkGetPhysicalDeviceSurfaceCapabilitiesKHR, VK_KHR_SURFACE_EXTENSION_NAME)
INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(vkGetPhysicalDeviceSurfaceFormatsKHR, VK_KHR_SURFACE_EXTENSION_NAME)
INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(vkGetPhysicalDeviceSurfacePresentModesKHR, VK_KHR_SURFACE_EXTENSION_NAME)
INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(vkDestroySurfaceKHR, VK_KHR_SURFACE_EXTENSION_NAME)

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

DEVICE_LEVEL_VULKAN_FUNCTION(vkGetDeviceQueue)
DEVICE_LEVEL_VULKAN_FUNCTION(vkDeviceWaitIdle)
DEVICE_LEVEL_VULKAN_FUNCTION(vkDestroyDevice)
DEVICE_LEVEL_VULKAN_FUNCTION(vkCreateBuffer)
DEVICE_LEVEL_VULKAN_FUNCTION(vkCreateBufferView)
DEVICE_LEVEL_VULKAN_FUNCTION(vkGetBufferMemoryRequirements)
DEVICE_LEVEL_VULKAN_FUNCTION(vkCreateImage)
DEVICE_LEVEL_VULKAN_FUNCTION(vkCreateImageView)
DEVICE_LEVEL_VULKAN_FUNCTION(vkGetImageMemoryRequirements)
DEVICE_LEVEL_VULKAN_FUNCTION(vkCreateSemaphore)
DEVICE_LEVEL_VULKAN_FUNCTION(vkDestroySemaphore)
DEVICE_LEVEL_VULKAN_FUNCTION(vkCreateFence)
DEVICE_LEVEL_VULKAN_FUNCTION(vkDestroyFence)
DEVICE_LEVEL_VULKAN_FUNCTION(vkResetFences)
DEVICE_LEVEL_VULKAN_FUNCTION(vkGetFenceStatus)
DEVICE_LEVEL_VULKAN_FUNCTION(vkWaitForFences)
DEVICE_LEVEL_VULKAN_FUNCTION(vkCreateCommandPool)
DEVICE_LEVEL_VULKAN_FUNCTION(vkDestroyCommandPool)
DEVICE_LEVEL_VULKAN_FUNCTION(vkAllocateCommandBuffers)
DEVICE_LEVEL_VULKAN_FUNCTION(vkBeginCommandBuffer)
DEVICE_LEVEL_VULKAN_FUNCTION(vkEndCommandBuffer)
DEVICE_LEVEL_VULKAN_FUNCTION(vkResetCommandBuffer)
DEVICE_LEVEL_VULKAN_FUNCTION(vkFreeCommandBuffers)
DEVICE_LEVEL_VULKAN_FUNCTION(vkResetCommandPool)
DEVICE_LEVEL_VULKAN_FUNCTION(vkQueueSubmit)
DEVICE_LEVEL_VULKAN_FUNCTION(vkQueueWaitIdle)
DEVICE_LEVEL_VULKAN_FUNCTION(vkAllocateMemory)
DEVICE_LEVEL_VULKAN_FUNCTION(vkBindBufferMemory)

#undef DEVICE_LEVEL_VULKAN_FUNCTION

//Device level vulkan functions from extensions
#ifndef DEVICE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION
#define DEVICE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION( function, extension )
#endif

DEVICE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(vkCreateSwapchainKHR, VK_KHR_SWAPCHAIN_EXTENSION_NAME)
DEVICE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(vkGetSwapchainImagesKHR, VK_KHR_SWAPCHAIN_EXTENSION_NAME)
DEVICE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(vkAcquireNextImageKHR, VK_KHR_SWAPCHAIN_EXTENSION_NAME)
DEVICE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(vkQueuePresentKHR, VK_KHR_SWAPCHAIN_EXTENSION_NAME)
DEVICE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(vkDestroySwapchainKHR, VK_KHR_SWAPCHAIN_EXTENSION_NAME)

#undef DEVICE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION
