# @file         - cpp_call_dll/CMakeLists.txt
# @author       - dongnian.wang
# @brief        - 调用动态库编译文件
# 
# @copyright Copyright (c) 2021 individual

cmake_minimum_required(VERSION 3.10)

project(dynamic_loader)

file(GLOB SOURCES "dynamic_loader.cpp" "main.cpp")

add_executable(dynamic_loader ${SOURCES})
if(UNIX)
    target_link_libraries(dynamic_loader dl )
endif()