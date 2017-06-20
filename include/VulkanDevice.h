#pragma once
#include "Headers.h"
#include "VulkanImage.h"

struct VulkanQueueInfo
{
    //Index of the queue family
    uint32_t queueFamilyIndex;
    //Family priorities. The number of queues in this family is priorities.size().
    std::vector<float> priorities;
};

//A class which holds and manages logical device operations. Logical devices
//are the main working force in a Vulkan-powered applications.
using namespace Raven;
class VulkanDevice
{
    public:
        VulkanDevice();
        ~VulkanDevice();
        //Initializes the vulkan device
        bool initializeDevice(VkPhysicalDevice &physicalDevice,
                              std::vector<const char*>  &desiredExtensions);
        //Sends commands to the gpu for computing. This function also chooses the
        //queue which the commands will be submitted to.
        bool executeCommands(VkSubmitInfo &submitInfo, VkFence &submitFence);

        //Creates a sampled image.
        bool createSampledImage(VkImageType imageType,
                                VkFormat format,
                                VkExtent3D size,
                                uint32_t numMipmaps,
                                uint32_t numLayers,
                                VkImageUsageFlags usage,
                                VkImageViewType viewType,
                                VkImageAspectFlags aspect,
                                VkBool32 linearFiltering,
                                VulkanImage &sampledImageObject,
                                VkDeviceMemory &memoryObject);

        //Makes a combined image sampler.
        bool createCombinedImageSampler(VkSamplerCreateInfo samplerInfo,
                                        VkImageType imageType,
                                        VkFormat format,
                                        VkExtent3D imageSize,
                                        uint32_t numMipmaps,
                                        uint32_t numLayers,
                                        VkImageUsageFlags usage,
                                        VkImageViewType viewType,
                                        VkImageAspectFlags aspect,
                                        VkSampler &sampler,
                                        VkDeviceMemory &sampledImageMemory,
                                        VulkanImage &sampledImageObject);

        //Returns a queue family reference by index
        inline VkQueueFamilyProperties& getQueueFamily(int index){return queueFamilies[index];}
        //Returns the queueFamilyIndex for the family that is currently used.
        //At this point Raven only uses one queue family which supports all the possible
        //operations. This will most probably change in the future as I experiment with
        //different graphics cards.
        inline uint32_t getPrimaryQueueFamilyIndex(){return primaryQueueFamilyIndex;}
        //Returns a reference to the logical device.
        inline VkDevice &getLogicalDevice(){return logicalDevice;}
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
        uint32_t primaryQueueFamilyIndex;
        //A vector containing all queue family informations.
        std::vector<VulkanQueueInfo> queueFamilyInfo;
        //Holds all of the device queue handles.
        std::vector<VkQueue> deviceQueueHandles;
};
