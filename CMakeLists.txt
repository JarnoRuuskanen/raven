project(Raven)
cmake_minimum_required(VERSION 3.7)

set_property( GLOBAL PROPERTY USE_FOLDERS ON )
set( CMAKE_SUPPRESS_REGENERATION true )

if( CMAKE_BUILD_TYPE STREQUAL "" )
        set( CMAKE_BUILD_TYPE "debug" )
endif()

#Find Vulkan
if (NOT CMAKE_VERSION VERSION_LESS 3.7.0)
	message(STATUS "Using module to find Vulkan")
	find_package(Vulkan)
endif()

IF (NOT Vulkan_FOUND)
	message(FATAL_ERROR "Could not find Vulkan library!")
ENDIF()

#Define platform
if( NOT DEFINED VK_USE_PLATFORM )
        if( WIN32 )
                set( VK_USE_PLATFORM "WIN32" )
        elseif( UNIX )
                set( VK_USE_PLATFORM "XCB" )
        endif()
endif()

if(WIN32)
        set( CMAKE_C_FLAGS_RELWITHDEBINFO    "${CMAKE_C_FLAGS_RELWITHDEBINFO} /MT" )
        set( CMAKE_C_FLAGS_RELEASE           "${CMAKE_C_FLAGS_RELEASE} /MT" )
        set( CMAKE_C_FLAGS_DEBUG             "${CMAKE_C_FLAGS_DEBUG} /MTd" )
        set( CMAKE_CXX_FLAGS_RELWITHDEBINFO  "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /MT" )
        set( CMAKE_CXX_FLAGS_RELEASE         "${CMAKE_CXX_FLAGS_RELEASE} /MT" )
        set( CMAKE_CXX_FLAGS_DEBUG           "${CMAKE_CXX_FLAGS_DEBUG} /MTd" )
        set( PLATFORM_LIBRARY                ${Vulkan_LIBRARY})
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_USE_PLATFORM_WIN32_KHR")
elseif(UNIX)
        set( PLATFORM_LIBRARY dl xcb pthread vulkan)
endif()

if( CMAKE_BUILD_TYPE STREQUAL "debug" )
        add_definitions(-D_DEBUG)
endif()

add_definitions( -DVK_NO_PROTOTYPES )
add_definitions( -DVK_USE_PLATFORM_${VK_USE_PLATFORM}_KHR )
#This next line is for enabling c++17 because it's not on by default
set (CMAKE_CXX_STANDARD 17)

# Define include paths
# STB SETUP Image library to load texture files from multiple formats.
set (EXTDIR "${CMAKE_SOURCE_DIR}/Resources/Libraries/stb")
set (STBINCLUDES "${EXTDIR}")
get_filename_component(STBINC_PREFIX "${STBINCLUDES}" ABSOLUTE)
if(NOT EXISTS ${STBINC_PREFIX})
    message(FATAL_ERROR "Necessary stb headers do not exist: " ${STBINC_PREFIX})
else()
		message("STB IMAGE LIBRARY FOUND AND LOADED!")
endif()
include_directories( ${STBINC_PREFIX} )

# Tiny object loader for loading .obj files.
set(EXTDIR "${CMAKE_SOURCE_DIR}/Resources/Libraries/tinyobjloader-master")
set(OBJINCLUDES "${EXTDIR}")
get_filename_component(OBJINC_PREFIX "${OBJINCLUDES}" ABSOLUTE)
if(NOT EXISTS ${OBJINC_PREFIX})
    message(FATAL_ERROR "Necessary tinyobjloader header does not exist: " ${OBJINC_PREFIX})
else()
		message("TINY OBJECT LOADER FOUND AND LOADED!")
endif()
include_directories(${OBJINC_PREFIX})

# GLM Math library.
set(EXTDIR "${CMAKE_SOURCE_DIR}/Resources/Libraries/glm")
set(GLMINCLUDES "${EXTDIR}")
get_filename_component(GLMINC_PREFIX "${GLMINCLUDES}" ABSOLUTE)
if(NOT EXISTS ${GLMINC_PREFIX})
    message(FATAL_ERROR "Necessary glm header does not exist: " ${GLMINC_PREFIX})
else()
		message("GLM FOUND AND LOADED!")
endif()
include_directories(${GLMINC_PREFIX})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${Vulkan_INCLUDE_DIR})

# Gather list of header and source files for compilation.
file(GLOB_RECURSE CPP_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE HPP_FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/*.*)

#When you want to add the unit tests + compile them, add these two lines.
#add_subdirectory(lib/googletest)
#add_subdirectory(UnitTests)

#Report warning level.
if(MSVC)
    add_definitions(/W2)
endif()

add_definitions(-D_CRT_SECURE_NO_WARNINGS)

add_executable(${PROJECT_NAME} ${CPP_FILES} ${HPP_FILES})
	
target_link_libraries(${PROJECT_NAME} ${CMAKE_DL_LIBS} ${PLATFORM_LIBRARY})

