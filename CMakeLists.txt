##
# @file CMakeLists.txt
# @brief ezctest 项目的 CMake 构建配置文件
#

cmake_minimum_required(VERSION 3.10)
project(ezctest LANGUAGES C CXX)

# 设置标准
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 编译选项
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -pedantic)
endif()

# C 版本主可执行文件
add_executable(main main.c)

# C++ 版本主可执行文件
add_executable(main_cpp main.cpp)
