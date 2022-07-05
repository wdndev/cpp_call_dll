/**
 * @file        - dynamic_loader.cpp
 * @author      - dongnian.wang
 * @brief       - Linux/Windows动态库隐式加载实现
 * 
 * @copyright Copyright (c) 2021 individual
 */

#include "dynamic_loader.h"

/**
 * 辅助函数：分割字符串函数
*/
inline std::string split_str(const std::string& str, const int ch)
{
    return std::string(strrchr(str.c_str(), ch) ? strrchr(str.c_str(), ch) + 1 : str.c_str());
}

/**
 * 默认构造函数
*/
DynamicLoader::DynamicLoader()
    : m_dll_handle(nullptr)
{
}

/**
 * 构造函数
*/
DynamicLoader::DynamicLoader(const std::string & dynamic_library_name, int mode)
    : m_dll_handle(nullptr)
{
    load_dynamic_library(dynamic_library_name, mode);
}

/**
 * 析构函数
*/
DynamicLoader::~DynamicLoader()
{
    try {
        if(m_dll_handle != nullptr)
            unload_dynamic_library();
    }
    catch (const std::invalid_argument &e) {
        std::cerr << "Unhandled close error: " << e.what() << std::endl;
    }
}

/**
 * 加载动态库
*/
void DynamicLoader::load_dynamic_library(const std::string& dynamic_library_name, int mode)
{
    if(m_dll_handle != nullptr)
    {
        std::cerr << "Dynamic library has been loaded, if you reload, please release!" << std::endl;
        return;
    }
    if (!get_library_handle(dynamic_library_name, mode, &m_dll_handle))
        throw DynamicLoaderException(get_error(), _FILE, __LINE__);
    m_library_name = dynamic_library_name;
}

/**
 * 卸载动态库
*/
void DynamicLoader::unload_dynamic_library()
{
    if(m_dll_handle == nullptr)
    {
        std::cerr << "Dynamic library has been unloaded, without repeated unloading!" << std::endl;
        return;
    }
    if(m_dll_handle != nullptr && !close_library_handle(m_dll_handle))
        throw DynamicLoaderException(get_error(), _FILE, __LINE__);

    m_dll_handle = nullptr;
    m_library_name.clear();
}

/**
 * 判断动态库是否打开
*/
bool DynamicLoader::is_open() const
{
    return m_dll_handle != nullptr;
}

/**
 * 获取动态库名称
*/
std::string DynamicLoader::get_library_name() const
{
    return m_library_name;
}

/**
 * 获取错误
*/
std::string DynamicLoader::get_error() const
{
#ifdef __linux__
    return dlerror();
#elif _WIN32
    DWORD id = ::GetLastError();
    if (id == 0) {
        return "SUCCESS";
    }
    LPSTR buffer;
    size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, id, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&buffer, 0, NULL);
    std::string msg(buffer, size);
    LocalFree(buffer);
    return msg;
#endif
}

/**
 * 获取动态库内部符号值
*/
LIBRARY_FUNCTION DynamicLoader::get_symbol(const char* symbol) const
{
    if (m_dll_handle != nullptr)
    {
        LIBRARY_FUNCTION temp_function_ptr = NULL;
#ifdef __linux
        temp_function_ptr = dlsym(m_dll_handle, symbol);
#elif _WIN32
        temp_function_ptr = GetProcAddress(m_dll_handle, symbol);
#endif
        return temp_function_ptr;
    }
    return NULL;
}

/**
 * 辅助函数：判断文件是否存在
*/
bool DynamicLoader::is_file_exist(const std::string file_name)
{
    std::fstream file;
    file.open(file_name, std::ios::in);
    if (!file)
    {
        std::string tmp = split_str(file_name, '/');
        throw DynamicLoaderException(std::string("Fail to open '" + tmp + "' file!!!"), _FILE, __LINE__);
    }
    file.close();

#ifdef __linux__
    if (split_str(file_name, '.') != "so")
#elif _WIN32
    if (split_str(file_name, '.') != "dll")
#endif
    {
        std::string tmp = split_str(file_name, '/');
        throw DynamicLoaderException(std::string("The file '" + tmp + "' is NOT dynamic library!!!"), _FILE, __LINE__);
    }

    return true;
}

/**
 * 辅助函数：加载动态库
*/
bool DynamicLoader::get_library_handle(const std::string& dynamic_library_name, int mode, LIBRARY_HANDLE* handle)
{
    if (is_file_exist(dynamic_library_name))
    {
#ifdef __linux__
        *handle = dlopen(dynamic_library_name.c_str(), mode);
#elif _WIN32
        *handle = LoadLibraryA(dynamic_library_name.c_str());
#endif
        if (*handle == nullptr)
        {
            return false;
        }
        return true;
    }
    else
        return false;
}

/**
 * 辅助函数：卸载动态库
*/
bool DynamicLoader::close_library_handle(LIBRARY_HANDLE handle)
{
#ifdef __linux__
    if (dlclose(handle) != 0)
        return false;
#elif _WIN32
    if (FreeLibrary(handle) == 0)
        return false;
#endif
    return true;
}
