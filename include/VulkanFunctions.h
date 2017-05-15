#pragma once
//This namespace defines the enabled vulkan functions in the Raven
//namespace.
//Define preprocessor definition
#include "vulkan/vulkan.h"

namespace Raven
{
    //Defining macros
    #define EXPORTED_VULKAN_FUNCTION(name) extern PFN_##name name;
    #define GLOBAL_LEVEL_VULKAN_FUNCTION(name) extern PFN_##name name;
    #define INSTANCE_LEVEL_VULKAN_FUNCTION(name) extern PFN_##name name;
    #define INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(name, extension) extern PFN_##name name;
    #define DEVICE_LEVEL_VULKAN_FUNCTION(name) extern PFN_##name name;
    #define DEVICE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(name, extension) extern PFN_##name name;

    #include "ListOfVulkanFunctions.inl"
}
