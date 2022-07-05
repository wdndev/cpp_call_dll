/**
 * @file        - mylib.h
 * @author      - dongnian.wang
 * @brief       - 测试动态库头文件
 * 
 * @copyright Copyright (c) 2021 individual
 * 
 */

#ifdef __linux__
#define EXPORT_API(...) extern "C" __attribute__((visibility("default"))) __VA_ARGS__
#elif _WIN32
#define EXPORT_API(...) extern "C" __declspec(dllexport) __VA_ARGS__ __cdecl
#endif

#ifndef _TEST_H
#define _TEST_H

#include <stdlib.h>

EXPORT_API(float) add(float a, float b);
EXPORT_API(float) sub(float a, float b);
EXPORT_API(float) mul(float a, float b);

#endif