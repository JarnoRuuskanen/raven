#include "TestHeaders.h"

using namespace Raven;

struct Library
{
    LIBRARY_TYPE library;
    Library(){}
    ~Library(){}
};

struct LibraryTest : testing::Test
{
    Library* libraryObject;
    LibraryTest()
    {
        libraryObject = new Library();
    }

    virtual ~LibraryTest()
    {
        delete libraryObject;
    }
};

/**Library tests. The load function must be first to enable vulkan functions.*/
TEST_F(LibraryTest, loadLibraryTest)
{
    EXPECT_EQ(true, Raven::loadVulkanLibrary(libraryObject->library));
}

TEST_F(LibraryTest, releaseLibraryTest)
{
    Raven::freeVulkanLibrary(libraryObject->library);
    EXPECT_EQ(nullptr, libraryObject->library);
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

TEST(RavenStructureTest, surfaceCreateInfoTest)
{
    WINDOW_SURFACE_CREATE_INFO createInfo =
            VulkanStructures::surfaceCreateInfo();
    EXPECT_EQ(WINDOW_SURFACE_CREATE_INFO_STRUCTURE_TYPE, createInfo.sType);
    EXPECT_EQ(nullptr, createInfo.pNext);
    EXPECT_EQ(0, createInfo.flags);
}

struct VulkanInstance
{
    VkInstance instance;
    VulkanInstance(){}
};

struct InstanceTest
{
    VulkanInstance* instanceObject;
    InstanceTest()
    {
        instanceObject = new VulkanInstance;
    }
    virtual ~InstanceTest()
    {
        delete instanceObject;
    }
};

TEST(RavenTest, startRaven)
{
    RavenEngine raven;
    EXPECT_TRUE(raven.start("Test"));
}

TEST(RavenTest, startWithoutNameTest)
{
    RavenEngine raven;
    EXPECT_FALSE(raven.start(nullptr));
}

TEST(UtilityFunctionTest, getPhysicalDeviceQueuesWithPropertiesEmptyTest)
{
    VkPhysicalDevice gpu;
    std::vector<VkQueueFamilyProperties> queueFamilies;
    EXPECT_FALSE(getPhysicalDeviceQueuesWithProperties(gpu, queueFamilies));
}

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

