#pragma once
//This file holds all the system settings so
//that they can be easily switched from a single place.
//Also lists where the settings are used in the
//application.

//Image presentation variables:
#define SETTINGS_IMAGE_FORMAT VK_FORMAT_B8G8R8A8_SRGB
#define SETTINGS_IMAGE_COLOR_SPACE VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
#define SETTINGS_TRANSFORM_FLAGS VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR
#define SETTINGS_IMAGE_USAGE_FLAGS VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT

//Define the default window values.
#define SETTINGS_DEFAULT_PRESENTATION_MODE VK_PRESENT_MODE_FIFO_KHR;
#define SETTINGS_WINDOW_SWAPHAIN_IMAGE_COUNT 2
