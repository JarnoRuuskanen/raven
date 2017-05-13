#include <iostream>
#include "VulkanInitializer.h"

using namespace Raven;
int main()
{

    //First load the vulkan library dynamically
    LIBRARY_TYPE vulkanLibrary;
    loadVulkanLibrary(vulkanLibrary);

    return 0;
}
