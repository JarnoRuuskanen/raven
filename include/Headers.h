#pragma once

//***************COMMON HEADERS*****************//
#ifdef _WIN32
#include <Windows.h>
#endif

#include "VulkanDestroyer.h"
#include <iostream>
#include <vector>
#include <array>
#include <string>
#include <cstring>
#include <thread>
#include <functional>

namespace Raven
{
    #if defined _WIN32
        #define LIBRARY_TYPE HMODULE;
    #elif defined __linux
        #include <dlfcn.h>
        #define LIBRARY_TYPE void*
    #endif
}
