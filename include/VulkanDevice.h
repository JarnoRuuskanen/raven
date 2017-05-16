#pragma once
#include "Headers.h"
//A class which holds and manages logical device operations
class VulkanDevice
{
    public:
        VulkanDevice(VkPhysicalDevice &device);
        ~VulkanDevice();
        //Initializes the vulkan device
        bool initializeDevice();
    private:
        //Creates a logical device for the VulkanDevice
        bool createDevice();

        //The physical device this class is using.
        VkPhysicalDevice physicalDevice;
        //The logical device created under the physical device
        VkDevice logicalDevice;
        //Every queue family the physical device supports
        std::vector<VkQueueFamilyProperties> queueFamilies;
        //Device queue family indices
        uint32_t graphicsFamilyIndex,
                 computeFamilyIndex,
                 transferFamilyIndex,
                 sparseFamilyIndex;
};
