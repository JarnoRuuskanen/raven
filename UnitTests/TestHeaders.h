#pragma once
#include <iostream>
#include <gtest/gtest.h>
#include <string>
#include <xcb/xcb.h>
/**RAVEN PROJECT INCLUDES IN CORRECT ORDER**/
#include "Settings.h"
#include "ListOfVulkanFunctions.inl"
#include "Headers.h"
#include "VulkanInitializer.h"
#include "VulkanInitializer.cpp"
#include "VulkanFunctions.h"
#include "VulkanFunctions.cpp"
#include "VulkanStructures.h"
/** **/
#include "VulkanDevice.h"
#include "VulkanDevice.cpp"
#include "VulkanUtility.h"
#include "VulkanUtility.cpp"
#include "VulkanWindow.h"
#include "VulkanWindow.cpp"
#include "RavenEngine.h"
#include "RavenEngine.cpp"