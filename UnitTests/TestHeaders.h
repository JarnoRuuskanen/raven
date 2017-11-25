#pragma once
#include <iostream>
#include <gtest/gtest.h>
#include <string>
#ifdef UNIX
	#include <xcb/xcb.h>
#endif
/**RAVEN PROJECT INCLUDES IN CORRECT ORDER.
   REMEMBER TO ALSO INCLUDE THE CPP-FILES
   OR THE PROJECT WON'T COMPILE!*/
#include "Settings.h"
#include "ListOfVulkanFunctions.inl"
#include "VulkanDestroyer.h"
#include "VulkanDestroyer.cpp"
#include "Headers.h"
#include "FileIO.h"
#include "FileIO.cpp"
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
#include "VulkanPipeline.h"
#include "VulkanPipeline.cpp"
#include "RavenEngine.h"
#include "RavenEngine.cpp"
#include "GraphicsObject.h"
#include "GraphicsObject.cpp"
