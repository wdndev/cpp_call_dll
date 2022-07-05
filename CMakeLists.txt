# @file         - CMakeLists.txt
# @author       - dongnian.wang
# @brief        - 编译文件
# 
# @copyright Copyright (c) 2021 individual

cmake_minimum_required(VERSION 3.10)

project(call_dll)

# 设置C C++语言标准
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
if (${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
    set (CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
endif()

if (NOT DEFINED CMAKE_BUILD_TYPE)
    message(STATUS "CMAKE_BUILD_TYPE not defined, 'Release' will be used")
    set(CMAKE_BUILD_TYPE "Release")
endif()

# 编译成可执行文件路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin) 
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

# 包含子项目
add_subdirectory ("generate_dll")
add_subdirectory ("cpp_call_dll")