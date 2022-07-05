# Cpp Call DLL

## 1.Description

Windows/Linux下C++隐式调用动态库

## 2.Environment

### 2.1 Windows 10

* Visual Studio 2017
* CMake工具（3.10+）

### 2.2 Ubuntu

* g++ 7.5.0
* CMake工具（3.10+）

## 3.Directory

```shell
  ├── bin: 生成可执行文件目录
  ├── build: CMake工具构建目录
  ├── lib: 生成库目录
  ├── cpp_call_dll: 调用动态库源码目录
  ├── generate_dll: 生成动态库目录
  └── CMakeLists.txt: 根目录CMake配置文件
```

## 4.Usage

执行以下命令，生成动态库及可执行文件。

```
mkdir build
cd build
cmake ..
VS打开工程，编译生成
```

运行测试用例

打开终端，执行以下命令，即可运行databus所有测试用例

```shell
PS E:\cpp_dll> .\bin\Release\dynamic_loader.exe -h
usage: E:\cpp_dll\bin\Release\dynamic_loader.exe [-h|--help] <dll-path>
        -h|--help    - Show this help
        dll-path     - Path to dynamic library file
```

```shell
PS E:\cpp_dll> .\bin\Release\dynamic_loader.exe .\lib\Release\generate_dll.dll
sub(1.0, 2.0) = -1
is open: 0        
is open: 1        
add(1.0, 3.0) = 4 
```

**注意**：

demo所示可执行文件必须输入参数。

