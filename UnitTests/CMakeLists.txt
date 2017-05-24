#project(UnitTests)
#cmake_minimum_required(VERSION 3.1)
#include the main project files so that we can test them.
include_directories(${gtest_SOURCE_DIR}/include)
#Include the main project source files so they can be tested.
include_directories("../src/")
set(test_sources test_main.cpp TestHeaders.h)
add_executable(RunUnitTests ${test_sources})
target_link_libraries(RunUnitTests gtest_main dl xcb)
