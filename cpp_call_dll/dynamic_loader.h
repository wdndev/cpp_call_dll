/**
 * @file        - dynamic_loader.h
 * @author      - dongnian.wang
 * @brief       - Linux/Windows动态库隐式加载头文件
 * 
 * @copyright Copyright (c) 2021 individual
 */

#ifndef DYNAMIC_LOADER_H
#define DYNAMIC_LOADER_H

#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <functional>
#include <exception>
#include <stdexcept>

// 系统头文件和一些宏
#ifdef __linux__
#include <dlfcn.h>
#define LIBRARY_HANDLE void*
#define LIBRARY_FUNCTION void*
#define _FILE strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__
#elif _WIN32
#include <windows.h>
#include <libloaderapi.h>
#define LIBRARY_HANDLE HMODULE
#define LIBRARY_FUNCTION FARPROC
#define _FILE strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__
#endif

/**
 * 异常处理类
*/
class DynamicLoaderException : public std::runtime_error
{
public:
    DynamicLoaderException (const std::string& err_msg, const std::string& func, const int line) 
        : std::runtime_error(std::string("[ERROR] [" + func +"] [" + std::to_string(line)  +"] : "+ err_msg)), 
        m_err_msg(err_msg), m_func(func), m_line(line) {}
    DynamicLoaderException (const char* err_msg, const std::string& func, const int line)
        : std::runtime_error(std::string("[ERROR] [" + func + "] [" + std::to_string(line) + "] : " + std::string(err_msg))),
        m_err_msg(err_msg), m_func(func), m_line(line) {}

    std::string get_err_msg() const
    {
        // return std::string("[ERROR] [" + m_func + "] [" + std::to_string(m_line) + "] : " + m_err_msg);
        return m_err_msg;
    }

    std::string get_err_func() const
    {
        return m_func;
    }

    int get_err_line() const
    {
        return m_line;
    }
private:
    std::string m_err_msg;
    std::string m_func;
    int m_line;
}; // DynamicLoaderException

/**
 * Linux/Windows动态库隐式加载类
 * 
 * \note: 注意使用异常捕获，此类中大量使用异常，抛出错误
 * 
 * eg：
 *      try {
 *          DynamicLoader lib(dll_name, DynamicLoader::Mode::DELAYED_RESOLVE);
 *          auto f = lib.get_function<float(float, float)>("sub");
 *          std::cout << f(1.0, 2.0) << std::endl;
 *          lib.unload_dynamic_library();
 *      }
 *      catch (const DynamicLoaderException &e) {
 *          std::cerr << "Unhandled close error: " << e.what() << std::endl;
 *      }
*/
class DynamicLoader
{
public:
    /**
     * 加载模式
     * 
     * \note：此参数只应用于Linux加载，Windows加载无用，设置与否都可以
     * 
     * @param DELAYED_RESOLVE   - 暂缓加载，等有需要时再解出符号
     * @param RESOLVE_NOW       - 立即加载，返回前解除所有未定义的符号。 
     * @param GLOBAL_RESOLVE    - 允许导出符号
    */
    enum Mode {
#ifdef __linux__
        DELAYED_RESOLVE = RTLD_LAZY,
        RESOLVE_NOW = RTLD_NOW,
        GLOBAL_RESOLVE = RTLD_GLOBAL,
#elif _WIN32
        DELAYED_RESOLVE = 0,
        RESOLVE_NOW = 0,
        GLOBAL_RESOLVE = 0,
#endif
    };

    /**
     * 默认构造函数，不加载任何动态库
    */
    DynamicLoader();

    /**
     * 构造函数
     * 
     * @param dynamic_library_name  - 动态库文件路径，后缀为 @c dll 或 @c so
     * @param mode                  - 加载模式，详见 Mode
     * 
     * @return
    */
    DynamicLoader(const std::string& dynamic_library_name, int mode = Mode::DELAYED_RESOLVE);
    
    /**
     * 析构函数
    */
    ~DynamicLoader();

    /**
     * 加载动态库
     * 
     * \note：只能加载一次，若想重新加载，请释放后加载
     * 
     * @param dynamic_library_name  - 动态库文件路径，后缀为 @c dll 或 @c so
     * @param mode                  - 加载模式，详见 Mode
     * 
     * @return @c void        
    */
    void load_dynamic_library(const std::string& dynamic_library_name, int mode = Mode::DELAYED_RESOLVE);
    
    /**
     * 卸载动态库
     * 
     * \note：只能卸载一次，多次卸载无效
    */
    void unload_dynamic_library();
    
    /**
     * 获取动态库中函数
     * 
     * \note: 模板 T
     *        一个函数指针，如 ***.get_function<float(float, float)>("**")
     * eg：
     *      DynamicLoader lib(dll_name, DynamicLoader::Mode::DELAYED_RESOLVE);
     *      auto f = lib.get_function<float(float, float)>("sub");
     *      std::cout << f(1.0, 2.0) << std::endl;
     *      lib.unload_dynamic_library();
     * 
     * @param symbol    - 函数符号
     * 
     * @return @c std::function<T>
    */
    template <typename T>
    std::function<T> get_function(const std::string& symbol) const
    {
        return std::function<T>(get<T>(symbol.c_str()));
    }

    /**
     * 获取动态库中函数
     * 
     * \note: 返回值为 @c void* ， 需要强制转化为函数指针
     *  eg：
     *      typedef float(*Func)(float, float);
     *      DynamicLoader lib(dll_name, DynamicLoader::Mode::DELAYED_RESOLVE);
     *      Func f = (Func)lib.get_function("mul");
     *      std::cout << f(1.0, 2.0) << std::endl;
     *      lib.unload_dynamic_library();
     * 
     * @param symbol    - 函数符号
     * 
     * @return @c void*
    */
    void* get_function(const std::string& symbol) const
    {
        return (void*)get_symbol(symbol.c_str());
    }

    /**
     * 获取动态库中符号的值
     * 
     * @param symbol    - 函数符号
     * 
     * @return @c T
    */
    template <typename T>
    T& get(const std::string& symbol) const
    {
        auto sym = reinterpret_cast<T *>(get_symbol(symbol.c_str()));
        if (sym == nullptr) {
            throw DynamicLoaderException(get_error(), _FILE, __LINE__);
        }
        return *sym;
    }

    /**
     * 判断动态库是否打开
     * 
     * @return @c bool  - true 打开 ； false 未打开
    */
    bool is_open() const;

    /**
     * 获取动态库名称
     * 
     * @return @c std::string
    */
    std::string get_library_name() const;
private:
    /**
     * 获取错误
    */
    std::string get_error() const;

    /**
     * 动态库名称
    */
    std::string m_library_name;

    /**
     * 动态库句柄
    */
    LIBRARY_HANDLE m_dll_handle;

    /**
     * 获取动态库内部符号值
    */
    LIBRARY_FUNCTION get_symbol(const char* symbol) const;

    /**
     * 辅助函数：判断文件是否存在
    */
    bool is_file_exist(const std::string file_name);

    /**
     * 辅助函数：加载动态库
    */
    bool get_library_handle(const std::string& dynamic_library_name, int mode, LIBRARY_HANDLE* handle);

    /**
     * 辅助函数：卸载动态库
    */
    bool close_library_handle(LIBRARY_HANDLE handle);

};  // DynamicLoader

#endif // DYNAMIC_LOADER_H