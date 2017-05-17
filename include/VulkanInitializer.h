#pragma once
#include "Headers.h"

//Raven namespace
namespace Raven
{
    //Loads the vulkan library. This way you don't need to
    //worry about static linking. vulkan.h defined prototypes
    //are disabled and function pointers are loaded dynamically
    //into the application. This has some performance advantages
    //as we don't need to load the whole library but only those
    //parts that we need for our application.
    bool loadVulkanLibrary(LIBRARY_TYPE& vulkanLibrary);
    //Frees the dynamically loaded vulkan library
    void freeVulkanLibrary(LIBRARY_TYPE& vulkanLibrary);
    //Loads functions exported from vulkan loader library.
    bool loadFunctionExportedFromVulkanLoaderLibrary(LIBRARY_TYPE const &vulkanLibrary);
    //Loads the global level functions described in the list of vulkan functions.
    bool loadGlobalLevelFunctions();
    //Loads instance level vulkan functions.
    bool loadInstanceLevelVulkanFunctions(VkInstance &instance,
                                          std::vector<const char*> const &enabledExtensions);
    //Loads logical device level functions
    bool loadDeviceLevelFunctions(VkDevice &device, std::vector<const char*>  &enabledExtensions);
}

