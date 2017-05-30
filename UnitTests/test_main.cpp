#include "TestHeaders.h"

using namespace Raven;
using namespace CommandBufferManager;

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

/**INSTANCE TESTING STARTS**/
struct TestInstance : testing::Test
{
    std::vector<const char*> testInstanceExtensions =
    {
        VK_KHR_SURFACE_EXTENSION_NAME,
        PLATFORM_SURFACE_EXTENSION
    };

    VkInstance testInstance;
    TestInstance()
    {
        if(createVulkanInstance(testInstanceExtensions, "TestName", testInstance) != VK_SUCCESS)
        {
            std::cerr << "Failed to create a test instance!" << std::endl;
        }
    }
    virtual ~TestInstance()
    {

    }
};

/**INSTANCE TESTING ENDS**/

/**STRUCTURE TESTS START**/
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
    uint32_t queueCount = 1;
    std::vector<float> priorities(1,1.0f);
    VkDeviceQueueCreateInfo createInfo =
            VulkanStructures::deviceQueueCreateInfo(familyIndex, priorities);

    EXPECT_EQ(VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO, createInfo.sType);
    EXPECT_EQ(nullptr, createInfo.pNext);
    EXPECT_EQ(0, createInfo.flags);
    EXPECT_EQ(familyIndex, createInfo.queueFamilyIndex);
    EXPECT_EQ(queueCount, createInfo.queueCount);
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

/**STRUCTURE TESTS END**/

/**RAVEN MAIN CLASS TESTS START**/
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
/**RAVEN MAIN CLASS TESTS END**/

/**UTILITY FUNCTIONS TESTS START**/
TEST(UtilityFunctionTest, getPhysicalDeviceQueuesWithPropertiesEmptyTest)
{

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    std::vector<VkQueueFamilyProperties> queueFamilies;
    EXPECT_FALSE(getPhysicalDeviceQueuesWithProperties(gpu, queueFamilies));
}

TEST(UtilityFunctionTest, createInstanceTest)
{
    std::vector<const char*> testInstanceExtensions =
    {
        VK_KHR_SURFACE_EXTENSION_NAME
    };
    VkInstance testInstance;
    EXPECT_TRUE(createVulkanInstance(testInstanceExtensions, "TestName", testInstance));
}

/*This test should fail since the logical device or swapchain info
  hasn't been correctly initialized.*/
TEST(UtilityFunctionTest, createSwapchainWithEmptyObjectsTest)
{
    VkSwapchainCreateInfoKHR testCreateInfo;
    VkSwapchainKHR testSwapchain;
    VkDevice testLogicalDevice = VK_NULL_HANDLE;
    EXPECT_FALSE(createSwapchain(testLogicalDevice, testCreateInfo, testSwapchain));
}
/**UTILITY FUNCTIONS TESTS START**/

/**PHYSICAL DEVICE TESTS START**/
struct PhysicalDevice
{
    std::vector<VkPhysicalDevice> gpus;
    VkPhysicalDevice gpu;
    PhysicalDevice(){}
    virtual ~PhysicalDevice(){}
};

struct PhysicalDeviceTest : testing::Test
{
    PhysicalDevice* testPhysicalDevice;
    PhysicalDeviceTest()
    {
        testPhysicalDevice = new PhysicalDevice();
    }
    virtual ~PhysicalDeviceTest()
    {
        delete testPhysicalDevice;
    }
};

TEST_F(PhysicalDeviceTest, failDeviceExtensionCheck)
{
    /**Create an instance**/
    std::vector<const char*> testInstanceExtensions =
    {
        VK_KHR_SURFACE_EXTENSION_NAME
    };
    VkInstance testInstance;
    EXPECT_TRUE(createVulkanInstance(testInstanceExtensions, "TestName", testInstance));

    /**Values that should fail the test since they are
     * instance level values and not device level values.*/
    std::vector<const char*> testFailValues =
    {
        VK_KHR_SURFACE_EXTENSION_NAME
    };

    loadPhysicalDevices(testInstance, testPhysicalDevice->gpus);

    for(int i = 0; i < testPhysicalDevice->gpus.size(); i++)
    {
        EXPECT_FALSE(arePhysicalDeviceExtensionsSupported(testPhysicalDevice->gpus.at(i), testFailValues));
    }
}

TEST_F(PhysicalDeviceTest, passDeviceExtensionCheck)
{
    /**Create an instance**/
    std::vector<const char*> testInstanceExtensions =
    {
        VK_KHR_SURFACE_EXTENSION_NAME
    };
    VkInstance testInstance;
    EXPECT_TRUE(createVulkanInstance(testInstanceExtensions, "TestName", testInstance));

    EXPECT_TRUE(loadPhysicalDevices(testInstance, testPhysicalDevice->gpus));

    std::vector<const char*> testGoodValues =
    {
         VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
    for(int i = 0; i < testPhysicalDevice->gpus.size(); i++)
    {
        EXPECT_TRUE(arePhysicalDeviceExtensionsSupported(testPhysicalDevice->gpus.at(i), testGoodValues));
    }
}

TEST_F(PhysicalDeviceTest, createLogicalDeviceTest)
{
    /**Create an instance**/
    std::vector<const char*> testInstanceExtensions =
    {
        VK_KHR_SURFACE_EXTENSION_NAME,
        PLATFORM_SURFACE_EXTENSION
    };

    VkInstance testInstance;
    EXPECT_TRUE(createVulkanInstance(testInstanceExtensions, "TestName", testInstance));

    EXPECT_TRUE(loadPhysicalDevices(testInstance, testPhysicalDevice->gpus));

    std::vector<const char*> testGoodValues =
    {
         VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
    for(int i = 0; i < testPhysicalDevice->gpus.size(); i++)
    {
        EXPECT_TRUE(arePhysicalDeviceExtensionsSupported(testPhysicalDevice->gpus.at(i), testGoodValues));
    }

    VkDevice testDevice;
    VkDeviceCreateInfo createInfo = VulkanStructures::deviceCreateInfo();
    createInfo.enabledExtensionCount = 0;
    createInfo.enabledLayerCount = 0;
    createInfo.pEnabledFeatures = nullptr;
    createInfo.ppEnabledExtensionNames = nullptr;
    createInfo.ppEnabledLayerNames = nullptr;
    createInfo.pQueueCreateInfos = nullptr;
    createInfo.queueCreateInfoCount = 0;

    EXPECT_TRUE(createLogicalDevice(testPhysicalDevice->gpus[0], createInfo, testDevice));
}

TEST_F(PhysicalDeviceTest, createCommandPoolTest)
{
    /**Create an instance**/
    std::vector<const char*> testInstanceExtensions =
    {
        VK_KHR_SURFACE_EXTENSION_NAME,
        PLATFORM_SURFACE_EXTENSION
    };

    VkInstance testInstance;
    EXPECT_TRUE(createVulkanInstance(testInstanceExtensions, "TestName", testInstance));

    EXPECT_TRUE(loadPhysicalDevices(testInstance, testPhysicalDevice->gpus));

    std::vector<const char*> testGoodValues =
    {
         VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
    for(int i = 0; i < testPhysicalDevice->gpus.size(); i++)
    {
        EXPECT_TRUE(arePhysicalDeviceExtensionsSupported(testPhysicalDevice->gpus.at(i), testGoodValues));
    }

    VkDevice testDevice;
    VkDeviceCreateInfo createInfo = VulkanStructures::deviceCreateInfo();
    createInfo.enabledExtensionCount = 0;
    createInfo.enabledLayerCount = 0;
    createInfo.pEnabledFeatures = nullptr;
    createInfo.ppEnabledExtensionNames = nullptr;
    createInfo.ppEnabledLayerNames = nullptr;
    createInfo.pQueueCreateInfos = nullptr;
    createInfo.queueCreateInfoCount = 0;

    EXPECT_TRUE(createLogicalDevice(testPhysicalDevice->gpus[0], createInfo, testDevice));

    VkCommandPool cmdPool;
    VkCommandPoolCreateInfo poolInfo = VulkanStructures::commandPoolCreateInfo(0);
    EXPECT_TRUE(createCommandPool(testDevice, poolInfo, cmdPool));
}

/**PHYSICAL DEVICE TESTS END**/

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

