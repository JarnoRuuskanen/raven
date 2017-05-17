#pragma once
#include "Headers.h"

struct VulkanQueueInfo
{
    //Index of the queue family
    uint32_t queueFamilyIndex;
    //Family priorities. The number of queues in this family is priorities.size().
    std::vector<float> priorities;
};

//A class which holds and manages logical device operations. Logical devices
//are the main working force in a Vulkan-powered applications.
class VulkanDevice
{
    public:
        VulkanDevice(VkPhysicalDevice &physicalDevice);
        ~VulkanDevice();
        //Initializes the vulkan device
        bool initializeDevice(std::vector<const char*>  &desiredExtensions);
    private:
        //Creates a logical device for the VulkanDevice
        bool createDevice();
        //Initializes the queues this logical vulkan device will be using
        bool initializeQueues(std::vector<VulkanQueueInfo> &familyInfo);

        //The physical device this class is using.
        VkPhysicalDevice physicalDevice;
        //The logical device created under the physical device
        VkDevice logicalDevice;
        //Every queue family the physical device supports
        std::vector<VkQueueFamilyProperties> queueFamilies;
        //Device queue family indices
        uint32_t queueFamilyIndex;
        //A vector containing all queue family informations.
        std::vector<VulkanQueueInfo> queueFamilyInfo;
};
