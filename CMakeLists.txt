project(Raven)
cmake_minimum_required(VERSION 3.0)

# Define include path
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Gather list of header and source files for compilation. Also add imgui source files.
file(GLOB_RECURSE CPP_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE HPP_FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/*.*)

#This next line is for enabling c++14 because it's not on by default
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

add_executable(${PROJECT_NAME} ${CPP_FILES} ${HPP_FILES})

target_link_libraries(${PROJECT_NAME} ${CMAKE_DL_LIBS} ${PLATFORM_LIBRARY})

