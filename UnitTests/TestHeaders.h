#pragma once
#include <iostream>
#include <gtest/gtest.h>
#include <string>
#include <xcb/xcb.h>
/**RAVEN PROJECT INCLUDES IN CORRECT ORDER.
   REMEMBER TO ALSO INCLUDE THE CPP-FILES
   OR THE PROJECT WON'T COMPILE!*/
#include "Settings.h"
#include "ListOfVulkanFunctions.inl"
#include "Headers.h"
#include "CommandBufferManager.h"
#include "CommandBufferManager.cpp"
#include "VulkanInitializer.h"
#include "VulkanInitializer.cpp"
#include "VulkanFunctions.h"
#include "VulkanFunctions.cpp"
#include "VulkanStructures.h"
#include "VulkanDescriptorManager.h"
#include "VulkanDescriptorManager.cpp"
/** **/
#include "VulkanDevice.h"
#include "VulkanDevice.cpp"
#include "VulkanUtility.h"
#include "VulkanUtility.cpp"
#include "VulkanWindow.h"
#include "VulkanWindow.cpp"
#include "VulkanRenderer.h"
#include "VulkanRenderer.cpp"
#include "RavenEngine.h"
#include "RavenEngine.cpp"
