##
# @file Makefile
# @brief ezctest 项目的 GNU Makefile 构建文件
#

# 编译器配置
CC := gcc
CXX := g++
CFLAGS := -Wall -Wextra -std=c99 -pedantic
CXXFLAGS := -Wall -Wextra -std=c++98 -pedantic

# 目标可执行文件名
ifeq ($(OS),Windows_NT)
    TARGET := main.exe
    TARGET_CPP := main_cpp.exe
    RM := del /Q
else
    TARGET := main
    TARGET_CPP := main_cpp
    RM := rm -f
endif

# 默认目标
all: $(TARGET) $(TARGET_CPP)

# 链接生成 C 可执行文件
$(TARGET): main.c ezctest.h
	$(CC) $(CFLAGS) main.c -o $(TARGET)

# 链接生成 C++ 可执行文件
$(TARGET_CPP): main.cpp ezctest.h
	$(CXX) $(CXXFLAGS) main.cpp -o $(TARGET_CPP)

# 清理构建产物
clean:
	$(RM) $(TARGET) $(TARGET_CPP)

.PHONY: all clean
