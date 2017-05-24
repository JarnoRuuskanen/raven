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
TEST(RavenTest, loadLibraryTest)
{
    void* library;
    EXPECT_EQ(true, Raven::loadVulkanLibrary(library));
}

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

