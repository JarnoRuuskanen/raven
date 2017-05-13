#pragma once
#include "Headers.h"

//Loads the vulkan library. This way you don't need to
//worry about static linking. vulkan.h defined prototypes
//are disabled and function pointers are loaded dynamically
//into the application. This has some performance advantages
//as we don't need to load the whole library but only those
//parts that we need for our application.
#if defined _WIN32
    #define LIBRARY_TYPE HMODULE;
#elif defined __linux
    #include <dlfcn.h>
    #define LIBRARY_TYPE void*
#endif

//Raven namespace
namespace Raven
{
    bool loadVulkanLibrary(LIBRARY_TYPE& vulkanLibrary);
}

