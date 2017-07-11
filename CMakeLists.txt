project(Raven)
cmake_minimum_required(VERSION 3.1)

set_property( GLOBAL PROPERTY USE_FOLDERS ON )
set( CMAKE_SUPPRESS_REGENERATION true )

if( CMAKE_BUILD_TYPE STREQUAL "" )
        set( CMAKE_BUILD_TYPE "debug" )
endif()

#Define platform
if( NOT DEFINED VK_USE_PLATFORM )
        if( WIN32 )
                set( VK_USE_PLATFORM "WIN32" )
        elseif( UNIX )
                set( VK_USE_PLATFORM "XCB" )
        endif()
endif()

if( USE_PLATFORM STREQUAL "VK_USE_PLATFORM_WIN32_KHR" )
        set( CMAKE_C_FLAGS_RELWITHDEBINFO    "${CMAKE_C_FLAGS_RELWITHDEBINFO} /MT" )
        set( CMAKE_C_FLAGS_RELEASE           "${CMAKE_C_FLAGS_RELEASE} /MT" )
        set( CMAKE_C_FLAGS_DEBUG             "${CMAKE_C_FLAGS_DEBUG} /MTd" )
        set( CMAKE_CXX_FLAGS_RELWITHDEBINFO  "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /MT" )
        set( CMAKE_CXX_FLAGS_RELEASE         "${CMAKE_CXX_FLAGS_RELEASE} /MT" )
        set( CMAKE_CXX_FLAGS_DEBUG           "${CMAKE_CXX_FLAGS_DEBUG} /MTd" )
        set( PLATFORM_LIBRARY                "" )
elseif( USE_PLATFORM STREQUAL "VK_USE_PLATFORM_XCB_KHR" )
        add_definitions( -std=c++14 )
        set( PLATFORM_LIBRARY dl xcb )
elseif( USE_PLATFORM STREQUAL "VK_USE_PLATFORM_XLIB_KHR" )
        add_definitions( -std=c++14 )
        set( PLATFORM_LIBRARY dl X11 ldl)
endif()

if( CMAKE_BUILD_TYPE STREQUAL "debug" )
        add_definitions(-D_DEBUG)
endif()

add_definitions( -DVK_NO_PROTOTYPES )
add_definitions( -DVK_USE_PLATFORM_${VK_USE_PLATFORM}_KHR )

# Define include paths
# STB SETUP Image library to load texture files from multiple formats.
set (EXTDIR "${CMAKE_SOURCE_DIR}/Resources/Libraries/stb")
set (STBINCLUDES "${EXTDIR}")
get_filename_component(STBINC_PREFIX "${STBINCLUDES}" ABSOLUTE)
if(NOT EXISTS ${STBINC_PREFIX})
    message(FATAL_ERROR "Necessary stb headers do not exist: " ${STBINC_PREFIX})
endif()
include_directories( ${STBINC_PREFIX} )

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Gather list of header and source files for compilation.
file(GLOB_RECURSE CPP_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE HPP_FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/*.*)

#This next line is for enabling c++14 because it's not on by default
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

#When you want to add the unit tests + compile them, add these two lines.
add_subdirectory(lib/googletest)
add_subdirectory(UnitTests)

add_executable(${PROJECT_NAME} ${CPP_FILES} ${HPP_FILES})

target_link_libraries(${PROJECT_NAME} ${CMAKE_DL_LIBS} ${PLATFORM_LIBRARY} xcb pthread)

