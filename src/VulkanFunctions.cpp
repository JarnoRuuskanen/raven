#include "VulkanFunctions.h"

//List of kind of functions will be loaded.
//The full list of vulkan functions can be found inside
//ListOfVulkanFunctions.inl
namespace Raven
{
      #define EXPORTED_VULKAN_FUNCTION(name) PFN_##name name;
      #define GLOBAL_LEVEL_VULKAN_FUNCTION(name) PFN_##name name;
      #define INSTANCE_LEVEL_VULKAN_FUNCTION(name) PFN_##name name;
      #define INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(name,extension) PFN_##name name;
      #define DEVICE_LEVEL_VULKAN_FUNCTION(name) PFN_##name name;
      #define DEVICE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(name, extension) PFN_##name name;

      #include "ListOfVulkanFunctions.inl"
}
