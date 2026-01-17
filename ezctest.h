/**
 * @file ezctest.h
 * @brief 轻量级C99单元测试框架 - 类似Google Test的单头文件实现
 * @version 3.9.0
 * @date 2026-01-04
 *
 * @copyright Copyright (c) 2025 wanglanchun
 *
 * @license MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 *
 * @details
 * 这是一个完全兼容C99的单元测试框架，设计用于跨平台使用（Windows/Linux/STM32）。
 *
 * 支持的编译器：
 * - MSVC 6.0及以上（VC6 使用 Magic Number + 内存扫描实现自动注册）
 * - GCC 4.5及以上
 * - MinGW
 * - ARM GCC (STM32)
 *
 * 主要特性：
 * - 单头文件，易于集成
 * - 类似Google Test的使用方式
 * - ASSERT和EXPECT断言宏
 * - 浮点数比较支持（FLOAT_EQ, DOUBLE_EQ, NEAR）
 * - 命令行参数支持（--filter, --repeat等）
 * - 通配符匹配测试用例
 * - 彩色输出（支持Windows和Linux）
 * - STM32支持（普通模式和交互式模式）
 * - 轻量级，适合嵌入式环境
 *
 * 使用示例：
 * @code
 * #define EZCTEST_IMPLEMENTATION
 * #include "ezctest.h"
 *
 * TEST(TestSuite, TestCase) {
 *     EXPECT_EQ(1, 1);
 *     ASSERT_TRUE(1 == 1);
 * }
 *
 * int main(int argc, char* argv[]) {
 *     return RUN_ALL_TESTS(argc, argv);
 * }
 * @endcode
 *
 * VC6 特殊说明：
 * VC6 由于不支持在宏中使用 #pragma，本框架采用创新的 Magic Number +
 * 内存扫描机制。 TEST
 * 宏会生成包含唯一标识符的元数据结构，运行时自动扫描内存发现所有测试。
 * 用户无需任何特殊操作，完全自动注册！
 *
 * STM32使用示例：
 * @code
 * // 定义STM32模式
 * #define EZCTEST_STM32_MODE
 * #define EZCTEST_IMPLEMENTATION
 * #include "ezctest.h"
 *
 * int main(void) {
 *     // 初始化UART等...
 *
 *     #ifdef EZCTEST_STM32_INTERACTIVE
 *     // 交互式模式
 *     RUN_TESTS_INTERACTIVE();
 *     #else
 *     // 自动运行所有测试
 *     RUN_ALL_TESTS(0, NULL);
 *     #endif
 * }
 * @endcode
 *
 * @note STM32平台需要实现printf重定向到UART
 * @warning 在嵌入式平台上注意内存使用
 */

#ifndef EZCTEST_H
#define EZCTEST_H

/* POSIX 特性测试宏（用于 fileno 等函数，必须在所有头文件之前定义） */
#if !defined(_WIN32) && !defined(_POSIX_C_SOURCE)
#define _POSIX_C_SOURCE 200809L
#endif

/* VC6 特殊支持：使用 Magic Number + 内存扫描实现自动注册 */

/* 检测GCC编译器及版本 */
#if defined(__GNUC__) && !defined(__clang__)
#define EZCTEST_COMPILER_GCC
/* GCC版本计算: __GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__ */
#define EZCTEST_GCC_VERSION                                                    \
  (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
/* GCC 3.3之前不支持 __attribute__((constructor))，需要使用内存扫描机制 */
#if EZCTEST_GCC_VERSION < 30300
#define EZCTEST_GCC_NO_CONSTRUCTOR
#endif
#endif

/* ============================================================================
 * 警告控制宏
 * ========================================================================== */

/* 禁用"未使用函数"警告的宏 */
#if defined(_MSC_VER) && _MSC_VER >= 1300
/* MSVC 7.0+: 支持 __pragma */
#define EZCTEST_DISABLE_UNUSED_FUNCTION_BEGIN                                  \
  __pragma(warning(push)) __pragma(warning(disable : 4505))
#define EZCTEST_DISABLE_UNUSED_FUNCTION_END __pragma(warning(pop))

#elif defined(__GNUC__) || defined(__clang__)
/* GCC/Clang: -Wunused-function */
#define EZCTEST_DISABLE_UNUSED_FUNCTION_BEGIN                                  \
  _Pragma("GCC diagnostic push")                                               \
      _Pragma("GCC diagnostic ignored \"-Wunused-function\"")
#define EZCTEST_DISABLE_UNUSED_FUNCTION_END _Pragma("GCC diagnostic pop")

#else
/* VC6 或其他编译器：不做处理 */
#define EZCTEST_DISABLE_UNUSED_FUNCTION_BEGIN
#define EZCTEST_DISABLE_UNUSED_FUNCTION_END
#endif

EZCTEST_DISABLE_UNUSED_FUNCTION_BEGIN

/* ============================================================================
 * 标准库包含
 * ========================================================================== */

/* ============================================================================
 * 编译器和平台检测
 * ========================================================================== */

/* 检测STM32平台 */
#if defined(STM32) || defined(STM32F0) || defined(STM32F1) ||                  \
    defined(STM32F2) || defined(STM32F3) || defined(STM32F4) ||                \
    defined(STM32F7) || defined(STM32H7) || defined(STM32L0) ||                \
    defined(STM32L1) || defined(STM32L4) || defined(STM32L5) ||                \
    defined(STM32G0) || defined(STM32G4) || defined(STM32WB) ||                \
    defined(STM32MP1)
#ifndef EZCTEST_STM32_MODE
#define EZCTEST_STM32_MODE
#endif
#endif

/* 检测Windows平台 */
#if defined(_WIN32) || defined(_WIN64) || defined(__WIN32__) ||                \
    defined(__WINDOWS__)
#define EZCTEST_PLATFORM_WINDOWS
#endif

/* 检测Linux平台 */
#if defined(__linux__) || defined(__unix__) || defined(__APPLE__)
#define EZCTEST_PLATFORM_LINUX
#endif

/* 检测MSVC编译器 */
#if defined(_MSC_VER)
#define EZCTEST_COMPILER_MSVC
/* MSVC不支持__FUNCTION__宏在旧版本中，使用__FUNCSIG__或替代方案 */
#if _MSC_VER < 1300 /* VC6 */
#define EZCTEST_FUNCTION_NAME __FILE__
#else
#define EZCTEST_FUNCTION_NAME __FUNCTION__
#endif

/* MSVC兼容性：snprintf */
#if _MSC_VER >= 1400 && _MSC_VER < 1900 /* VC8-VC14: 使用安全版本 */
/* VC8-VC14：使用 _snprintf_s 消除 C4996 警告 */
#define snprintf(buf, size, fmt, ...)                                          \
  _snprintf_s(buf, size, _TRUNCATE, fmt, ##__VA_ARGS__)
#elif _MSC_VER < 1400 /* VC6-VC7.1: 使用 _snprintf */
#define snprintf _snprintf
#endif
/* VS2015+ 已内置标准 snprintf */

/* vsnprintf 在 VC9+ 已经存在，只为 VC6-VC8 定义 */
#if _MSC_VER < 1500 /* VC9之前 */
#define vsnprintf _vsnprintf
#endif
#else
#define EZCTEST_FUNCTION_NAME __func__
#endif

/* 包含必要的标准头文件 */
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* 对于非STM32平台，包含更多系统头文件 */
#ifndef EZCTEST_STM32_MODE
#include <math.h>
#endif

/* Windows特定头文件 */
#ifdef EZCTEST_PLATFORM_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <io.h>
#include <windows.h>

/* VC7-VC9兼容性：显式声明IsDebuggerPresent */
#if defined(_MSC_VER) && _MSC_VER >= 1300 && _MSC_VER < 1600
#ifdef __cplusplus
extern "C" {
#endif
WINBASEAPI BOOL WINAPI IsDebuggerPresent(VOID);
#ifdef __cplusplus
}
#endif
#endif

#ifndef EZCTEST_COMPILER_MSVC
#include <unistd.h>
#endif

/* Windows异常码定义（确保所有编译模式都可用） */
#ifndef EXCEPTION_ACCESS_VIOLATION
#define EXCEPTION_ACCESS_VIOLATION 0xC0000005L
#endif
#ifndef EXCEPTION_INT_DIVIDE_BY_ZERO
#define EXCEPTION_INT_DIVIDE_BY_ZERO 0xC0000094L
#endif
#ifndef EXCEPTION_ARRAY_BOUNDS_EXCEEDED
#define EXCEPTION_ARRAY_BOUNDS_EXCEEDED 0xC000008CL
#endif
#ifndef EXCEPTION_STACK_OVERFLOW
#define EXCEPTION_STACK_OVERFLOW 0xC00000FDL
#endif
#ifndef EXCEPTION_ILLEGAL_INSTRUCTION
#define EXCEPTION_ILLEGAL_INSTRUCTION 0xC000001DL
#endif
#ifndef EXCEPTION_FLT_DIVIDE_BY_ZERO
#define EXCEPTION_FLT_DIVIDE_BY_ZERO 0xC000008EL
#endif
#ifndef EXCEPTION_INT_OVERFLOW
#define EXCEPTION_INT_OVERFLOW 0xC0000095L
#endif

#else
/* Linux/Unix特定头文件 */
#ifndef EZCTEST_STM32_MODE
#include <signal.h>
#include <sys/time.h>
#include <unistd.h>

#endif
#endif

/* C++标准库（用于异常处理和值格式化） */
#ifdef __cplusplus
#include <exception>
#include <sstream>
#include <stdexcept>
#include <string>
#endif

/* ============================================================================
 * C标准版本检测
 * ========================================================================== */

/**
 * @brief 检测C11 _Generic支持
 */
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L &&                \
    !defined(EZCTEST_NO_GENERIC)
#define EZCTEST_HAS_GENERIC 1
#endif

/* ============================================================================
 * 配置选项
 * ========================================================================== */

/* 最大测试用例数量 */
#ifndef EZCTEST_MAX_TESTS
#ifdef EZCTEST_STM32_MODE
#define EZCTEST_MAX_TESTS 128
#else
#define EZCTEST_MAX_TESTS 1024
#endif
#endif

/* 最大测试名称长度 */
#ifndef EZCTEST_MAX_NAME_LENGTH
#define EZCTEST_MAX_NAME_LENGTH 128
#endif

/* 最大失败消息长度 */
#ifndef EZCTEST_MAX_MESSAGE_LENGTH
#ifdef EZCTEST_STM32_MODE
#define EZCTEST_MAX_MESSAGE_LENGTH 256
#else
#define EZCTEST_MAX_MESSAGE_LENGTH 512
#endif
#endif

/* STM32交互式命令行缓冲区大小 */
#ifndef EZCTEST_STM32_CMD_BUFFER_SIZE
#define EZCTEST_STM32_CMD_BUFFER_SIZE 128
#endif

/* DEFER清理回调栈大小 */
#ifndef EZCTEST_MAX_DEFER_CALLBACKS
#ifdef EZCTEST_STM32_MODE
#define EZCTEST_MAX_DEFER_CALLBACKS 8
#else
#define EZCTEST_MAX_DEFER_CALLBACKS 32
#endif
#endif

/* ============================================================================
 * 颜色输出支持
 * ========================================================================== */

typedef enum {
  EZCTEST_COLOR_DEFAULT,
  EZCTEST_COLOR_RED,
  EZCTEST_COLOR_GREEN,
  EZCTEST_COLOR_YELLOW,
  EZCTEST_COLOR_BLUE,
  EZCTEST_COLOR_MAGENTA,
  EZCTEST_COLOR_CYAN,
  EZCTEST_COLOR_WHITE
} ezctest_color_t;

/* 对外可见/实现控制：
 * - 在一个 .c 文件中定义 EZCTEST_IMPLEMENTATION 再包含本头文件，生成实现
 * - 其他只包含本头文件的单元只看到声明和宏
 */
#ifdef EZCTEST_IMPLEMENTATION
#define EZCTEST_API
#else
#define EZCTEST_API extern
#endif

/* ============================================================================
 * 测试结果统计
 * ========================================================================== */

typedef struct {
  int total_tests;       /* 总测试数 */
  int passed_tests;      /* 通过的测试数 */
  int failed_tests;      /* 失败的测试数 */
  int total_assertions;  /* 总断言数 */
  int failed_assertions; /* 失败的断言数 */
} ezctest_result_t;

/* ============================================================================
 * 测试用例定义
 * ========================================================================== */

typedef void (*ezctest_func_t)(void);

typedef struct {
  const char *suite_name;   /* 测试套件名称 */
  const char *test_name;    /* 测试用例名称 */
  ezctest_func_t test_func; /* 测试函数指针 */
  int enabled;              /* 是否启用 */
  int failed;               /* 本轮测试是否失败 */
} ezctest_info_t;

/* ============================================================================
 * VC6 特殊支持：Magic Number + 内存扫描（完全不使用 pragma）
 * ========================================================================== */

#if defined(_MSC_VER)

/* Magic number: "CTST" = 0x54535443 */
#define EZCTEST_MAGIC 0x54535443
/* Fixture magic number: "FXTR" = 0x52545846 */
#define EZCTEST_FIXTURE_MAGIC 0x52545846

/* 测试元数据结构（包含 magic number） */
typedef struct {
  unsigned int magic;
  ezctest_info_t info;
} ezctest_metadata_t;

#endif

/* 测试配置类型 */
typedef struct {
  const char *filter; /* 测试过滤器 */
  int repeat;         /* 重复次数 */
  int shuffle;        /* 是否随机顺序 */
  int color;          /* 彩色输出 */
  int list_tests;     /* 仅列出测试 */
  int no_exec;        /* 禁用多进程隔离 (-1=自动, 0=启用, 1=禁用) */
} ezctest_config_t;

/* Worker模式支持 - 声明在后面的全局变量块中 */

/* ============================================================================
 * Setup/Teardown 和 DEFER 清理机制
 * ========================================================================== */

/* 清理函数类型 */
typedef void (*ezctest_cleanup_func_t)(void *data);

/* Setup函数类型 */
typedef void (*ezctest_setup_func_t)(void);

/* Teardown函数类型 */
typedef void (*ezctest_teardown_func_t)(void);

#if defined(_MSC_VER)
/* Fixture元数据结构（包含 magic number） */
typedef struct {
  unsigned int magic;
  const char *suite_name;
  ezctest_setup_func_t setup;
  ezctest_teardown_func_t teardown;
  int is_setup; /* 1=setup, 0=teardown */
} ezctest_fixture_metadata_t;
#endif

/* DEFER清理回调栈 */
typedef struct {
  ezctest_cleanup_func_t callbacks[EZCTEST_MAX_DEFER_CALLBACKS];
  void *data[EZCTEST_MAX_DEFER_CALLBACKS];
  int count;
} ezctest_defer_stack_t;

/* Setup/Teardown注册表 */
typedef struct {
  const char *suite_name;
  ezctest_setup_func_t setup;
  ezctest_teardown_func_t teardown;
} ezctest_fixture_t;

#ifndef EZCTEST_MAX_FIXTURES
#define EZCTEST_MAX_FIXTURES 64
#endif

/* longjmp支持 */
#include <setjmp.h>

typedef struct {
  jmp_buf jmp_env;
  int has_jumped;
} ezctest_longjmp_context_t;

/* 全局变量将在后面的全局变量块中声明/定义 */

/* ============================================================================
 * 测试注册
 * ========================================================================== */

/**
 * @brief 注册一个测试用例
 * @param suite_name 测试套件名称
 * @param test_name 测试用例名称
 * @param test_func 测试函数指针
 * @return 注册成功返回1，失败返回0
 */
EZCTEST_API int ezctest_register(const char *suite_name, const char *test_name,
                                 ezctest_func_t test_func);

/**
 * @brief 注册测试套件的Setup函数
 * @param suite_name 测试套件名称
 * @param setup Setup函数指针
 * @return 注册成功返回1，失败返回0
 */
EZCTEST_API int ezctest_register_setup(const char *suite_name,
                                       ezctest_setup_func_t setup);

/**
 * @brief 注册测试套件的Teardown函数
 * @param suite_name 测试套件名称
 * @param teardown Teardown函数指针
 * @return 注册成功返回1，失败返回0
 */
EZCTEST_API int ezctest_register_teardown(const char *suite_name,
                                          ezctest_teardown_func_t teardown);

/**
 * @brief 添加DEFER清理回调
 * @param func 清理函数指针
 * @param data 传递给清理函数的数据
 * @return 添加成功返回1，失败返回0
 */
EZCTEST_API int ezctest_defer_add(ezctest_cleanup_func_t func, void *data);

/**
 * @brief 执行所有DEFER清理回调（LIFO顺序）
 */
EZCTEST_API void ezctest_defer_execute(void);

/**
 * @brief 清空DEFER栈
 */
EZCTEST_API void ezctest_defer_clear(void);

/* ============================================================================
 * 全局变量声明与定义
 * ========================================================================== */

#ifdef EZCTEST_IMPLEMENTATION

/* 在实现单元中定义全局变量 */
ezctest_info_t g_ezctest_registry[EZCTEST_MAX_TESTS];
int g_ezctest_count = 0;
int g_ezctest_current_failed = 0;
int g_ezctest_current_assertion_failed = 0;
ezctest_result_t g_ezctest_result = {0, 0, 0, 0, 0};
ezctest_config_t g_ezctest_config = {NULL, 1, 0,
                                     -1,   0, -1}; /* 增加no_exec=-1 */
int g_ezctest_color_enabled = -1;
ezctest_fixture_t g_ezctest_fixtures[EZCTEST_MAX_FIXTURES];
int g_ezctest_fixture_count = 0;
ezctest_defer_stack_t g_ezctest_defer_stack = {{0}, {0}, 0};
/* jmp_buf 初始化：使用 memset 在运行时初始化以避免编译警告 */
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-braces"
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif
ezctest_longjmp_context_t g_ezctest_longjmp_ctx = {{0}, 0};
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif
int g_ezctest_worker_index = -1; /* 主进程 */
char *g_ezctest_argv0 = NULL;

#if defined(_MSC_VER)
int g_ezctest_scanned = 0;
#endif

#else

/* 在其他单元中声明全局变量为 extern */
extern ezctest_info_t g_ezctest_registry[EZCTEST_MAX_TESTS];
extern int g_ezctest_count;
extern int g_ezctest_current_failed;
extern int g_ezctest_current_assertion_failed;
extern ezctest_result_t g_ezctest_result;
extern ezctest_config_t g_ezctest_config;
extern int g_ezctest_color_enabled;
extern ezctest_fixture_t g_ezctest_fixtures[EZCTEST_MAX_FIXTURES];
extern int g_ezctest_fixture_count;
extern ezctest_defer_stack_t g_ezctest_defer_stack;
extern ezctest_longjmp_context_t g_ezctest_longjmp_ctx;
extern int g_ezctest_worker_index;
extern char *g_ezctest_argv0;

#if defined(_MSC_VER)
extern int g_ezctest_scanned;
#endif

#endif /* EZCTEST_IMPLEMENTATION */

/* ============================================================================
 * 函数实现
 * ========================================================================== */

#ifdef EZCTEST_IMPLEMENTATION

int ezctest_register(const char *suite_name, const char *test_name,
                     ezctest_func_t test_func) {
  if (g_ezctest_count >= EZCTEST_MAX_TESTS) {
    fprintf(stderr, "Error: Maximum number of tests (%d) exceeded\n",
            EZCTEST_MAX_TESTS);
    return 0;
  }

  g_ezctest_registry[g_ezctest_count].suite_name = suite_name;
  g_ezctest_registry[g_ezctest_count].test_name = test_name;
  g_ezctest_registry[g_ezctest_count].test_func = test_func;
  g_ezctest_registry[g_ezctest_count].enabled = 1;
  g_ezctest_count++;

  return 1;
}

int ezctest_register_setup(const char *suite_name, ezctest_setup_func_t setup) {
  int i;

  /* 查找是否已存在该suite */
  for (i = 0; i < g_ezctest_fixture_count; i++) {
    if (strcmp(g_ezctest_fixtures[i].suite_name, suite_name) == 0) {
      g_ezctest_fixtures[i].setup = setup;
      return 1;
    }
  }

  /* 新建fixture */
  if (g_ezctest_fixture_count >= EZCTEST_MAX_FIXTURES) {
    fprintf(stderr, "Error: Maximum number of fixtures (%d) exceeded\n",
            EZCTEST_MAX_FIXTURES);
    return 0;
  }

  g_ezctest_fixtures[g_ezctest_fixture_count].suite_name = suite_name;
  g_ezctest_fixtures[g_ezctest_fixture_count].setup = setup;
  g_ezctest_fixtures[g_ezctest_fixture_count].teardown = NULL;
  g_ezctest_fixture_count++;

  return 1;
}

int ezctest_register_teardown(const char *suite_name,
                              ezctest_teardown_func_t teardown) {
  int i;

  /* 查找是否已存在该suite */
  for (i = 0; i < g_ezctest_fixture_count; i++) {
    if (strcmp(g_ezctest_fixtures[i].suite_name, suite_name) == 0) {
      g_ezctest_fixtures[i].teardown = teardown;
      return 1;
    }
  }

  /* 新建fixture */
  if (g_ezctest_fixture_count >= EZCTEST_MAX_FIXTURES) {
    fprintf(stderr, "Error: Maximum number of fixtures (%d) exceeded\n",
            EZCTEST_MAX_FIXTURES);
    return 0;
  }

  g_ezctest_fixtures[g_ezctest_fixture_count].suite_name = suite_name;
  g_ezctest_fixtures[g_ezctest_fixture_count].setup = NULL;
  g_ezctest_fixtures[g_ezctest_fixture_count].teardown = teardown;
  g_ezctest_fixture_count++;

  return 1;
}

int ezctest_defer_add(ezctest_cleanup_func_t func, void *data) {
  if (g_ezctest_defer_stack.count >= EZCTEST_MAX_DEFER_CALLBACKS) {
    fprintf(stderr, "Error: DEFER stack full (max %d)\n",
            EZCTEST_MAX_DEFER_CALLBACKS);
    return 0;
  }

  g_ezctest_defer_stack.callbacks[g_ezctest_defer_stack.count] = func;
  g_ezctest_defer_stack.data[g_ezctest_defer_stack.count] = data;
  g_ezctest_defer_stack.count++;

  return 1;
}

void ezctest_defer_execute(void) {
  int i;

  /* LIFO顺序执行清理回调 */
  for (i = g_ezctest_defer_stack.count - 1; i >= 0; i--) {
    if (g_ezctest_defer_stack.callbacks[i]) {
      g_ezctest_defer_stack.callbacks[i](g_ezctest_defer_stack.data[i]);
    }
  }
}

void ezctest_defer_clear(void) { g_ezctest_defer_stack.count = 0; }

#endif /* EZCTEST_IMPLEMENTATION */

/* ============================================================================
 * 通配符匹配
 * ========================================================================== */

/**
 * @brief 通配符匹配函数（支持*和?）
 * @param pattern 匹配模式
 * @param str 待匹配字符串
 * @return 匹配返回1，不匹配返回0
 */
static int ezctest_wildcard_match(const char *pattern, const char *str) {
  const char *p = pattern;
  const char *s = str;
  const char *star_p = NULL;
  const char *star_s = NULL;

  while (*s) {
    if (*p == '*') {
      /* 记录*位置 */
      star_p = p++;
      star_s = s;
    } else if (*p == '?' || *p == *s) {
      /* ?匹配任意字符，或者字符相同 */
      p++;
      s++;
    } else if (star_p) {
      /* 回溯到上一个* */
      p = star_p + 1;
      s = ++star_s;
    } else {
      return 0;
    }
  }

  /* 处理剩余的* */
  while (*p == '*') {
    p++;
  }

  return *p == '\0';
}

/**
 * @brief 检查测试是否匹配过滤器
 * @param suite_name 测试套件名称
 * @param test_name 测试用例名称
 * @param filter 过滤器字符串
 * @return 匹配返回1，不匹配返回0
 */
static int ezctest_matches_filter(const char *suite_name, const char *test_name,
                                  const char *filter) {
  char full_name[EZCTEST_MAX_NAME_LENGTH * 2];

  if (!filter || filter[0] == '\0') {
    return 1; /* 空过滤器匹配所有 */
  }

  /* 构建完整测试名称 */
  snprintf(full_name, sizeof(full_name), "%s.%s", suite_name, test_name);

  /* 支持多个过滤器，用:分隔 */
  {
    char filter_copy[EZCTEST_MAX_NAME_LENGTH * 4];
    char *token;
    char *rest;
#if defined(_MSC_VER) && _MSC_VER >= 1400 /* VC8+ 使用安全版本 */
    char *context = NULL;
#endif

#if defined(_MSC_VER) && _MSC_VER >= 1400
    strncpy_s(filter_copy, sizeof(filter_copy), filter, _TRUNCATE);
#else
    strncpy(filter_copy, filter, sizeof(filter_copy) - 1);
    filter_copy[sizeof(filter_copy) - 1] = '\0';
#endif

    rest = filter_copy;
#if defined(_MSC_VER) && _MSC_VER >= 1400
    token = strtok_s(rest, ":", &context);
    while (token != NULL) {
#else
    while ((token = strtok(rest, ":")) != NULL) {
      rest = NULL;
#endif

      /* 检查是否为负向过滤（-开头） */
      if (token[0] == '-') {
        if (ezctest_wildcard_match(token + 1, full_name)) {
          return 0; /* 负向匹配，排除 */
        }
      } else {
        if (ezctest_wildcard_match(token, full_name)) {
          return 1; /* 正向匹配 */
        }
      }
#if defined(_MSC_VER) && _MSC_VER >= 1400
      token = strtok_s(NULL, ":", &context);
    }
#else
      rest = NULL;
    }
#endif
  }

  return 0;
}

/* ============================================================================
 * 开始 C 链接块（在全局变量定义之后）
 * ========================================================================== */
#ifdef __cplusplus
extern "C" {
#endif

/* 函数前置声明 */
static void ezctest_set_color(ezctest_color_t color);
static void ezctest_reset_color(void);
static int ezctest_should_use_color(void);
static void ezctest_run_test(const ezctest_info_t *test);

/* ============================================================================
 * 函数实现
 * ========================================================================== */
#if defined(EZCTEST_PLATFORM_WINDOWS) &&                                       \
    (defined(_WIN32_WINNT) && _WIN32_WINNT >= 0x0A00) &&                       \
    !defined(ENABLE_VIRTUAL_TERMINAL_PROCESSING)
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif

static int ezctest_should_use_color(void) {
  if (g_ezctest_config.color == 0) {
    return 0;
  }
  if (g_ezctest_config.color == 1) {
    return 1;
  }

  /* 自动检测 */
  if (g_ezctest_color_enabled != -1) {
    return g_ezctest_color_enabled;
  }

#ifdef EZCTEST_STM32_MODE
  g_ezctest_color_enabled = 0; /* STM32默认不使用颜色 */
#elif defined(EZCTEST_PLATFORM_WINDOWS)
/* Windows 10+ 支持ANSI颜色 */
#if defined(_WIN32_WINNT) && _WIN32_WINNT >= 0x0A00
  {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    if (GetConsoleMode(hConsole, &mode)) {
      mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
      SetConsoleMode(hConsole, mode);
      g_ezctest_color_enabled = 1;
    } else {
      g_ezctest_color_enabled = 0;
    }
  }
#else
  /* 旧版Windows使用控制台API */
  g_ezctest_color_enabled = (_isatty(_fileno(stdout)) != 0);
#endif
#else
  /* Linux/Unix检查是否为终端 */
  g_ezctest_color_enabled = (isatty(fileno(stdout)) != 0);
#endif

  return g_ezctest_color_enabled;
}

static void ezctest_set_color(ezctest_color_t color) {
  if (!ezctest_should_use_color()) {
    return;
  }

#if defined(EZCTEST_PLATFORM_WINDOWS) && !defined(EZCTEST_FORCE_ANSI_COLOR)
#if defined(EZCTEST_COMPILER_MSVC) || defined(__MINGW32__)
  {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    WORD color_attr = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;

    switch (color) {
    case EZCTEST_COLOR_RED:
      color_attr = FOREGROUND_RED | FOREGROUND_INTENSITY;
      break;
    case EZCTEST_COLOR_GREEN:
      color_attr = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
      break;
    case EZCTEST_COLOR_YELLOW:
      color_attr = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
      break;
    case EZCTEST_COLOR_BLUE:
      color_attr = FOREGROUND_BLUE | FOREGROUND_INTENSITY;
      break;
    case EZCTEST_COLOR_MAGENTA:
      color_attr = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
      break;
    case EZCTEST_COLOR_CYAN:
      color_attr = FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
      break;
    case EZCTEST_COLOR_WHITE:
      color_attr = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE |
                   FOREGROUND_INTENSITY;
      break;
    default:
      break;
    }

    SetConsoleTextAttribute(hConsole, color_attr);
  }
#endif
#else
  /* ANSI转义码 */
  {
    const char *color_code = "\033[0m";

    switch (color) {
    case EZCTEST_COLOR_RED:
      color_code = "\033[0;31m";
      break;
    case EZCTEST_COLOR_GREEN:
      color_code = "\033[0;32m";
      break;
    case EZCTEST_COLOR_YELLOW:
      color_code = "\033[0;33m";
      break;
    case EZCTEST_COLOR_BLUE:
      color_code = "\033[0;34m";
      break;
    case EZCTEST_COLOR_MAGENTA:
      color_code = "\033[0;35m";
      break;
    case EZCTEST_COLOR_CYAN:
      color_code = "\033[0;36m";
      break;
    case EZCTEST_COLOR_WHITE:
      color_code = "\033[0;37m";
      break;
    default:
      break;
    }

    printf("%s", color_code);
  }
#endif
}

static void ezctest_reset_color(void) {
  if (!ezctest_should_use_color()) {
    return;
  }

#if defined(EZCTEST_PLATFORM_WINDOWS) && !defined(EZCTEST_FORCE_ANSI_COLOR)
#if defined(EZCTEST_COMPILER_MSVC) || defined(__MINGW32__)
  {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN |
                                          FOREGROUND_BLUE);
  }
#endif
#else
  printf("\033[0m");
#endif
}

/* ============================================================================
 * 彩色打印辅助函数
 * ========================================================================== */

static void ezctest_printf_colored(ezctest_color_t color, const char *format,
                                   ...) {
  va_list args;
  ezctest_set_color(color);
  va_start(args, format);
  vprintf(format, args);
  va_end(args);
  ezctest_reset_color();
}

/* ============================================================================
 * 调试器检测
 * ========================================================================== */

/* VC6 兼容性：IsDebuggerPresent 在旧版 SDK 中不存在，需要手动声明 */
#if defined(_MSC_VER) && _MSC_VER < 1300 && defined(EZCTEST_PLATFORM_WINDOWS)
/* 为 VC6 提供 IsDebuggerPresent 函数声明（来自 kernel32.dll） */
typedef int(WINAPI *IsDebuggerPresentFunc)(void);

static int ezctest_is_debugger_present_vc6(void) {
  HMODULE hKernel32;
  IsDebuggerPresentFunc pIsDebuggerPresent;

  /* 动态加载 kernel32.dll 中的 IsDebuggerPresent 函数 */
  hKernel32 = GetModuleHandleA("kernel32.dll");
  if (!hKernel32) {
    return 0; /* 无法获取 kernel32.dll 句柄 */
  }

  pIsDebuggerPresent =
      (IsDebuggerPresentFunc)GetProcAddress(hKernel32, "IsDebuggerPresent");
  if (!pIsDebuggerPresent) {
    return 0; /* IsDebuggerPresent 不可用（Windows 95/98/NT 4.0） */
  }

  return pIsDebuggerPresent() ? 1 : 0;
}
#endif

/**
 * @brief 检测程序是否在调试器下运行
 * @return 在调试器下返回1，否则返回0
 */
static int ezctest_under_debugger(void) {
#ifdef EZCTEST_STM32_MODE
  /* STM32平台不支持调试器检测 */
  return 0;
#elif defined(EZCTEST_PLATFORM_LINUX)
  /* Linux: 检查/proc/self/status中的TracerPid */
  {
    FILE *fp = fopen("/proc/self/status", "r");
    if (fp) {
      char buf[256];
      while (fgets(buf, sizeof(buf), fp)) {
        if (strncmp(buf, "TracerPid:", 10) == 0) {
          int tracer_pid = atoi(buf + 10);
          fclose(fp);
          return (tracer_pid != 0) ? 1 : 0;
        }
      }
      fclose(fp);
    }
  }
  return 0;
#elif defined(EZCTEST_PLATFORM_WINDOWS)
/* Windows: 使用IsDebuggerPresent API */
#if defined(_MSC_VER) && _MSC_VER < 1300
  /* VC6: 动态加载 IsDebuggerPresent */
  return ezctest_is_debugger_present_vc6();
#else
  /* VC7+ 和 MinGW: 直接调用 IsDebuggerPresent */
  return IsDebuggerPresent() ? 1 : 0;
#endif
#else
  return 0;
#endif
}

/* ============================================================================
 * 多进程隔离机制
 * ========================================================================== */

#ifndef EZCTEST_STM32_MODE

#ifdef EZCTEST_PLATFORM_LINUX
#include <sys/types.h>
#include <sys/wait.h>
#endif

/**
 * @brief 在子进程中执行测试（仅非STM32平台）
 * @param test 测试信息
 * @param test_index 测试索引
 * @return 子进程退出码
 */
static int ezctest_exec_child(const ezctest_info_t *test, int test_index) {
#ifdef EZCTEST_PLATFORM_LINUX
  /* Unix/Linux: 使用fork */
  /* 在fork前刷新所有缓冲区，避免子进程继承未刷新的内容 */
  fflush(stdout);
  fflush(stderr);

  pid_t pid = fork();

  if (pid < 0) {
    /* fork失败 */
    fprintf(stderr, "Error: fork() failed\n");
    return -1;
  }

  if (pid == 0) {
    /* 子进程：运行单个测试 */
    int exit_code = 0;

    /* 标记为worker进程（避免重复输出） */
    g_ezctest_worker_index = test_index;

    /* 重置统计 */
    g_ezctest_result.total_tests = 0;
    g_ezctest_result.passed_tests = 0;
    g_ezctest_result.failed_tests = 0;
    g_ezctest_result.total_assertions = 0;
    g_ezctest_result.failed_assertions = 0;

    /* 执行测试 */
    ezctest_run_test(test);

    /* 子进程退出码：0=成功，1=失败 */
    if (g_ezctest_current_failed || g_ezctest_current_assertion_failed) {
      exit_code = 1;
    } else {
      exit_code = 0;
    }

    exit(exit_code);
  } else {
    /* 父进程：等待子进程 */
    int status;
    waitpid(pid, &status, 0);

    if (WIFEXITED(status)) {
      return WEXITSTATUS(status);
    } else if (WIFSIGNALED(status)) {
      /* 信号终止（如段错误） - 返回特殊退出码 */
      int sig = WTERMSIG(status);
      /* 返回128+信号值，模拟shell的信号退出码约定 */
      /* 这样父进程可以识别为异常退出并输出详细信息 */
      return 128 + sig;
    }
    return 2; /* 其他异常情况 */
  }

#elif defined(EZCTEST_PLATFORM_WINDOWS)
  /* Windows: 使用CreateProcess创建子进程 */
  /* 注意：test参数在Windows下未直接使用，通过test_index在子进程中重新查找 */
  (void)test;
  {
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    char cmd_line[2048];
    DWORD exit_code;
    char exe_path[MAX_PATH];
    int cmd_len;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
    si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    si.hStdError = GetStdHandle(STD_ERROR_HANDLE);
    ZeroMemory(&pi, sizeof(pi));

    /* 获取当前程序路径 */
    if (!GetModuleFileNameA(NULL, exe_path, MAX_PATH)) {
      fprintf(stderr, "Error: GetModuleFileName failed (%lu)\n",
              GetLastError());
      return -1;
    }

    /* 构建命令行：程序路径 + worker参数 + 原有参数 */
    cmd_len = snprintf(cmd_line, sizeof(cmd_line), "\"%s\" --ezctest_worker=%d",
                       exe_path, test_index);

    /* 添加过滤器参数（如果有） */
    if (g_ezctest_config.filter && cmd_len < (int)sizeof(cmd_line) - 50) {
      cmd_len += snprintf(cmd_line + cmd_len, sizeof(cmd_line) - cmd_len,
                          " --ezctest_filter=%s", g_ezctest_config.filter);
    }

    /* 添加颜色参数 */
    if (g_ezctest_config.color >= 0 && cmd_len < (int)sizeof(cmd_line) - 30) {
      cmd_len += snprintf(cmd_line + cmd_len, sizeof(cmd_line) - cmd_len,
                          " --ezctest_color=%s",
                          g_ezctest_config.color ? "yes" : "no");
    }

    /* 创建子进程，继承stdout/stderr */
    if (!CreateProcessA(NULL,     /* 应用程序名 */
                        cmd_line, /* 命令行 */
                        NULL,     /* 进程安全属性 */
                        NULL,     /* 线程安全属性 */
                        TRUE,     /* 继承句柄（允许子进程输出到父进程控制台） */
                        0,        /* 创建标志 */
                        NULL,     /* 环境变量 */
                        NULL,     /* 当前目录 */
                        &si,      /* 启动信息 */
                        &pi       /* 进程信息 */
                        )) {
      fprintf(stderr, "Error: CreateProcess failed (%lu)\n", GetLastError());
      fprintf(stderr, "Command: %s\n", cmd_line);
      return -1;
    }

    /* 等待子进程完成 */
    WaitForSingleObject(pi.hProcess, INFINITE);

    /* 获取退出码 */
    if (!GetExitCodeProcess(pi.hProcess, &exit_code)) {
      fprintf(stderr, "Error: GetExitCodeProcess failed (%lu)\n",
              GetLastError());
      CloseHandle(pi.hProcess);
      CloseHandle(pi.hThread);
      return -1;
    }

    /* 清理句柄 */
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    /* 检查退出码 */
    if (exit_code == 0) {
      return 0; /* 成功 */
    } else if (exit_code == 1) {
      return 1; /* 测试失败 */
    } else {
      /* 异常退出（崩溃等） - 返回特殊值让父进程处理输出 */
      return (int)exit_code; /* 返回实际的退出码 */
    }
  }

#else
  (void)test;
  (void)test_index;
  return -1; /* 不支持的平台 */
#endif
}

#endif /* !EZCTEST_STM32_MODE */

/* ============================================================================
 * 浮点数比较辅助函数
 * ========================================================================== */

/**
 * @brief 比较两个 float 是否近似相等
 * @param a 第一个浮点数
 * @param b 第二个浮点数
 * @param epsilon 容差（默认 1e-6f）
 * @return 相等返回1，不等返回0
 */
static int ezctest_float_eq(float a, float b, float epsilon) {
  float diff;
  float abs_a;
  float abs_b;
  float largest;

  diff = a - b;
  if (diff < 0)
    diff = -diff;

  /* 处理特殊值 */
  if (a == b)
    return 1; /* 处理无穷大、零等 */

  /* 使用相对误差和绝对误差的混合方式 */
  abs_a = a < 0 ? -a : a;
  abs_b = b < 0 ? -b : b;
  largest = abs_a > abs_b ? abs_a : abs_b;

  return diff <= epsilon * largest || diff < epsilon;
}

/**
 * @brief 比较两个 double 是否近似相等
 * @param a 第一个双精度浮点数
 * @param b 第二个双精度浮点数
 * @param epsilon 容差（默认 1e-10）
 * @return 相等返回1，不等返回0
 */
static int ezctest_double_eq(double a, double b, double epsilon) {
  double diff;
  double abs_a;
  double abs_b;
  double largest;

  diff = a - b;
  if (diff < 0)
    diff = -diff;

  /* 处理特殊值 */
  if (a == b)
    return 1; /* 处理无穷大、零等 */

  /* 使用相对误差和绝对误差的混合方式 */
  abs_a = a < 0 ? -a : a;
  abs_b = b < 0 ? -b : b;
  largest = abs_a > abs_b ? abs_a : abs_b;

  return diff <= epsilon * largest || diff < epsilon;
}

/* ============================================================================
 * 断言实现
 * ========================================================================== */

/**
 * @brief 断言失败处理
 * @param file 源文件名
 * @param line 行号
 * @param is_fatal 是否为致命错误（ASSERT vs EXPECT）
 * @param format 格式化字符串
 *
 * @note
 * - 所有使用 EXPECT_/ASSERT_ 宏的编译单元都需要看到函数声明
 * - 实现仅在定义了 EZCTEST_IMPLEMENTATION 的编译单元中提供
 */
EZCTEST_API void ezctest_assertion_failed(const char *file, int line,
                                          int is_fatal, const char *format,
                                          ...);

/**
 * @brief 断言成功处理
 *
 * @note 同上，声明对所有编译单元可见，实现仅在 EZCTEST_IMPLEMENTATION 下提供
 */
EZCTEST_API void ezctest_assertion_passed(void);

#ifdef EZCTEST_IMPLEMENTATION

void ezctest_assertion_failed(const char *file, int line, int is_fatal,
                              const char *format, ...) {
  va_list args;

  g_ezctest_result.total_assertions++;
  g_ezctest_result.failed_assertions++;
  g_ezctest_current_assertion_failed = 1;

  ezctest_printf_colored(EZCTEST_COLOR_RED, "%s:%d: Failure\n", file, line);

  va_start(args, format);
  printf("  ");
  vprintf(format, args);
  printf("\n");
  va_end(args);

  if (is_fatal) {
    g_ezctest_current_failed = 1;
  }
}

void ezctest_assertion_passed(void) { g_ezctest_result.total_assertions++; }

#endif /* EZCTEST_IMPLEMENTATION */

/* ============================================================================
 * 命令行参数解析
 * ========================================================================== */

/**
 * @brief 解析命令行参数
 * @param argc 参数个数
 * @param argv 参数数组
 */
static void ezctest_parse_arguments(int argc, char *argv[]) {
  int i;

  for (i = 1; i < argc; i++) {
    const char *arg = argv[i];

    if (strncmp(arg, "--ezctest_filter=", 15) == 0 ||
        strncmp(arg, "--filter=", 9) == 0) {
      const char *eq = strchr(arg, '=');
      g_ezctest_config.filter = eq + 1;
    } else if (strncmp(arg, "--ezctest_repeat=", 15) == 0 ||
               strncmp(arg, "--repeat=", 9) == 0) {
      const char *eq = strchr(arg, '=');
      g_ezctest_config.repeat = atoi(eq + 1);
      if (g_ezctest_config.repeat < 1) {
        g_ezctest_config.repeat = 1;
      }
    } else if (strcmp(arg, "--ezctest_shuffle") == 0 ||
               strcmp(arg, "--shuffle") == 0) {
      g_ezctest_config.shuffle = 1;
    } else if (strcmp(arg, "--ezctest_color=yes") == 0 ||
               strcmp(arg, "--color=yes") == 0) {
      g_ezctest_config.color = 1;
    } else if (strcmp(arg, "--ezctest_color=no") == 0 ||
               strcmp(arg, "--color=no") == 0) {
      g_ezctest_config.color = 0;
    } else if (strcmp(arg, "--ezctest_list_tests") == 0 ||
               strcmp(arg, "--list_tests") == 0) {
      g_ezctest_config.list_tests = 1;
    } else if (strcmp(arg, "--ezctest_no_exec") == 0 ||
               strcmp(arg, "--no_exec") == 0) {
      g_ezctest_config.no_exec = 1;
    } else if (strncmp(arg, "--ezctest_worker=", 15) == 0) {
      const char *eq = strchr(arg, '=');
      g_ezctest_worker_index = atoi(eq + 1);
    } else if (strcmp(arg, "--help") == 0 || strcmp(arg, "-h") == 0) {
      printf("Usage: %s [OPTIONS]\n", argv[0]);
      printf("\nOptions:\n");
      printf("  --ezctest_filter=PATTERN    Run only tests matching PATTERN\n");
      printf("  --ezctest_repeat=COUNT      Run tests COUNT times\n");
      printf("  --ezctest_shuffle           Randomize test order\n");
      printf("  --ezctest_color=yes|no      Enable/disable colored output\n");
      printf("  --ezctest_list_tests        List all tests without running\n");
      printf("  --ezctest_no_exec           Disable process isolation (run in "
             "same process)\n");
      printf("  --help, -h                Show this help message\n");
      printf("\nFilter patterns:\n");
      printf("  *          Match any characters\n");
      printf("  ?          Match single character\n");
      printf("  :          Separate multiple patterns\n");
      printf("  -PATTERN   Exclude tests matching PATTERN\n");
      printf("\nExamples:\n");
      printf("  %s --ezctest_filter=MyTest.*\n", argv[0]);
      printf("  %s --ezctest_filter=*Fast*:*Quick*\n", argv[0]);
      printf("  %s --ezctest_filter=*:-*Slow*\n", argv[0]);
      printf("\nProcess Isolation:\n");
      printf("  By default, tests run in separate processes for isolation.\n");
      printf("  Under debugger or single test, isolation is auto-disabled.\n");
      printf("  Use --ezctest_no_exec to force single-process mode.\n");
      exit(0);
    }
  }
}

/* ============================================================================
 * 测试执行
 * ========================================================================== */

/**
 * @brief 查找测试套件的fixture
 * @param suite_name 测试套件名称
 * @return fixture指针，未找到返回NULL
 */
static const ezctest_fixture_t *ezctest_find_fixture(const char *suite_name) {
  int i;
  for (i = 0; i < g_ezctest_fixture_count; i++) {
    if (strcmp(g_ezctest_fixtures[i].suite_name, suite_name) == 0) {
      return &g_ezctest_fixtures[i];
    }
  }
  return NULL;
}

/* ============================================================================
 * 异常安全的测试执行包装函数
 * ========================================================================== */

#if defined(__cplusplus) && defined(_MSC_VER) && !defined(__clang__)
/**
 * @brief C++ 异常处理包装函数（用于 MSVC）
 * @param test_func 测试函数指针
 * @return 是否有异常（0=无异常，1=有异常）
 * @note 这个函数必须独立，因为 MSVC 不允许在同一函数中混合 SEH 和 C++ 异常
 */
static int ezctest_run_with_cpp_exception(void (*test_func)(void)) {
  int has_exception = 0;
  try {
    test_func();
  } catch (const std::exception &e) {
    g_ezctest_current_failed = 1;
    printf("  Uncaught C++ exception (std::exception): %s\n", e.what());
    has_exception = 1;
  } catch (...) {
    g_ezctest_current_failed = 1;
    printf("  Uncaught C++ exception (unknown type)\n");
    has_exception = 1;
  }
  return has_exception;
}
#endif

/**
 * @brief 执行测试函数（带异常保护）
 * @param test 测试信息
 * @param fixture Fixture信息
 * @return 是否有异常（0=无异常，1=有异常）
 */
static int
ezctest_run_test_with_exception_guard(const ezctest_info_t *test,
                                      const ezctest_fixture_t *fixture) {
  int has_exception = 0;

  /* 设置longjmp跳转点 */
  g_ezctest_longjmp_ctx.has_jumped = 1;
  if (setjmp(g_ezctest_longjmp_ctx.jmp_env) == 0) {
    /* 第一次进入：执行Setup */
    if (fixture && fixture->setup) {
      fixture->setup();
    }

    /* 执行测试 - 带异常保护 */
#if defined(__cplusplus) && defined(_MSC_VER) && !defined(__clang__)
    /* MSVC C++: SEH 包装 C++ 异常处理（分离到不同函数以避免 C2713 错误） */
    __try {
      has_exception = ezctest_run_with_cpp_exception(test->test_func);
    } __except (EXCEPTION_EXECUTE_HANDLER) {
      g_ezctest_current_failed = 1;
      printf("  Windows Structured Exception (SEH): 0x%08lX\n",
             GetExceptionCode());

      /* 解释常见的SEH异常 */
      switch (GetExceptionCode()) {
      case EXCEPTION_ACCESS_VIOLATION:
        printf("  Reason: Access Violation\n");
        break;
      case EXCEPTION_INT_DIVIDE_BY_ZERO:
        printf("  Reason: Integer Division by Zero\n");
        break;
      case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
        printf("  Reason: Array Bounds Exceeded\n");
        break;
      case EXCEPTION_STACK_OVERFLOW:
        printf("  Reason: Stack Overflow\n");
        break;
      case EXCEPTION_ILLEGAL_INSTRUCTION:
        printf("  Reason: Illegal Instruction\n");
        break;
      case EXCEPTION_FLT_DIVIDE_BY_ZERO:
        printf("  Reason: Float Division by Zero\n");
        break;
      case EXCEPTION_INT_OVERFLOW:
        printf("  Reason: Integer Overflow\n");
        break;
      default:
        break;
      }
      has_exception = 1;
    }

#elif defined(__cplusplus)
    /* GCC/Clang C++: 只支持C++异常 */
    try {
      test->test_func();
    } catch (const std::exception &e) {
      g_ezctest_current_failed = 1;
      printf("  Uncaught C++ exception (std::exception): %s\n", e.what());
      has_exception = 1;
    } catch (...) {
      g_ezctest_current_failed = 1;
      printf("  Uncaught C++ exception (unknown type)\n");
      has_exception = 1;
    }

#elif defined(_MSC_VER) && !defined(__clang__)
    /* MSVC C模式: 支持SEH */
    __try {
      test->test_func();
    } __except (EXCEPTION_EXECUTE_HANDLER) {
      g_ezctest_current_failed = 1;
      printf("  Windows Structured Exception (SEH): 0x%08lX\n",
             GetExceptionCode());

      switch (GetExceptionCode()) {
      case EXCEPTION_ACCESS_VIOLATION:
        printf("  Reason: Access Violation\n");
        break;
      case EXCEPTION_INT_DIVIDE_BY_ZERO:
        printf("  Reason: Integer Division by Zero\n");
        break;
      case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
        printf("  Reason: Array Bounds Exceeded\n");
        break;
      case EXCEPTION_STACK_OVERFLOW:
        printf("  Reason: Stack Overflow\n");
        break;
      default:
        break;
      }
      has_exception = 1;
    }

#else
    /* 纯C模式: 无异常保护 */
    test->test_func();
#endif

  } else {
    /* longjmp跳转到这里 - ASSERT失败 */
    has_exception = 2; /* 标记为longjmp */
  }

  return has_exception;
}

/**
 * @brief 执行单个测试
 * @param test 测试信息
 */
static void ezctest_run_test(const ezctest_info_t *test) {
  clock_t start, end;
  double elapsed_ms;
  const ezctest_fixture_t *fixture;
  int exception_type = 0; /* 0=无, 1=C++/SEH异常, 2=longjmp */
  int is_worker = (g_ezctest_worker_index >= 0); /* 是否为子进程 */

  g_ezctest_current_failed = 0;
  g_ezctest_current_assertion_failed = 0;
  ezctest_defer_clear(); /* 清空DEFER栈 */

  /* Worker 模式下不输出（避免重复） */
  if (!is_worker) {
    ezctest_printf_colored(EZCTEST_COLOR_GREEN, "[ RUN      ] ");
    printf("%s.%s\n", test->suite_name, test->test_name);
  }

  /* 查找fixture */
  fixture = ezctest_find_fixture(test->suite_name);

  start = clock();

  /* 执行测试（带异常保护） */
  exception_type = ezctest_run_test_with_exception_guard(test, fixture);

  /* 无论如何都执行清理 */
  g_ezctest_longjmp_ctx.has_jumped = 0;

  /* 执行DEFER清理（LIFO） */
  ezctest_defer_execute();
  ezctest_defer_clear();

  /* 执行Teardown */
  if (fixture && fixture->teardown) {
    fixture->teardown();
  }

  end = clock();
  elapsed_ms = ((double)(end - start) * 1000.0) / CLOCKS_PER_SEC;

  /* 输出异常信息（子进程也输出，因为需要知道错误原因） */
  if (exception_type == 1) {
    printf("  (test terminated by exception)\n");
  } else if (exception_type == 2) {
    printf("  (test terminated by ASSERT failure)\n");
  }

  /* Worker 模式：只在结束时输出结果（含毫秒） */
  /* 非 Worker 模式：也输出结果 */
  if (g_ezctest_current_failed || g_ezctest_current_assertion_failed) {
    ezctest_printf_colored(EZCTEST_COLOR_RED, "[  FAILED  ] ");
    printf("%s.%s (%.0f ms)\n", test->suite_name, test->test_name, elapsed_ms);
    fflush(stdout); /* 立即刷新输出 */
    g_ezctest_result.failed_tests++;
  } else {
    ezctest_printf_colored(EZCTEST_COLOR_GREEN, "[       OK ] ");
    printf("%s.%s (%.0f ms)\n", test->suite_name, test->test_name, elapsed_ms);
    fflush(stdout); /* 立即刷新输出 */
    g_ezctest_result.passed_tests++;
  }
}

/**
 * @brief Worker模式：只运行指定索引的测试
 * @param worker_index Worker索引（从0开始）
 * @return 测试退出码（0=成功，1=失败）
 */
static int ezctest_worker_mode(int worker_index) {
  int i;
  int test_count = 0;

  /* 找到第worker_index个启用的测试 */
  for (i = 0; i < g_ezctest_count; i++) {
    if (!g_ezctest_registry[i].enabled) {
      continue;
    }

    if (!ezctest_matches_filter(g_ezctest_registry[i].suite_name,
                                g_ezctest_registry[i].test_name,
                                g_ezctest_config.filter)) {
      continue;
    }

    if (test_count == worker_index) {
      /* 找到了，运行这个测试 */
      ezctest_run_test(&g_ezctest_registry[i]);

      /* 返回退出码 */
      if (g_ezctest_current_failed || g_ezctest_current_assertion_failed) {
        return 1; /* 失败 */
      } else {
        return 0; /* 成功 */
      }
    }

    test_count++;
  }

  /* 没找到测试 */
  fprintf(stderr, "Error: Worker index %d not found (total enabled: %d)\n",
          worker_index, test_count);
  return 1;
}

/**
 * @brief 列出所有测试
 */
static void ezctest_list_tests(void) {
  int i;
  int count = 0;
  const char *last_suite = "";

  for (i = 0; i < g_ezctest_count; i++) {
    const ezctest_info_t *test = &g_ezctest_registry[i];

    if (!test->enabled) {
      continue;
    }

    if (!ezctest_matches_filter(test->suite_name, test->test_name,
                                g_ezctest_config.filter)) {
      continue;
    }

    if (strcmp(last_suite, test->suite_name) != 0) {
      printf("%s.\n", test->suite_name);
      last_suite = test->suite_name;
    }

    printf("  %s\n", test->test_name);
    count++;
  }

  printf("\nTotal: %d test(s)\n", count);
}

/**
 * @brief MSVC专用：扫描内存查找测试元数据
 */
#if defined(_MSC_VER)
static void ezctest_scan_tests_in_memory(void) {
  MEMORY_BASIC_INFORMATION mbi;
  unsigned char *addr;
  unsigned char *end_addr;
  int count = 0;
  int regions_scanned = 0;
  /* x64使用8字节对齐，x86使用4字节对齐 */
  const size_t alignment = sizeof(void *);

  if (g_ezctest_scanned) {
    return; /* 已扫描 */
  }

  /* MSVC: 使用Windows API扫描内存 */

  /* 获取模块基址 */
  addr = (unsigned char *)GetModuleHandle(NULL);

#ifdef EZCTEST_DEBUG_SCAN
  fprintf(stderr, "DEBUG: Starting memory scan (alignment=%d, ptr_size=%d)\n",
          (int)alignment, (int)sizeof(void *));
  fprintf(stderr, "DEBUG: Module base address: %p\n", (void *)addr);
  fprintf(stderr, "DEBUG: EZCTEST_MAGIC = 0x%08X\n", EZCTEST_MAGIC);
  fprintf(stderr, "DEBUG: sizeof(ezctest_metadata_t) = %d\n",
          (int)sizeof(ezctest_metadata_t));
#endif

  /* 扫描进程内存 */
  while (VirtualQuery(addr, &mbi, sizeof(mbi)) == sizeof(mbi)) {
    /* 只扫描已提交的可读内存 */
    if (mbi.State == MEM_COMMIT &&
        (mbi.Protect == PAGE_READONLY || mbi.Protect == PAGE_READWRITE ||
         mbi.Protect == PAGE_EXECUTE_READ ||
         mbi.Protect == PAGE_EXECUTE_READWRITE)) {

      regions_scanned++;
      end_addr = (unsigned char *)mbi.BaseAddress + mbi.RegionSize;

#ifdef EZCTEST_DEBUG_SCAN
      if (regions_scanned <= 10) {
        fprintf(stderr, "DEBUG: Scanning region %d: %p - %p (size=%lu)\n",
                regions_scanned, mbi.BaseAddress, (void *)end_addr,
                (unsigned long)mbi.RegionSize);
      }
#endif

      /* 在这个内存区域中查找 magic number */
      for (addr = (unsigned char *)mbi.BaseAddress;
           addr < end_addr - sizeof(ezctest_metadata_t); addr += alignment) {

        ezctest_metadata_t *meta = (ezctest_metadata_t *)addr;
        ezctest_fixture_metadata_t *fixture_meta =
            (ezctest_fixture_metadata_t *)addr;

        /* 检查测试用例 magic number */
        if (meta->magic == EZCTEST_MAGIC) {
#ifdef EZCTEST_DEBUG_SCAN
          fprintf(stderr, "DEBUG: Found magic at %p\n", (void *)addr);
#endif
          /* 验证数据有效性 */
          __try {
            if (meta->info.suite_name != NULL && meta->info.test_name != NULL &&
                meta->info.test_func != NULL &&
                strlen(meta->info.suite_name) > 0 &&
                strlen(meta->info.suite_name) < 100 &&
                strlen(meta->info.test_name) > 0 &&
                strlen(meta->info.test_name) < 100) {

              /* 添加到注册表 */
              if (g_ezctest_count < EZCTEST_MAX_TESTS) {
                g_ezctest_registry[g_ezctest_count] = meta->info;
                g_ezctest_count++;
                count++;
#ifdef EZCTEST_DEBUG_SCAN
                fprintf(stderr, "DEBUG: Registered test: %s.%s\n",
                        meta->info.suite_name, meta->info.test_name);
#endif
              }
            }
          } __except (EXCEPTION_EXECUTE_HANDLER) {
            /* 忽略无效指针 */
#ifdef EZCTEST_DEBUG_SCAN
            fprintf(stderr, "DEBUG: Invalid pointer at %p\n", (void *)addr);
#endif
          }
        }
        /* 检查fixture magic number */
        else if (fixture_meta->magic == EZCTEST_FIXTURE_MAGIC) {
          __try {
            if (fixture_meta->suite_name != NULL &&
                strlen(fixture_meta->suite_name) > 0 &&
                strlen(fixture_meta->suite_name) < 100) {

              /* 注册SETUP或TEARDOWN */
              if (fixture_meta->is_setup && fixture_meta->setup != NULL) {
                ezctest_register_setup(fixture_meta->suite_name,
                                       fixture_meta->setup);
              } else if (!fixture_meta->is_setup &&
                         fixture_meta->teardown != NULL) {
                ezctest_register_teardown(fixture_meta->suite_name,
                                          fixture_meta->teardown);
              }
            }
          } __except (EXCEPTION_EXECUTE_HANDLER) {
            /* 忽略无效指针 */
          }
        }
      }
    }

    /* 移动到下一个内存区域 */
    addr = (unsigned char *)mbi.BaseAddress + mbi.RegionSize;

    /* 防止无限循环和溢出 */
    if (addr <= (unsigned char *)mbi.BaseAddress)
      break;

    /* x64地址空间检查：防止超过用户态地址空间 */
#ifdef _WIN64
    /* x64: 用户态地址空间最大约 0x00007FFFFFFFFFFF (128TB) */
    if ((size_t)addr > 0x00007FFF00000000ULL)
      break;
#else
    /* x86: 用户态地址空间最大 0x7FFFFFFF (2GB) */
    if ((size_t)addr > 0x7FFFFFFF)
      break;
#endif
  }

#ifdef EZCTEST_DEBUG_SCAN
  fprintf(stderr, "DEBUG: Scan complete. Regions=%d, Tests found=%d\n",
          regions_scanned, count);
#endif

  g_ezctest_scanned = 1;
}
#endif

/**
 * @brief 运行所有匹配的测试
 */
static int ezctest_run_all_tests_internal(void) {
  int i, repeat;
  clock_t start_time, end_time;
  double total_time_ms;
  int enabled_count = 0;
  int use_process_isolation = 0;

  /* 统计启用的测试数量 */
  for (i = 0; i < g_ezctest_count; i++) {
    if (g_ezctest_registry[i].enabled &&
        ezctest_matches_filter(g_ezctest_registry[i].suite_name,
                               g_ezctest_registry[i].test_name,
                               g_ezctest_config.filter)) {
      enabled_count++;
    }
  }

  if (enabled_count == 0) {
    ezctest_printf_colored(EZCTEST_COLOR_YELLOW, "No tests to run\n");
    return 0;
  }

#ifndef EZCTEST_STM32_MODE
  /* 自动决策是否使用进程隔离 */
  if (g_ezctest_config.no_exec == -1) {
    /* 自动模式：调试时或只有一个测试时禁用隔离 */
    if (enabled_count <= 1 || ezctest_under_debugger()) {
      use_process_isolation = 0;
    } else {
      use_process_isolation = 1;
    }
  } else {
    use_process_isolation = (g_ezctest_config.no_exec == 0) ? 1 : 0;
  }
#else
  /* STM32平台不支持进程隔离 */
  use_process_isolation = 0;
#endif

  /* 输出测试开始信息 */
  ezctest_printf_colored(EZCTEST_COLOR_GREEN, "[==========] ");
  printf("Running %d test(s)", enabled_count);
  if (g_ezctest_config.repeat > 1) {
    printf(" (%d iteration(s))", g_ezctest_config.repeat);
  }
#ifndef EZCTEST_STM32_MODE
  if (use_process_isolation) {
    printf(" [Process Isolation: ON]");
  } else {
    printf(" [Process Isolation: OFF]");
  }
#endif
  printf("\n");

  start_time = clock();

  /* 重复执行测试 */
  for (repeat = 0; repeat < g_ezctest_config.repeat; repeat++) {
    int test_count = 0; /* 用于worker索引计数 */

    /* 重置所有测试的失败标志 */
    for (i = 0; i < g_ezctest_count; i++) {
      g_ezctest_registry[i].failed = 0;
    }

    if (g_ezctest_config.repeat > 1) {
      ezctest_printf_colored(EZCTEST_COLOR_CYAN, "\n[----------] ");
      printf("Iteration %d/%d\n", repeat + 1, g_ezctest_config.repeat);
    }

    /* 随机化测试顺序 */
    if (g_ezctest_config.shuffle && repeat == 0) {
      /* 简单的Fisher-Yates洗牌算法 */
      srand((unsigned int)time(NULL));
      for (i = g_ezctest_count - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        ezctest_info_t temp = g_ezctest_registry[i];
        g_ezctest_registry[i] = g_ezctest_registry[j];
        g_ezctest_registry[j] = temp;
      }
    }

    /* 执行测试 */
    for (i = 0; i < g_ezctest_count; i++) {
      ezctest_info_t *test = &g_ezctest_registry[i];

      if (!test->enabled) {
        continue;
      }

      if (!ezctest_matches_filter(test->suite_name, test->test_name,
                                  g_ezctest_config.filter)) {
        continue;
      }

      g_ezctest_result.total_tests++;

#ifndef EZCTEST_STM32_MODE
      /* 根据配置选择执行方式 */
      if (use_process_isolation) {
        /* 多进程隔离模式 */
        int child_exit_code;

        /* 父进程：输出开始标记 */
        ezctest_printf_colored(EZCTEST_COLOR_GREEN, "[ RUN      ] ");
        printf("%s.%s\n", test->suite_name, test->test_name);
        fflush(stdout); /* 立即刷新输出 */

        /* 执行子进程 */
        child_exit_code = ezctest_exec_child(test, test_count);

        /* 父进程：根据子进程退出码决定是否输出 */
        if (child_exit_code == 0) {
          /* 子进程正常退出（测试通过），子进程已输出结果，父进程不输出 */
          g_ezctest_result.passed_tests++;
        } else if (child_exit_code == 1) {
          /* 子进程正常退出（测试失败），子进程已输出结果，父进程不输出 */
          g_ezctest_result.failed_tests++;
          test->failed = 1;
        } else if (child_exit_code < 0) {
          /* 进程创建失败，回退到单进程模式 */
          ezctest_printf_colored(EZCTEST_COLOR_YELLOW, "[ FALLBACK ] ");
          printf("Process isolation failed, running in-process\n");
          ezctest_run_test(test);
          /* 单进程模式下检查失败状态 */
          if (g_ezctest_current_failed || g_ezctest_current_assertion_failed) {
            test->failed = 1;
          }
        } else {
          /* 子进程异常退出（崩溃），父进程输出详细错误信息 */
          printf("  Test terminated abnormally with exit code %d\n",
                 child_exit_code);

          /* 解析退出码 */
#ifdef EZCTEST_PLATFORM_LINUX
          /* Linux: 128+信号值表示被信号终止 */
          if (child_exit_code >= 128 && child_exit_code < 256) {
            int sig = child_exit_code - 128;
            printf("  Reason: Terminated by signal %d (%s)\n", sig,
                   sig == SIGSEGV   ? "SIGSEGV - Segmentation fault"
                   : sig == SIGABRT ? "SIGABRT - Aborted"
                   : sig == SIGFPE  ? "SIGFPE - Floating point exception"
                   : sig == SIGILL  ? "SIGILL - Illegal instruction"
                   : sig == SIGBUS  ? "SIGBUS - Bus error"
                                    : "Unknown signal");
          } else
#endif
            /* Windows 异常退出码或其他 */
            /* 使用 if-else 代替 switch 避免 C++ narrowing conversion 警告 */
            if (child_exit_code == (int)0xC0000005) {
              printf(
                  "  Reason: Access Violation (EXCEPTION_ACCESS_VIOLATION)\n");
            } else if (child_exit_code == (int)0xC0000094) {
              printf("  Reason: Integer Division by Zero "
                     "(EXCEPTION_INT_DIVIDE_BY_ZERO)\n");
            } else if (child_exit_code == (int)0xC000008C) {
              printf("  Reason: Array Bounds Exceeded "
                     "(EXCEPTION_ARRAY_BOUNDS_EXCEEDED)\n");
            } else if (child_exit_code == (int)0xC00000FD) {
              printf("  Reason: Stack Overflow (EXCEPTION_STACK_OVERFLOW)\n");
            } else if (child_exit_code == (int)0xC000001D) {
              printf("  Reason: Illegal Instruction "
                     "(EXCEPTION_ILLEGAL_INSTRUCTION)\n");
            } else if (child_exit_code == 3) {
              printf("  Reason: Assertion failed (abort() called)\n");
            } else if ((unsigned int)child_exit_code >= 0xC0000000U &&
                       (unsigned int)child_exit_code <= 0xDFFFFFFFU) {
              printf("  Reason: Windows Exception (0x%08X)\n",
                     (unsigned int)child_exit_code);
            } else {
              printf("  Reason: Unknown\n");
            }

          ezctest_printf_colored(EZCTEST_COLOR_RED, "[  FAILED  ] ");
          printf("%s.%s\n", test->suite_name, test->test_name);
          g_ezctest_result.failed_tests++;
          test->failed = 1;
        }

        test_count++; /* worker索引递增 */
      } else
#endif
      {
        /* 单进程模式（默认或STM32） */
        ezctest_run_test(test);
        /* 记录失败状态 */
        if (g_ezctest_current_failed || g_ezctest_current_assertion_failed) {
          test->failed = 1;
        }
      }
    }
  }

  end_time = clock();
  total_time_ms = ((double)(end_time - start_time) * 1000.0) / CLOCKS_PER_SEC;

  /* 输出测试总结 */
  ezctest_printf_colored(EZCTEST_COLOR_GREEN, "[==========] ");
  printf("%d test(s) ran (%.0f ms total)\n", g_ezctest_result.total_tests,
         total_time_ms);

  ezctest_printf_colored(EZCTEST_COLOR_GREEN, "[  PASSED  ] ");
  printf("%d test(s)\n", g_ezctest_result.passed_tests);

  if (g_ezctest_result.failed_tests > 0) {
    ezctest_printf_colored(EZCTEST_COLOR_RED, "[  FAILED  ] ");
    printf("%d test(s), listed below:\n", g_ezctest_result.failed_tests);

    /* 列出失败的测试 */
    for (i = 0; i < g_ezctest_count; i++) {
      if (g_ezctest_registry[i].failed) {
        ezctest_printf_colored(EZCTEST_COLOR_RED, "[  FAILED  ] ");
        printf("%s.%s\n", g_ezctest_registry[i].suite_name,
               g_ezctest_registry[i].test_name);
      }
    }
  }

  /* 断言统计：仅在非进程隔离模式下有效 */
  if (use_process_isolation) {
#if 0
        printf("\nAssertions: N/A (process isolation mode - assertion counts not available)\n");
#endif
  } else {
    printf("\nAssertions: %d total, %d passed, %d failed\n",
           g_ezctest_result.total_assertions,
           g_ezctest_result.total_assertions -
               g_ezctest_result.failed_assertions,
           g_ezctest_result.failed_assertions);
  }

  if (g_ezctest_result.total_tests == g_ezctest_result.passed_tests) {
    ezctest_set_color(EZCTEST_COLOR_GREEN);
    printf("ALL %d TESTS PASSED!\n", g_ezctest_result.total_tests);
    ezctest_reset_color();
  }

  return (g_ezctest_result.failed_tests == 0) ? 0 : 1;
}

/* ============================================================================
 * STM32交互式模式
 * ========================================================================== */

#ifdef EZCTEST_STM32_INTERACTIVE

/**
 * @brief STM32交互式命令行模式
 *
 * @details 在STM32上运行交互式测试环境，用户可以通过串口输入命令
 */
static void ezctest_stm32_interactive_loop(void) {
  char cmd_buffer[EZCTEST_STM32_CMD_BUFFER_SIZE];
  int cmd_pos = 0;

  printf("\n");
  ezctest_printf_colored(EZCTEST_COLOR_CYAN,
                         "===========================================\n");
  ezctest_printf_colored(EZCTEST_COLOR_CYAN,
                         "  CTest Interactive Mode (STM32)\n");
  ezctest_printf_colored(EZCTEST_COLOR_CYAN,
                         "===========================================\n");
  printf("\nCommands:\n");
  printf("  run [filter]    Run tests (optional filter)\n");
  printf("  list            List all tests\n");
  printf("  help            Show this help\n");
  printf("  exit            Exit interactive mode\n");
  printf("\nType 'run' to run all tests, or 'help' for more info.\n\n");

  while (1) {
    printf("> ");
    fflush(stdout);

    /* 读取一行命令 */
    cmd_pos = 0;
    while (1) {
      int ch = getchar();

      if (ch == '\r' || ch == '\n') {
        cmd_buffer[cmd_pos] = '\0';
        printf("\n");
        break;
      } else if (ch == '\b' || ch == 127) { /* 退格键 */
        if (cmd_pos > 0) {
          cmd_pos--;
          printf("\b \b");
          fflush(stdout);
        }
      } else if (ch >= 32 && ch < 127 &&
                 cmd_pos < EZCTEST_STM32_CMD_BUFFER_SIZE - 1) {
        cmd_buffer[cmd_pos++] = (char)ch;
        putchar(ch);
        fflush(stdout);
      }
    }

    /* 处理命令 */
    if (cmd_buffer[0] == '\0') {
      continue;
    }

    if (strcmp(cmd_buffer, "exit") == 0 || strcmp(cmd_buffer, "quit") == 0) {
      printf("Exiting interactive mode...\n");
      break;
    } else if (strcmp(cmd_buffer, "help") == 0) {
      printf("\nAvailable commands:\n");
      printf("  run [filter]    Run tests matching filter pattern\n");
      printf("  list [filter]   List tests matching filter pattern\n");
      printf("  repeat N        Set repeat count to N\n");
      printf("  help            Show this help message\n");
      printf("  exit            Exit interactive mode\n");
      printf("\nFilter examples:\n");
      printf("  run MyTest.*\n");
      printf("  run *Fast*\n");
      printf("  list\n\n");
    } else if (strcmp(cmd_buffer, "list") == 0) {
      g_ezctest_config.filter = NULL;
      ezctest_list_tests();
      printf("\n");
    } else if (strncmp(cmd_buffer, "list ", 5) == 0) {
      g_ezctest_config.filter = cmd_buffer + 5;
      ezctest_list_tests();
      g_ezctest_config.filter = NULL;
      printf("\n");
    } else if (strcmp(cmd_buffer, "run") == 0) {
      /* 重置统计 */
      memset(&g_ezctest_result, 0, sizeof(g_ezctest_result));
      g_ezctest_config.filter = NULL;
      ezctest_run_all_tests_internal();
      printf("\n");
    } else if (strncmp(cmd_buffer, "run ", 4) == 0) {
      /* 重置统计 */
      memset(&g_ezctest_result, 0, sizeof(g_ezctest_result));
      g_ezctest_config.filter = cmd_buffer + 4;
      ezctest_run_all_tests_internal();
      g_ezctest_config.filter = NULL;
      printf("\n");
    } else if (strncmp(cmd_buffer, "repeat ", 7) == 0) {
      int repeat = atoi(cmd_buffer + 7);
      if (repeat > 0) {
        g_ezctest_config.repeat = repeat;
        printf("Repeat count set to %d\n\n", repeat);
      } else {
        printf("Invalid repeat count\n\n");
      }
    } else {
      printf("Unknown command: %s\n", cmd_buffer);
      printf("Type 'help' for available commands.\n\n");
    }
  }
}

#endif /* EZCTEST_STM32_INTERACTIVE */

/* ============================================================================
 * 公共API
 * ========================================================================== */

/**
 * @brief 运行所有测试
 * @param argc 命令行参数个数
 * @param argv 命令行参数数组
 * @return 测试通过返回0，失败返回1
 *
 * 注意：为了让所有单元看到同一份测试注册表，必须在恰好一个
 * 源文件中定义 EZCTEST_IMPLEMENTATION 再包含本头文件（本工程中为
 * src/labez_core_unittest/main.c）。
 */
EZCTEST_API int ezctest_run_all_tests(int argc, char *argv[]);

#ifdef EZCTEST_IMPLEMENTATION

int ezctest_run_all_tests(int argc, char *argv[]) {
  /* 保存程序路径 */
  if (argc > 0 && argv != NULL && argv[0] != NULL) {
    g_ezctest_argv0 = argv[0];
  }

  /* 解析命令行参数 */
  if (argc > 0 && argv != NULL) {
    ezctest_parse_arguments(argc, argv);
  }

#if defined(_MSC_VER)
  /* MSVC: 扫描内存查找测试 */
  ezctest_scan_tests_in_memory();
#endif
  /* 老版本GCC使用.ctors段自动注册，不需要扫描 */

  /* 如果是worker模式，只运行一个测试 */
  if (g_ezctest_worker_index >= 0) {
    return ezctest_worker_mode(g_ezctest_worker_index);
  }

  /* 如果只是列出测试 */
  if (g_ezctest_config.list_tests) {
    ezctest_list_tests();
    return 0;
  }

  /* 执行测试 */
  return ezctest_run_all_tests_internal();
}

#endif /* EZCTEST_IMPLEMENTATION */

/* ============================================================================
 * Unicode/宽字符支持（Windows平台）
 * ========================================================================== */

#ifdef EZCTEST_PLATFORM_WINDOWS

/**
 * @brief 运行所有测试（宽字符版本，用于Unicode程序）
 * @param argc 命令行参数个数
 * @param wargv 宽字符命令行参数数组
 * @return 测试通过返回0，失败返回1
 *
 * @details
 * 用于Windows平台的Unicode版本main函数（wmain或_tmain）。
 * 该函数将宽字符参数转换为多字节字符串，然后调用标准版本的测试框架。
 *
 * 使用示例：
 * @code
 * // Unicode版本
 * int wmain(int argc, wchar_t* argv[]) {
 *     return RUN_ALL_TESTS_W(argc, argv);
 * }
 *
 * // TCHAR版本（根据_UNICODE自动选择）
 * int _tmain(int argc, TCHAR* argv[]) {
 *     return RUN_ALL_TESTS_T(argc, argv);
 * }
 * @endcode
 */
EZCTEST_API int ezctest_run_all_tests_w(int argc, wchar_t *wargv[]);

#ifdef EZCTEST_IMPLEMENTATION

int ezctest_run_all_tests_w(int argc, wchar_t *wargv[]) {
  char **argv = NULL;
  int result;
  int i;

  /* 分配char*数组 */
  if (argc > 0 && wargv != NULL) {
    argv = (char **)malloc(sizeof(char *) * (size_t)argc);
    if (argv == NULL) {
      return 1;
    }

    /* 初始化为NULL，便于清理 */
    for (i = 0; i < argc; i++) {
      argv[i] = NULL;
    }

    /* 转换每个宽字符参数为多字节字符 */
    for (i = 0; i < argc; i++) {
      if (wargv[i] != NULL) {
        size_t len;

        /* 计算所需缓冲区大小 */
#if defined(_MSC_VER) && _MSC_VER >= 1400 /* VC8+ */
        size_t converted;
        if (wcstombs_s(&converted, NULL, 0, wargv[i], 0) != 0) {
          /* 转换失败，清理并返回 */
          int j;
          for (j = 0; j < i; j++) {
            if (argv[j] != NULL) {
              free(argv[j]);
            }
          }
          free(argv);
          return 1;
        }
        len = converted;
#else
        len = wcstombs(NULL, wargv[i], 0);
        if (len == (size_t)-1) {
          /* 转换失败，清理并返回 */
          int j;
          for (j = 0; j < i; j++) {
            if (argv[j] != NULL) {
              free(argv[j]);
            }
          }
          free(argv);
          return 1;
        }
        len++; /* 包含空终止符 */
#endif

        /* 分配缓冲区 */
        argv[i] = (char *)malloc(len);
        if (argv[i] == NULL) {
          /* 内存分配失败，清理并返回 */
          int j;
          for (j = 0; j < i; j++) {
            if (argv[j] != NULL) {
              free(argv[j]);
            }
          }
          free(argv);
          return 1;
        }

        /* 执行转换 */
#if defined(_MSC_VER) && _MSC_VER >= 1400 /* VC8+ */
        {
          size_t converted;
          if (wcstombs_s(&converted, argv[i], len, wargv[i], len - 1) != 0) {
            /* 转换失败，清理并返回 */
            int j;
            for (j = 0; j <= i; j++) {
              if (argv[j] != NULL) {
                free(argv[j]);
              }
            }
            free(argv);
            return 1;
          }
        }
#else
        wcstombs(argv[i], wargv[i], len);
#endif
      }
    }
  }

  /* 调用标准版本的函数 */
  result = ezctest_run_all_tests(argc, argv);

  /* 释放内存 */
  if (argv != NULL) {
    for (i = 0; i < argc; i++) {
      if (argv[i] != NULL) {
        free(argv[i]);
      }
    }
    free(argv);
  }

  return result;
}

#endif /* EZCTEST_IMPLEMENTATION */

/**
 * @brief 运行所有测试（宽字符版本）宏定义
 * @param argc 命令行参数个数
 * @param argv 宽字符命令行参数数组
 * @return 测试通过返回0，失败返回1
 *
 * @details
 * 此宏显式使用宽字符版本，适用于明确需要 wmain 的场景。
 * 通常情况下，建议直接使用 RUN_ALL_TESTS，它会自动根据 _UNICODE 宏选择。
 */
#define RUN_ALL_TESTS_W(argc, argv) ezctest_run_all_tests_w(argc, argv)

/**
 * @brief 运行所有测试（TCHAR版本别名）
 * @param argc 命令行参数个数
 * @param argv TCHAR类型命令行参数数组
 * @return 测试通过返回0，失败返回1
 *
 * @details
 * 此宏是 RUN_ALL_TESTS 的别名，保留用于显式表明使用 TCHAR 类型。
 * 推荐直接使用 RUN_ALL_TESTS，效果相同。
 */
#define RUN_ALL_TESTS_T(argc, argv) RUN_ALL_TESTS(argc, argv)

#endif /* EZCTEST_PLATFORM_WINDOWS */

/* ============================================================================
 * 宏定义 - 测试定义
 * ========================================================================== */

/**
 * @brief 定义一个测试用例
 * @param suite_name 测试套件名称
 * @param test_name 测试用例名称
 *
 * @details
 * 使用示例：
 * @code
 * TEST(MathTest, Addition) {
 *     EXPECT_EQ(1 + 1, 2);
 * }
 * @endcode
 */

/* ============================================================================
 * 测试自动注册机制
 * ========================================================================== */
/**
 * @brief TEST 宏：定义一个测试用例
 *
 * @note VC6: 使用段扫描机制自动注册，需要在TEST定义周围添加段标记
 * @note VC7+: 使用 .CRT$XCU 机制自动注册
 * @note GCC < 3.3: 使用.ctors段机制自动注册（类似constructor）
 * @note GCC >= 3.3: 使用constructor属性自动注册
 */
#if defined(_MSC_VER)
/* MSVC所有版本: 生成包含 magic number 的元数据，使用内存扫描机制 */
#define TEST(suite_name, test_name)                                            \
  static void ezctest_##suite_name##_##test_name##_func(void);                 \
  static const ezctest_metadata_t ezctest_##suite_name##_##test_name##_meta =  \
      {EZCTEST_MAGIC,                                                          \
       {#suite_name, #test_name, ezctest_##suite_name##_##test_name##_func, 1, \
        0}};                                                                   \
  static volatile const void *ezctest_keep_##suite_name##_##test_name##_meta = \
      &ezctest_##suite_name##_##test_name##_meta;                              \
  static void ezctest_##suite_name##_##test_name##_func(void)
#elif defined(EZCTEST_GCC_NO_CONSTRUCTOR)
/* 老版本GCC: 使用.ctors段实现自动注册 */
#define TEST(suite_name, test_name)                                            \
  static void ezctest_##suite_name##_##test_name##_func(void);                 \
  static void ezctest_##suite_name##_##test_name##_register(void) {            \
    ezctest_register(#suite_name, #test_name,                                  \
                     ezctest_##suite_name##_##test_name##_func);               \
  }                                                                            \
  static void (*ezctest_##suite_name##_##test_name##_ctor_ptr)(void)           \
      __attribute__((section(".ctors"), used)) =                               \
          ezctest_##suite_name##_##test_name##_register;                       \
  static void ezctest_##suite_name##_##test_name##_func(void)
#else
/* 现代GCC/Clang: 使用 constructor 属性自动注册 */
#define TEST(suite_name, test_name)                                            \
  static void ezctest_##suite_name##_##test_name##_func(void);                 \
  static int ezctest_##suite_name##_##test_name##_registered = 0;              \
  static void ezctest_##suite_name##_##test_name##_register(void) {            \
    if (!ezctest_##suite_name##_##test_name##_registered) {                    \
      ezctest_register(#suite_name, #test_name,                                \
                       ezctest_##suite_name##_##test_name##_func);             \
      ezctest_##suite_name##_##test_name##_registered = 1;                     \
    }                                                                          \
  }                                                                            \
  static void __attribute__((constructor))                                     \
  ezctest_##suite_name##_##test_name##_init(void) {                            \
    ezctest_##suite_name##_##test_name##_register();                           \
  }                                                                            \
  static void ezctest_##suite_name##_##test_name##_func(void)
#endif

/* ============================================================================
 * 宏定义 - EXPECT断言（非致命）
 * ========================================================================== */

#define EXPECT_TRUE(condition)                                                 \
  do {                                                                         \
    if (condition) {                                                           \
      ezctest_assertion_passed();                                              \
    } else {                                                                   \
      ezctest_assertion_failed(__FILE__, __LINE__, 0,                          \
                               "Expected: (%s) is true\n  Actual: false",      \
                               #condition);                                    \
    }                                                                          \
  } while (0)

#define EXPECT_FALSE(condition)                                                \
  do {                                                                         \
    if (!(condition)) {                                                        \
      ezctest_assertion_passed();                                              \
    } else {                                                                   \
      ezctest_assertion_failed(__FILE__, __LINE__, 0,                          \
                               "Expected: (%s) is false\n  Actual: true",      \
                               #condition);                                    \
    }                                                                          \
  } while (0)

#define EXPECT_EQ(val1, val2)                                                  \
  do {                                                                         \
    if ((val1) == (val2)) {                                                    \
      ezctest_assertion_passed();                                              \
    } else {                                                                   \
      char ezctest_msg_buf[EZCTEST_MAX_MESSAGE_LENGTH];                        \
      EZCTEST_FORMAT_VALUES("==", val1, val2, ezctest_msg_buf,                 \
                            EZCTEST_MAX_MESSAGE_LENGTH);                       \
      ezctest_assertion_failed(__FILE__, __LINE__, 0, "%s", ezctest_msg_buf);  \
    }                                                                          \
  } while (0)

#define EXPECT_NE(val1, val2)                                                  \
  do {                                                                         \
    if ((val1) != (val2)) {                                                    \
      ezctest_assertion_passed();                                              \
    } else {                                                                   \
      char ezctest_msg_buf[EZCTEST_MAX_MESSAGE_LENGTH];                        \
      EZCTEST_FORMAT_VALUES("!=", val1, val2, ezctest_msg_buf,                 \
                            EZCTEST_MAX_MESSAGE_LENGTH);                       \
      ezctest_assertion_failed(__FILE__, __LINE__, 0, "%s", ezctest_msg_buf);  \
    }                                                                          \
  } while (0)

#define EXPECT_LT(val1, val2)                                                  \
  do {                                                                         \
    if ((val1) < (val2)) {                                                     \
      ezctest_assertion_passed();                                              \
    } else {                                                                   \
      char ezctest_msg_buf[EZCTEST_MAX_MESSAGE_LENGTH];                        \
      EZCTEST_FORMAT_VALUES("<", val1, val2, ezctest_msg_buf,                  \
                            EZCTEST_MAX_MESSAGE_LENGTH);                       \
      ezctest_assertion_failed(__FILE__, __LINE__, 0, "%s", ezctest_msg_buf);  \
    }                                                                          \
  } while (0)

#define EXPECT_LE(val1, val2)                                                  \
  do {                                                                         \
    if ((val1) <= (val2)) {                                                    \
      ezctest_assertion_passed();                                              \
    } else {                                                                   \
      char ezctest_msg_buf[EZCTEST_MAX_MESSAGE_LENGTH];                        \
      EZCTEST_FORMAT_VALUES("<=", val1, val2, ezctest_msg_buf,                 \
                            EZCTEST_MAX_MESSAGE_LENGTH);                       \
      ezctest_assertion_failed(__FILE__, __LINE__, 0, "%s", ezctest_msg_buf);  \
    }                                                                          \
  } while (0)

#define EXPECT_GT(val1, val2)                                                  \
  do {                                                                         \
    if ((val1) > (val2)) {                                                     \
      ezctest_assertion_passed();                                              \
    } else {                                                                   \
      char ezctest_msg_buf[EZCTEST_MAX_MESSAGE_LENGTH];                        \
      EZCTEST_FORMAT_VALUES(">", val1, val2, ezctest_msg_buf,                  \
                            EZCTEST_MAX_MESSAGE_LENGTH);                       \
      ezctest_assertion_failed(__FILE__, __LINE__, 0, "%s", ezctest_msg_buf);  \
    }                                                                          \
  } while (0)

#define EXPECT_GE(val1, val2)                                                  \
  do {                                                                         \
    if ((val1) >= (val2)) {                                                    \
      ezctest_assertion_passed();                                              \
    } else {                                                                   \
      char ezctest_msg_buf[EZCTEST_MAX_MESSAGE_LENGTH];                        \
      EZCTEST_FORMAT_VALUES(">=", val1, val2, ezctest_msg_buf,                 \
                            EZCTEST_MAX_MESSAGE_LENGTH);                       \
      ezctest_assertion_failed(__FILE__, __LINE__, 0, "%s", ezctest_msg_buf);  \
    }                                                                          \
  } while (0)

#define EXPECT_STREQ(str1, str2)                                               \
  do {                                                                         \
    if (strcmp((str1), (str2)) == 0) {                                         \
      ezctest_assertion_passed();                                              \
    } else {                                                                   \
      ezctest_assertion_failed(                                                \
          __FILE__, __LINE__, 0,                                               \
          "Expected: %s == %s\n  Actual: \"%s\" != \"%s\"", #str1, #str2,      \
          (str1), (str2));                                                     \
    }                                                                          \
  } while (0)

#define EXPECT_STRNE(str1, str2)                                               \
  do {                                                                         \
    if (strcmp((str1), (str2)) != 0) {                                         \
      ezctest_assertion_passed();                                              \
    } else {                                                                   \
      ezctest_assertion_failed(                                                \
          __FILE__, __LINE__, 0,                                               \
          "Expected: %s != %s\n  Actual: both are \"%s\"", #str1, #str2,       \
          (str1));                                                             \
    }                                                                          \
  } while (0)

#define EXPECT_NULL(ptr)                                                       \
  do {                                                                         \
    if ((ptr) == NULL) {                                                       \
      ezctest_assertion_passed();                                              \
    } else {                                                                   \
      ezctest_assertion_failed(__FILE__, __LINE__, 0,                          \
                               "Expected: %s is NULL\n  Actual: not NULL",     \
                               #ptr);                                          \
    }                                                                          \
  } while (0)

#define EXPECT_NOT_NULL(ptr)                                                   \
  do {                                                                         \
    if ((ptr) != NULL) {                                                       \
      ezctest_assertion_passed();                                              \
    } else {                                                                   \
      ezctest_assertion_failed(__FILE__, __LINE__, 0,                          \
                               "Expected: %s is not NULL\n  Actual: NULL",     \
                               #ptr);                                          \
    }                                                                          \
  } while (0)

#define EXPECT_EMPTY(ptr, size)                                                \
  do {                                                                         \
    int is_empty = 1;                                                          \
    int i;                                                                     \
    for (i = 0; i < (int)size; ++i) {                                          \
      if (((char *)ptr)[i]) {                                                  \
        is_empty = 0;                                                          \
        break;                                                                 \
      }                                                                        \
    }                                                                          \
    if (is_empty) {                                                            \
      ezctest_assertion_passed();                                              \
    } else {                                                                   \
      ezctest_assertion_failed(__FILE__, __LINE__, 0,                          \
                               "Expected: %s is not empty\n", #ptr);           \
    }                                                                          \
  } while (0)

#define EXPECT_NOT_EMPTY(ptr, size)                                            \
  do {                                                                         \
    int is_empty = 1;                                                          \
    int i;                                                                     \
    for (i = 0; i < (int)size; ++i) {                                          \
      if (((char *)ptr)[i]) {                                                  \
        is_empty = 0;                                                          \
        break;                                                                 \
      }                                                                        \
    }                                                                          \
    if (!is_empty) {                                                           \
      ezctest_assertion_passed();                                              \
    } else {                                                                   \
      ezctest_assertion_failed(__FILE__, __LINE__, 0,                          \
                               "Expected: %s is empty\n", #ptr);               \
    }                                                                          \
  } while (0)

/* ============================================================================
 * 宏定义 - EXPECT浮点数断言（非致命）
 * ========================================================================== */

#define EXPECT_FLOAT_EQ(val1, val2)                                            \
  do {                                                                         \
    float ezctest_v1 = (float)(val1);                                          \
    float ezctest_v2 = (float)(val2);                                          \
    if (ezctest_float_eq(ezctest_v1, ezctest_v2, 1e-6f)) {                     \
      ezctest_assertion_passed();                                              \
    } else {                                                                   \
      ezctest_assertion_failed(                                                \
          __FILE__, __LINE__, 0,                                               \
          "Expected: %s == %s (float)\n  Actual: %g vs %g (diff: %g)", #val1,  \
          #val2, (double)ezctest_v1, (double)ezctest_v2,                       \
          (double)(ezctest_v1 - ezctest_v2));                                  \
    }                                                                          \
  } while (0)

#define EXPECT_DOUBLE_EQ(val1, val2)                                           \
  do {                                                                         \
    double ezctest_v1 = (double)(val1);                                        \
    double ezctest_v2 = (double)(val2);                                        \
    if (ezctest_double_eq(ezctest_v1, ezctest_v2, 1e-10)) {                    \
      ezctest_assertion_passed();                                              \
    } else {                                                                   \
      ezctest_assertion_failed(__FILE__, __LINE__, 0,                          \
                               "Expected: %s == %s (double)\n  Actual: %.15g " \
                               "vs %.15g (diff: %.15g)",                       \
                               #val1, #val2, ezctest_v1, ezctest_v2,           \
                               (ezctest_v1 - ezctest_v2));                     \
    }                                                                          \
  } while (0)

#define EXPECT_NEAR(val1, val2, epsilon)                                       \
  do {                                                                         \
    double ezctest_v1 = (double)(val1);                                        \
    double ezctest_v2 = (double)(val2);                                        \
    double ezctest_eps = (double)(epsilon);                                    \
    if (ezctest_double_eq(ezctest_v1, ezctest_v2, ezctest_eps)) {              \
      ezctest_assertion_passed();                                              \
    } else {                                                                   \
      ezctest_assertion_failed(                                                \
          __FILE__, __LINE__, 0,                                               \
          "Expected: |%s - %s| <= %s\n  Actual: |%.15g - %.15g| = %.15g "      \
          "(epsilon: %.15g)",                                                  \
          #val1, #val2, #epsilon, ezctest_v1, ezctest_v2,                      \
          (ezctest_v1 > ezctest_v2 ? ezctest_v1 - ezctest_v2                   \
                                   : ezctest_v2 - ezctest_v1),                 \
          ezctest_eps);                                                        \
    }                                                                          \
  } while (0)

/* ============================================================================
 * 类型安全的值格式化辅助宏（C++/C11支持）
 * ========================================================================== */

#ifdef __cplusplus
/* C++版本：使用std::stringstream，支持operator<<重载 */

#ifdef __cplusplus
} /* extern "C" */
#endif

/**
 * @brief C++输出辅助：优先以指针形式显示
 * @note 避免NULL被当作整数输出导致的告警
 */
static inline void ezctest_stream_value(std::ostringstream *oss,
                                        const void *ptr) {
  (*oss) << ptr;
}

/**
 * @brief C++输出辅助：非const指针
 * @note 统一按指针形式输出
 */
template <typename T>
static inline void ezctest_stream_value(std::ostringstream *oss, T *ptr) {
  (*oss) << static_cast<const void *>(ptr);
}

/**
 * @brief C++输出辅助：const指针
 * @note 统一按指针形式输出
 */
template <typename T>
static inline void ezctest_stream_value(std::ostringstream *oss,
                                        const T *ptr) {
  (*oss) << static_cast<const void *>(ptr);
}

/**
 * @brief C++输出辅助：非指针类型
 */
template <typename T>
static inline void ezctest_stream_value(std::ostringstream *oss,
                                        const T &value) {
  (*oss) << value;
}

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief C++版本：格式化两个值的比较结果（支持operator<<重载）
 * @note 任何重载了operator<<的类型都可以自动显示
 */
#define EZCTEST_FORMAT_VALUES(op, val1, val2, buf, bufsize)                    \
  do {                                                                         \
    std::ostringstream ezctest_oss;                                            \
    ezctest_oss << "Expected: " << #val1 << " " op " " << #val2                \
                << "\n  Actual: ";                                             \
    ezctest_stream_value(&ezctest_oss, (val1));                                \
    ezctest_oss << " vs ";                                                     \
    ezctest_stream_value(&ezctest_oss, (val2));                                \
    snprintf(buf, bufsize, "%s", ezctest_oss.str().c_str());                   \
  } while (0)

#elif defined(EZCTEST_HAS_GENERIC)
/* C11版本：使用_Generic进行类型安全的格式化 */

/**
 * @brief 使用_Generic根据类型选择合适的格式化字符串
 * @note 仅在支持C11的编译器上可用
 */
#define EZCTEST_FORMAT_SPEC(val)                                               \
  _Generic((val),                                                              \
      char: "%d",                                                              \
      signed char: "%d",                                                       \
      unsigned char: "%u",                                                     \
      short: "%d",                                                             \
      unsigned short: "%u",                                                    \
      int: "%d",                                                               \
      unsigned int: "%u",                                                      \
      long: "%ld",                                                             \
      unsigned long: "%lu",                                                    \
      long long: "%lld",                                                       \
      unsigned long long: "%llu",                                              \
      float: "%g",                                                             \
      double: "%g",                                                            \
      long double: "%Lg",                                                      \
      char *: "\"%s\"",                                                        \
      const char *: "\"%s\"",                                                  \
      void *: "%p",                                                            \
      default: "%p")
/* C11版本：使用_Generic进行类型安全的格式化 */

/* 1) 先提供一组“值转换”辅助函数（返回类型要匹配 printf 的格式） */
static inline int ezctest_vcast_int(int v) { return v; }
static inline unsigned int ezctest_vcast_uint(unsigned int v) { return v; }
static inline long ezctest_vcast_long(long v) { return v; }
static inline unsigned long ezctest_vcast_ulong(unsigned long v) { return v; }
static inline long long ezctest_vcast_ll(long long v) { return v; }
static inline unsigned long long ezctest_vcast_ull(unsigned long long v) { return v; }

static inline double ezctest_vcast_float(float v) { return (double)v; }
static inline double ezctest_vcast_double(double v) { return v; }
static inline long double ezctest_vcast_ld(long double v) { return v; }

static inline char *ezctest_vcast_cstr(char *s) { return s; }
static inline const char *ezctest_vcast_ccstr(const char *s) { return s; }

static inline const void *ezctest_vcast_ptr(const void *p) { return p; }
/* 2) 用_Generic 选择“函数”，再把 val 传给它（避免指针触发 float 分支的非法转换） */
 #define EZCTEST_VALUE_CAST(val)                                                 \
 (_Generic((val),                                                              \
     char: ezctest_vcast_int,                                                  \
     signed char: ezctest_vcast_int,                                           \
     unsigned char: ezctest_vcast_uint,                                        \
     short: ezctest_vcast_int,                                                 \
     unsigned short: ezctest_vcast_uint,                                       \
     int: ezctest_vcast_int,                                                   \
     unsigned int: ezctest_vcast_uint,                                         \
     long: ezctest_vcast_long,                                                 \
     unsigned long: ezctest_vcast_ulong,                                       \
     long long: ezctest_vcast_ll,                                              \
     unsigned long long: ezctest_vcast_ull,                                    \
     float: ezctest_vcast_float,                                               \
     double: ezctest_vcast_double,                                             \
     long double: ezctest_vcast_ld,                                            \
     char *: ezctest_vcast_cstr,                                               \
     const char *: ezctest_vcast_ccstr,                                        \
     void *: ezctest_vcast_ptr,                                                \
     const void *: ezctest_vcast_ptr,                                          \
     default: ezctest_vcast_ptr                                                \
 )(val))

/**
 * @brief 格式化两个值的比较结果（带实际值显示）
 */
#define EZCTEST_FORMAT_VALUES(op, val1, val2, buf, bufsize)                    \
  do {                                                                         \
    const char *fmt1 = EZCTEST_FORMAT_SPEC(val1);                              \
    const char *fmt2 = EZCTEST_FORMAT_SPEC(val2);                              \
    char fmt_buf[256];                                                         \
    snprintf(fmt_buf, sizeof(fmt_buf),                                         \
             "Expected: %%s " op " %%s\n  Actual: %s vs %s", fmt1, fmt2);      \
    snprintf(buf, bufsize, fmt_buf, #val1, #val2, EZCTEST_VALUE_CAST(val1),    \
             EZCTEST_VALUE_CAST(val2));                                        \
  } while (0)

#else
/* C99及更早版本：回退到简单显示 */

/**
 * @brief 不支持_Generic时的回退版本（只显示表达式）
 */
#define EZCTEST_FORMAT_VALUES(op, val1, val2, buf, bufsize)                    \
  do {                                                                         \
    snprintf(buf, bufsize,                                                     \
             "Expected: %s " op " %s\n  Actual: different values", #val1,      \
             #val2);                                                           \
  } while (0)

#endif

/* ============================================================================
 * 宏定义 - ASSERT断言（致命）
 * ========================================================================== */

#define ASSERT_TRUE(condition)                                                 \
  do {                                                                         \
    if (condition) {                                                           \
      ezctest_assertion_passed();                                              \
    } else {                                                                   \
      ezctest_assertion_failed(__FILE__, __LINE__, 1,                          \
                               "Expected: (%s) is true\n  Actual: false",      \
                               #condition);                                    \
      if (g_ezctest_longjmp_ctx.has_jumped) {                                  \
        longjmp(g_ezctest_longjmp_ctx.jmp_env, 1);                             \
      }                                                                        \
      return;                                                                  \
    }                                                                          \
  } while (0)

#define ASSERT_FALSE(condition)                                                \
  do {                                                                         \
    if (!(condition)) {                                                        \
      ezctest_assertion_passed();                                              \
    } else {                                                                   \
      ezctest_assertion_failed(__FILE__, __LINE__, 1,                          \
                               "Expected: (%s) is false\n  Actual: true",      \
                               #condition);                                    \
      if (g_ezctest_longjmp_ctx.has_jumped) {                                  \
        longjmp(g_ezctest_longjmp_ctx.jmp_env, 1);                             \
      }                                                                        \
      return;                                                                  \
    }                                                                          \
  } while (0)

#define ASSERT_EQ(val1, val2)                                                  \
  do {                                                                         \
    if ((val1) == (val2)) {                                                    \
      ezctest_assertion_passed();                                              \
    } else {                                                                   \
      char ezctest_msg_buf[EZCTEST_MAX_MESSAGE_LENGTH];                        \
      EZCTEST_FORMAT_VALUES("==", val1, val2, ezctest_msg_buf,                 \
                            EZCTEST_MAX_MESSAGE_LENGTH);                       \
      ezctest_assertion_failed(__FILE__, __LINE__, 1, "%s", ezctest_msg_buf);  \
      if (g_ezctest_longjmp_ctx.has_jumped) {                                  \
        longjmp(g_ezctest_longjmp_ctx.jmp_env, 1);                             \
      }                                                                        \
      return;                                                                  \
    }                                                                          \
  } while (0)

#define ASSERT_NE(val1, val2)                                                  \
  do {                                                                         \
    if ((val1) != (val2)) {                                                    \
      ezctest_assertion_passed();                                              \
    } else {                                                                   \
      char ezctest_msg_buf[EZCTEST_MAX_MESSAGE_LENGTH];                        \
      EZCTEST_FORMAT_VALUES("!=", val1, val2, ezctest_msg_buf,                 \
                            EZCTEST_MAX_MESSAGE_LENGTH);                       \
      ezctest_assertion_failed(__FILE__, __LINE__, 1, "%s", ezctest_msg_buf);  \
      if (g_ezctest_longjmp_ctx.has_jumped) {                                  \
        longjmp(g_ezctest_longjmp_ctx.jmp_env, 1);                             \
      }                                                                        \
      return;                                                                  \
    }                                                                          \
  } while (0)

#define ASSERT_LT(val1, val2)                                                  \
  do {                                                                         \
    if ((val1) < (val2)) {                                                     \
      ezctest_assertion_passed();                                              \
    } else {                                                                   \
      char ezctest_msg_buf[EZCTEST_MAX_MESSAGE_LENGTH];                        \
      EZCTEST_FORMAT_VALUES("<", val1, val2, ezctest_msg_buf,                  \
                            EZCTEST_MAX_MESSAGE_LENGTH);                       \
      ezctest_assertion_failed(__FILE__, __LINE__, 1, "%s", ezctest_msg_buf);  \
      if (g_ezctest_longjmp_ctx.has_jumped) {                                  \
        longjmp(g_ezctest_longjmp_ctx.jmp_env, 1);                             \
      }                                                                        \
      return;                                                                  \
    }                                                                          \
  } while (0)

#define ASSERT_LE(val1, val2)                                                  \
  do {                                                                         \
    if ((val1) <= (val2)) {                                                    \
      ezctest_assertion_passed();                                              \
    } else {                                                                   \
      char ezctest_msg_buf[EZCTEST_MAX_MESSAGE_LENGTH];                        \
      EZCTEST_FORMAT_VALUES("<=", val1, val2, ezctest_msg_buf,                 \
                            EZCTEST_MAX_MESSAGE_LENGTH);                       \
      ezctest_assertion_failed(__FILE__, __LINE__, 1, "%s", ezctest_msg_buf);  \
      if (g_ezctest_longjmp_ctx.has_jumped) {                                  \
        longjmp(g_ezctest_longjmp_ctx.jmp_env, 1);                             \
      }                                                                        \
      return;                                                                  \
    }                                                                          \
  } while (0)

#define ASSERT_GT(val1, val2)                                                  \
  do {                                                                         \
    if ((val1) > (val2)) {                                                     \
      ezctest_assertion_passed();                                              \
    } else {                                                                   \
      char ezctest_msg_buf[EZCTEST_MAX_MESSAGE_LENGTH];                        \
      EZCTEST_FORMAT_VALUES(">", val1, val2, ezctest_msg_buf,                  \
                            EZCTEST_MAX_MESSAGE_LENGTH);                       \
      ezctest_assertion_failed(__FILE__, __LINE__, 1, "%s", ezctest_msg_buf);  \
      if (g_ezctest_longjmp_ctx.has_jumped) {                                  \
        longjmp(g_ezctest_longjmp_ctx.jmp_env, 1);                             \
      }                                                                        \
      return;                                                                  \
    }                                                                          \
  } while (0)

#define ASSERT_GE(val1, val2)                                                  \
  do {                                                                         \
    if ((val1) >= (val2)) {                                                    \
      ezctest_assertion_passed();                                              \
    } else {                                                                   \
      char ezctest_msg_buf[EZCTEST_MAX_MESSAGE_LENGTH];                        \
      EZCTEST_FORMAT_VALUES(">=", val1, val2, ezctest_msg_buf,                 \
                            EZCTEST_MAX_MESSAGE_LENGTH);                       \
      ezctest_assertion_failed(__FILE__, __LINE__, 1, "%s", ezctest_msg_buf);  \
      if (g_ezctest_longjmp_ctx.has_jumped) {                                  \
        longjmp(g_ezctest_longjmp_ctx.jmp_env, 1);                             \
      }                                                                        \
      return;                                                                  \
    }                                                                          \
  } while (0)

#define ASSERT_STREQ(str1, str2)                                               \
  do {                                                                         \
    if (strcmp((str1), (str2)) == 0) {                                         \
      ezctest_assertion_passed();                                              \
    } else {                                                                   \
      ezctest_assertion_failed(                                                \
          __FILE__, __LINE__, 1,                                               \
          "Expected: %s == %s\n  Actual: \"%s\" != \"%s\"", #str1, #str2,      \
          (str1), (str2));                                                     \
      if (g_ezctest_longjmp_ctx.has_jumped) {                                  \
        longjmp(g_ezctest_longjmp_ctx.jmp_env, 1);                             \
      }                                                                        \
      return;                                                                  \
    }                                                                          \
  } while (0)

#define ASSERT_STRNE(str1, str2)                                               \
  do {                                                                         \
    if (strcmp((str1), (str2)) != 0) {                                         \
      ezctest_assertion_passed();                                              \
    } else {                                                                   \
      ezctest_assertion_failed(                                                \
          __FILE__, __LINE__, 1,                                               \
          "Expected: %s != %s\n  Actual: both are \"%s\"", #str1, #str2,       \
          (str1));                                                             \
      if (g_ezctest_longjmp_ctx.has_jumped) {                                  \
        longjmp(g_ezctest_longjmp_ctx.jmp_env, 1);                             \
      }                                                                        \
      return;                                                                  \
    }                                                                          \
  } while (0)

#define ASSERT_NULL(ptr)                                                       \
  do {                                                                         \
    if ((ptr) == NULL) {                                                       \
      ezctest_assertion_passed();                                              \
    } else {                                                                   \
      ezctest_assertion_failed(__FILE__, __LINE__, 1,                          \
                               "Expected: %s is NULL\n  Actual: not NULL",     \
                               #ptr);                                          \
      if (g_ezctest_longjmp_ctx.has_jumped) {                                  \
        longjmp(g_ezctest_longjmp_ctx.jmp_env, 1);                             \
      }                                                                        \
      return;                                                                  \
    }                                                                          \
  } while (0)

#define ASSERT_NOT_NULL(ptr)                                                   \
  do {                                                                         \
    if ((ptr) != NULL) {                                                       \
      ezctest_assertion_passed();                                              \
    } else {                                                                   \
      ezctest_assertion_failed(__FILE__, __LINE__, 1,                          \
                               "Expected: %s is not NULL\n  Actual: NULL",     \
                               #ptr);                                          \
      if (g_ezctest_longjmp_ctx.has_jumped) {                                  \
        longjmp(g_ezctest_longjmp_ctx.jmp_env, 1);                             \
      }                                                                        \
      return;                                                                  \
    }                                                                          \
  } while (0)

/* ============================================================================
 * 宏定义 - ASSERT浮点数断言（致命）
 * ========================================================================== */

#define ASSERT_FLOAT_EQ(val1, val2)                                            \
  do {                                                                         \
    float ezctest_v1 = (float)(val1);                                          \
    float ezctest_v2 = (float)(val2);                                          \
    if (ezctest_float_eq(ezctest_v1, ezctest_v2, 1e-6f)) {                     \
      ezctest_assertion_passed();                                              \
    } else {                                                                   \
      ezctest_assertion_failed(                                                \
          __FILE__, __LINE__, 1,                                               \
          "Expected: %s == %s (float)\n  Actual: %g vs %g (diff: %g)", #val1,  \
          #val2, (double)ezctest_v1, (double)ezctest_v2,                       \
          (double)(ezctest_v1 - ezctest_v2));                                  \
      if (g_ezctest_longjmp_ctx.has_jumped) {                                  \
        longjmp(g_ezctest_longjmp_ctx.jmp_env, 1);                             \
      }                                                                        \
      return;                                                                  \
    }                                                                          \
  } while (0)

#define ASSERT_DOUBLE_EQ(val1, val2)                                           \
  do {                                                                         \
    double ezctest_v1 = (double)(val1);                                        \
    double ezctest_v2 = (double)(val2);                                        \
    if (ezctest_double_eq(ezctest_v1, ezctest_v2, 1e-10)) {                    \
      ezctest_assertion_passed();                                              \
    } else {                                                                   \
      ezctest_assertion_failed(__FILE__, __LINE__, 1,                          \
                               "Expected: %s == %s (double)\n  Actual: %.15g " \
                               "vs %.15g (diff: %.15g)",                       \
                               #val1, #val2, ezctest_v1, ezctest_v2,           \
                               (ezctest_v1 - ezctest_v2));                     \
      if (g_ezctest_longjmp_ctx.has_jumped) {                                  \
        longjmp(g_ezctest_longjmp_ctx.jmp_env, 1);                             \
      }                                                                        \
      return;                                                                  \
    }                                                                          \
  } while (0)

#define ASSERT_NEAR(val1, val2, epsilon)                                       \
  do {                                                                         \
    double ezctest_v1 = (double)(val1);                                        \
    double ezctest_v2 = (double)(val2);                                        \
    double ezctest_eps = (double)(epsilon);                                    \
    if (ezctest_double_eq(ezctest_v1, ezctest_v2, ezctest_eps)) {              \
      ezctest_assertion_passed();                                              \
    } else {                                                                   \
      ezctest_assertion_failed(                                                \
          __FILE__, __LINE__, 1,                                               \
          "Expected: |%s - %s| <= %s\n  Actual: |%.15g - %.15g| = %.15g "      \
          "(epsilon: %.15g)",                                                  \
          #val1, #val2, #epsilon, ezctest_v1, ezctest_v2,                      \
          (ezctest_v1 > ezctest_v2 ? ezctest_v1 - ezctest_v2                   \
                                   : ezctest_v2 - ezctest_v1),                 \
          ezctest_eps);                                                        \
      if (g_ezctest_longjmp_ctx.has_jumped) {                                  \
        longjmp(g_ezctest_longjmp_ctx.jmp_env, 1);                             \
      }                                                                        \
      return;                                                                  \
    }                                                                          \
  } while (0)

#define ASSERT_EMPTY(ptr, size)                                                \
  do {                                                                         \
    int is_empty = 1;                                                          \
    int i;                                                                     \
    for (i = 0; i < (int)size; ++i) {                                          \
      if (((char *)ptr)[i]) {                                                  \
        is_empty = 0;                                                          \
        break;                                                                 \
      }                                                                        \
    }                                                                          \
    if (is_empty) {                                                            \
      ezctest_assertion_passed();                                              \
    } else {                                                                   \
      ezctest_assertion_failed(__FILE__, __LINE__, 1,                          \
                               "Expected: %s is not empty\n", #ptr);           \
      if (g_ezctest_longjmp_ctx.has_jumped) {                                  \
        longjmp(g_ezctest_longjmp_ctx.jmp_env, 1);                             \
      }                                                                        \
      return;                                                                  \
    }                                                                          \
  } while (0)

#define ASSERT_NOT_EMPTY(ptr, size)                                            \
  do {                                                                         \
    int is_empty = 1;                                                          \
    int i;                                                                     \
    for (i = 0; i < (int)size; ++i) {                                          \
      if (((char *)ptr)[i]) {                                                  \
        is_empty = 0;                                                          \
        break;                                                                 \
      }                                                                        \
    }                                                                          \
    if (!is_empty) {                                                           \
      ezctest_assertion_passed();                                              \
    } else {                                                                   \
      ezctest_assertion_failed(__FILE__, __LINE__, 1,                          \
                               "Expected: %s is empty\n", #ptr);               \
      if (g_ezctest_longjmp_ctx.has_jumped) {                                  \
        longjmp(g_ezctest_longjmp_ctx.jmp_env, 1);                             \
      }                                                                        \
      return;                                                                  \
    }                                                                          \
  } while (0)

/* ============================================================================
 * Setup/Teardown 和 DEFER 宏
 * ========================================================================== */

/**
 * @brief 定义测试套件的Setup函数
 * @param suite_name 测试套件名称
 *
 * @details
 * Setup函数在测试套件中的每个测试执行前调用，用于初始化测试环境。
 *
 * 使用示例：
 * @code
 * SETUP(MyTestSuite) {
 *     // 初始化代码
 *     g_test_data = malloc(1024);
 * }
 * @endcode
 */
#if defined(_MSC_VER)
/* MSVC: 使用内存扫描机制 */
#define SETUP(suite_name)                                                      \
  static void ezctest_setup_##suite_name##_func(void);                         \
  static const ezctest_fixture_metadata_t ezctest_setup_##suite_name##_meta =  \
      {EZCTEST_FIXTURE_MAGIC, #suite_name, ezctest_setup_##suite_name##_func,  \
       NULL, 1};                                                               \
  static volatile const void *ezctest_keep_setup_##suite_name##_meta =         \
      &ezctest_setup_##suite_name##_meta;                                      \
  static void ezctest_setup_##suite_name##_func(void)
#elif defined(EZCTEST_GCC_NO_CONSTRUCTOR)
/* 老版本GCC: 使用.ctors段 */
#define SETUP(suite_name)                                                      \
  static void ezctest_setup_##suite_name##_func(void);                         \
  static void ezctest_setup_##suite_name##_init(void) {                        \
    ezctest_register_setup(#suite_name, ezctest_setup_##suite_name##_func);    \
  }                                                                            \
  static void (*ezctest_setup_##suite_name##_ctor_ptr)(void)                   \
      __attribute__((section(".ctors"), used)) =                               \
          ezctest_setup_##suite_name##_init;                                   \
  static void ezctest_setup_##suite_name##_func(void)
#else
/* 现代GCC/Clang: 使用constructor属性 */
#define SETUP(suite_name)                                                      \
  static void ezctest_setup_##suite_name##_func(void);                         \
  static void __attribute__((constructor)) ezctest_setup_##suite_name##_init(  \
      void) {                                                                  \
    ezctest_register_setup(#suite_name, ezctest_setup_##suite_name##_func);    \
  }                                                                            \
  static void ezctest_setup_##suite_name##_func(void)
#endif

/**
 * @brief 定义测试套件的Teardown函数
 * @param suite_name 测试套件名称
 *
 * @details
 * Teardown函数在测试套件中的每个测试执行后调用，用于清理测试环境。
 * 无论测试成功或失败，Teardown都会执行。
 *
 * 使用示例：
 * @code
 * TEARDOWN(MyTestSuite) {
 *     // 清理代码
 *     free(g_test_data);
 *     g_test_data = NULL;
 * }
 * @endcode
 */
#if defined(_MSC_VER)
/* MSVC: 使用内存扫描机制 */
#define TEARDOWN(suite_name)                                                   \
  static void ezctest_teardown_##suite_name##_func(void);                      \
  static const ezctest_fixture_metadata_t                                      \
      ezctest_teardown_##suite_name##_meta = {                                 \
          EZCTEST_FIXTURE_MAGIC, #suite_name, NULL,                            \
          ezctest_teardown_##suite_name##_func, 0};                            \
  static volatile const void *ezctest_keep_teardown_##suite_name##_meta =      \
      &ezctest_teardown_##suite_name##_meta;                                   \
  static void ezctest_teardown_##suite_name##_func(void)
#elif defined(EZCTEST_GCC_NO_CONSTRUCTOR)
/* 老版本GCC: 使用.ctors段 */
#define TEARDOWN(suite_name)                                                   \
  static void ezctest_teardown_##suite_name##_func(void);                      \
  static void ezctest_teardown_##suite_name##_init(void) {                     \
    ezctest_register_teardown(#suite_name,                                     \
                              ezctest_teardown_##suite_name##_func);           \
  }                                                                            \
  static void (*ezctest_teardown_##suite_name##_ctor_ptr)(void)                \
      __attribute__((section(".ctors"), used)) =                               \
          ezctest_teardown_##suite_name##_init;                                \
  static void ezctest_teardown_##suite_name##_func(void)
#else
/* 现代GCC/Clang: 使用constructor属性 */
#define TEARDOWN(suite_name)                                                   \
  static void ezctest_teardown_##suite_name##_func(void);                      \
  static void __attribute__((constructor))                                     \
  ezctest_teardown_##suite_name##_init(void) {                                 \
    ezctest_register_teardown(#suite_name,                                     \
                              ezctest_teardown_##suite_name##_func);           \
  }                                                                            \
  static void ezctest_teardown_##suite_name##_func(void)
#endif

/**
 * @brief 注册DEFER清理回调
 * @param func 清理函数指针
 * @param data 传递给清理函数的数据
 *
 * @details
 * DEFER类似Go语言的defer，注册的清理函数会在测试结束时自动执行（LIFO顺序）。
 * 无论测试成功或失败，DEFER的清理函数都会执行。
 * 这是最灵活的清理机制，适合复杂的资源管理场景。
 *
 * 使用示例：
 * @code
 * TEST(MyTest, ResourceManagement) {
 *     char* buffer = malloc(1024);
 *     DEFER(free, buffer);  // 注册清理
 *
 *     FILE* fp = fopen("test.txt", "r");
 *     DEFER((ezctest_cleanup_func_t)fclose, fp);  // 再注册一个
 *
 *     ASSERT_NOT_NULL(buffer);  // 即使这里失败
 *     // fclose和free也会按LIFO顺序被调用
 * }
 * @endcode
 */
#define DEFER(func, data)                                                      \
  do {                                                                         \
    if (!ezctest_defer_add((ezctest_cleanup_func_t)(func), (void *)(data))) {  \
      fprintf(stderr, "Warning: DEFER stack full at %s:%d\n", __FILE__,        \
              __LINE__);                                                       \
    }                                                                          \
  } while (0)

/* ============================================================================
 * 主入口宏
 * ========================================================================== */

/**
 * @brief 运行所有测试的主入口
 * @param argc 命令行参数个数
 * @param argv 命令行参数数组
 * @return 测试通过返回0，失败返回1
 *
 * @details
 * 此宏智能适配所有平台和字符编码：
 * - Windows + _UNICODE：自动使用宽字符版本（wchar_t*）
 * - Windows + ANSI：使用多字节字符版本（char*）
 * - 其他平台：使用标准版本（char*）
 *
 * 使用示例：
 * @code
 * // Windows Unicode 程序
 * int wmain(int argc, wchar_t* argv[]) {
 *     return RUN_ALL_TESTS(argc, argv);  // 自动使用宽字符版本
 * }
 *
 * // Windows TCHAR 程序
 * int _tmain(int argc, TCHAR* argv[]) {
 *     return RUN_ALL_TESTS(argc, argv);  // 根据_UNICODE自动选择
 * }
 *
 * // 标准 main 函数（所有平台）
 * int main(int argc, char* argv[]) {
 *     return RUN_ALL_TESTS(argc, argv);  // 使用标准版本
 * }
 * @endcode
 */
#if defined(EZCTEST_PLATFORM_WINDOWS) && defined(_UNICODE)
#define RUN_ALL_TESTS(argc, argv) ezctest_run_all_tests_w(argc, argv)
#else
#define RUN_ALL_TESTS(argc, argv) ezctest_run_all_tests(argc, argv)
#endif

/* ============================================================================
 * VC6 特殊支持：段控制辅助宏
 * ========================================================================== */

#if defined(_MSC_VER) && _MSC_VER < 1300

/**
 * @brief VC6 段控制辅助宏
 *
 * @details VC6 的技术限制说明：
 * 1. #pragma 不能在宏中使用（预处理器限制）
 * 2. _Pragma 是 C99 特性，VC6 不支持
 * 3. 内联汇编 __asm 中无法使用 ## 标记拼接
 *
 * **解决方案**：采用 "一次性段切换" 模式
 * - 将所有 TEST 定义放在一起
 * - 在所有 TEST 之后调用一次 EZCTEST_VC6_RESTORE_SEGMENT
 * - 然后再写 main 和其他函数
 *
 * 这样只需要一次段切换，不需要反复添加 pragma。
 */

/* VC6 专用：恢复代码段（只需在所有 TEST 定义之后调用一次） */
#if defined(_MSC_VER) && _MSC_VER < 1300
/* 这是一个空宏，但提醒用户在此位置之前添加 #pragma data_seg() */
#define EZCTEST_VC6_RESTORE_SEGMENT /* 在此宏之前的一行添加: #pragma         \
                                        data_seg() */
#else
#define EZCTEST_VC6_RESTORE_SEGMENT
#endif

/**
 * @brief VC6 使用说明（最简化版本）
 *
 * @details
 * **推荐模式**：将所有 TEST 放在一起，只需一次段恢复
 *
 * @code
 * #define EZCTEST_IMPLEMENTATION
 * #include "ezctest.h"
 *
 * // 1. 定义所有测试（函数指针在 .CRT$XCU 段，函数声明已生成）
 * TEST(MyTest, Case1) {
 *     EXPECT_EQ(1, 1);
 * }
 *
 * TEST(MyTest, Case2) {
 *     EXPECT_EQ(2, 2);
 * }
 *
 * // 2. VC6 必需：恢复代码段（让后续函数实现在正确的段）
 * #if defined(_MSC_VER) && _MSC_VER < 1300
 * #pragma data_seg()
 * #endif
 * EZCTEST_VC6_RESTORE_SEGMENT  // 此宏用于标记位置，方便查找
 *
 * // 3. 正常写 main 和其他代码
 * int main(int argc, char* argv[]) {
 *     return RUN_ALL_TESTS(argc, argv);
 * }
 * @endcode
 *
 * **关键点**：
 * - 文件开头的 ezctest.h 已经设置了 #pragma data_seg(".CRT$XCU")
 * - 所有 TEST 宏生成的函数指针会自动在 .CRT$XCU 段
 * - 在 TEST 定义结束后，用 #pragma data_seg() 恢复段
 * - 这样函数实现就在正确的代码段，而不是数据段
 *
 * **注意**：VC7+ 和 GCC 不需要手动段控制，完全自动化。
 */

#else

/* 非 VC6 编译器：无需特殊处理 */

#endif

/**
 * @brief STM32交互式模式入口
 */
#ifdef EZCTEST_STM32_INTERACTIVE
#define RUN_TESTS_INTERACTIVE() ezctest_stm32_interactive_loop()
#endif

#ifdef EZCTEST_IMPLEMENTATION_MAIN
int main(int argc, char **argv) {
#ifdef _WIN32
/* 设置控制台代码页为UTF-8，解决中文乱码问题 */
/* VC6 的 SDK 可能不支持，需要动态加载 */
#if defined(_MSC_VER) && _MSC_VER >= 1300 /* VC7+ */
  SetConsoleOutputCP(CP_UTF8);
#endif
#endif
  return RUN_ALL_TESTS(argc, argv);
}
#endif

EZCTEST_DISABLE_UNUSED_FUNCTION_END

#ifdef __cplusplus
}
#endif

/* VC6: 恢复默认段设置 */
#if defined(_MSC_VER) && _MSC_VER < 1300
#pragma data_seg()
#pragma code_seg()
#endif

#endif /* EZCTEST_H */
