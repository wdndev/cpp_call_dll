# @file         - generate_dll/CMakeLists.txt
# @author       - dongnian.wang
# @brief        - 生成动态库编译文件
# 
# @copyright Copyright (c) 2021 individual

cmake_minimum_required(VERSION 3.10)

project(generate_dll)

file(GLOB SOURCES "mylib.cpp" )

add_library(generate_dll SHARED ${SOURCES})