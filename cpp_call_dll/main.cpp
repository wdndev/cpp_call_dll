/**
 * @file        - main.cpp
 * @author      - dongnian.wang
 * @brief       - 动态库调用测试文件
 * 
 * @copyright Copyright (c) 2021 individual
 */

#include "dynamic_loader.h"
#include <iostream>

typedef float(*Func)(float, float);

void usage(const char* name) 
{
    fprintf(stderr, "usage: %s [-h|--help] <dll-path>\n", name);
    fprintf(stderr, "\t-h|--help    - Show this help\n");
    fprintf(stderr, "\tdll-path     - Path to dynamic library file\n");
}

int main(int argc, char* argv[])
{
    if(argc == 1 || strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) 
    {
       usage(argv[0]);
       return 0;
    }
    std::string dll_name = argv[1];
    // std::string dll_name = "E:/15MedicalRobots/e_data/DLL/cpp_dll/lib/Release/generate_dll.dll";
    try {
        // 实例化类，装载动态库
        DynamicLoader lib(dll_name, DynamicLoader::Mode::DELAYED_RESOLVE);
        // 加载函数
        auto f = lib.get_function<float(float, float)>("sub");
        // Func f = (Func)lib.get_function("mul");
        // 调用函数
        std::cout << "sub(1.0, 2.0) = "<< f(1.0, 2.0) << std::endl;
        // 卸载动态库
        lib.unload_dynamic_library();
        // 是否打开
        std::cout << "is open: " << lib.is_open() << std::endl;
        // 装载动态库
        lib.load_dynamic_library(dll_name, DynamicLoader::Mode::DELAYED_RESOLVE);
        // 是否打开
        std::cout << "is open: " << lib.is_open() << std::endl;
        // 加载函数
        auto fun = lib.get_function<float(float, float)>("add");
        // 调用函数
        std::cout << "add(1.0, 3.0) = " << fun(1.0, 3.0) << std::endl;
        // 卸载动态库
        lib.unload_dynamic_library();
    }
    catch (const DynamicLoaderException& e) {
        std::cerr << e.what() << std::endl;
        std::cerr << e.get_err_func() << std::endl;
    }
    catch (...) {
        std::cout << "22222222222222222222222" << std::endl;
    }
    return 0;
}