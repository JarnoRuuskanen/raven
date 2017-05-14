#pragma once
#include "VulkanInitializer.h"
//The main class for Raven
class RavenEngine
{
    public:
        RavenEngine();
        ~RavenEngine();
        void start();
        void stop();
    private:
        bool initializeVulkan();
        bool createInstance();

        //Private objects:

        //The vulkan dynamically loaded library
        LIBRARY_TYPE vulkanLibrary;
};
