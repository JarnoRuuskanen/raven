#include <iostream>
#include <gtest/gtest.h>
#include <string>
#include <xcb/xcb.h>

/**RAVEN PROJECT INCLUDES **/
#include "Settings.h"
#include "ListOfVulkanFunctions.inl"
#include "Headers.h"
#include "VulkanInitializer.h"
#include "VulkanInitializer.cpp"
#include "VulkanFunctions.h"
#include "VulkanFunctions.cpp"
#include "VulkanStructures.h"

using namespace Raven;
TEST(RavenTest, loadAndReleaseLibraryTest)
{
    void* library;
    EXPECT_EQ(true, Raven::loadVulkanLibrary(library));
    Raven::freeVulkanLibrary(library);
    EXPECT_EQ(nullptr, library);
}

TEST(RavenStructureTest, applicationCreateInfoTest)
{
    VkApplicationInfo appInfo = VulkanStructures::applicationInfo();
    EXPECT_EQ(VK_STRUCTURE_TYPE_APPLICATION_INFO, appInfo.sType);
    EXPECT_EQ(nullptr, appInfo.pNext);
}

TEST(RavenStructureTest, instanceCreateInfoTest)
{
    VkInstanceCreateInfo createInfo = VulkanStructures::instanceCreateInfo();
    EXPECT_EQ(VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO, createInfo.sType);
    EXPECT_EQ(nullptr, createInfo.pNext);
    EXPECT_EQ(0, createInfo.flags);
}

TEST(RavenStructureTest, deviceCreateInfoTest)
{
    VkDeviceCreateInfo createInfo = VulkanStructures::deviceCreateInfo();
    EXPECT_EQ(VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO, createInfo.sType);
    EXPECT_EQ(nullptr, createInfo.pNext);
    EXPECT_EQ(0, createInfo.flags);
}

TEST(RavenStructureTest, deviceQueueCreateInfoTest)
{
    uint32_t familyIndex = 0;
    std::vector<float> priorities;
    VkDeviceQueueCreateInfo createInfo =
            VulkanStructures::deviceQueueCreateInfo(familyIndex, priorities);

    EXPECT_EQ(VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO, createInfo.sType);
    EXPECT_EQ(nullptr, createInfo.pNext);
    EXPECT_EQ(0, createInfo.flags);
    EXPECT_EQ(familyIndex, createInfo.queueFamilyIndex);
    EXPECT_EQ(static_cast<uint32_t>(priorities.size()), createInfo.queueCount);
    EXPECT_EQ(priorities.data(), createInfo.pQueuePriorities);
}

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

