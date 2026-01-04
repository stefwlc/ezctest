<div align="center">

# 🚀 EZCTest - Extremely Simple C/C++ Unit Testing Framework

**A single-header framework that makes testing simple and elegant**

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE.txt)
[![C99](https://img.shields.io/badge/standard-C99-green.svg)](https://en.wikipedia.org/wiki/C99)
[![C++](https://img.shields.io/badge/C%2B%2B-compatible-orange.svg)](https://en.cppreference.com/w/)
[![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20Linux%20%7C%20macOS%20%7C%20STM32-lightgrey.svg)](readme_ch.md)
[![Version](https://img.shields.io/badge/version-3.2.0-brightgreen.svg)](ezctest.h)

**English | [简体中文](readme_ch.md)**

</div>

## 🌟 Framework Overview

**EZCTest** is a truly **cross-platform, single-header, all-compiler-compatible** C/C++ unit testing framework. It breaks through the limitations of traditional testing frameworks, spanning from Visual Studio 6.0 (1998) to the latest compilers of 2026, from Windows XP to modern Linux/macOS, from desktop applications to STM32 embedded systems — **just one header file to start testing in any environment**. This is not just a testing tool, but a testing solution tempered through 15+ years of engineering practice, embodying modern software engineering principles. Whether you're dealing with legacy codebases or cutting-edge projects, EZCTest provides reliable testing support in the same elegant way.

---

## ✨ Why Choose EZCTest?

Among numerous C testing frameworks, **EZCTest** stands out for:

- 🎯 **Extremely Simple** - Just include one header file, start testing with 5 lines of code
- 💪 **Powerful Features** - Complete assertion system comparable to Google Test
- 🔄 **Resource Safety** - Innovative four-layer resource protection mechanism, completely avoiding leaks
- 🌍 **Unparalleled Compatibility** - Full coverage of 21 compiler environments (VC6 to VS2026, GCC 3.2.3+)
- ⚡ **Zero Dependencies** - Pure C99 implementation, no external libraries required
- 🎨 **Modern Experience** - Colored output, filters, interactive mode
- 🔬 **Embedded-Friendly** - Deeply optimized STM32 support and interactive debugging
- 🛡️ **Production-Grade Quality** - Incorporating the author's 15+ years of practical engineering experience

> 💡 **This is not just a testing framework, but a work of art that makes C/C++ testing elegant.**

---

## 🎬 Quick Start

### 5 Lines of Code to Begin Your Testing Journey

```c
#define EZCTEST_IMPLEMENTATION  // This line only needs to appear once in one C or CPP file in your project
#include "ezctest.h"

TEST(HelloWorld, FirstTest) {
    EXPECT_EQ(1 + 1, 2);
}

int main(int argc, char* argv[]) {
    return RUN_ALL_TESTS(argc, argv);
}
```

编译运行：
```bash
gcc test.c -o test && ./test
```

输出：
```sh
[==========] Running 1 test(s)
[ RUN      ] HelloWorld.FirstTest
[       OK ] HelloWorld.FirstTest (0 ms)
[==========] 1 test(s) ran (0 ms total)
[  PASSED  ] 1 test(s)
```

**就这么简单！** 坦白说，这几行代码足以应对我整个职业生涯中 99% 的单元测试场景。

---

## 🌟 核心特性

### 1️⃣ 完整的断言体系

类似 Google Test 的丰富断言，让测试表达更清晰：

```c
// 基本比较
EXPECT_EQ(actual, expected);      // 相等
EXPECT_NE(a, b);                  // 不等
EXPECT_LT(a, b);                  // 小于
EXPECT_LE(a, b);                  // 小于等于
EXPECT_GT(a, b);                  // 大于
EXPECT_GE(a, b);                  // 大于等于

// 布尔断言
EXPECT_TRUE(condition);
EXPECT_FALSE(condition);

// 字符串断言
EXPECT_STREQ("hello", str);       // 字符串相等
EXPECT_STRNE("hello", "world");   // 字符串不等

// 指针断言
EXPECT_NULL(ptr);
EXPECT_NOT_NULL(ptr);

// 浮点数断言（处理精度问题）
EXPECT_FLOAT_EQ(0.1f + 0.2f, 0.3f);
EXPECT_DOUBLE_EQ(result, 3.14159265358979);
EXPECT_NEAR(measured, 100.0, 0.5);  // 允许误差
```

**EXPECT vs ASSERT**：
- `EXPECT_*` - 失败后继续执行（适合多个检查点）
- `ASSERT_*` - 失败后立即返回（适合前置条件）

### 2️⃣ 独创的四层资源保护机制

这是 EZCTest 最引以为傲的创新！确保**任何情况下资源都不会泄漏**：

```c
TEST(ResourceSafe, CompleteProtection) {
    // 第一层：DEFER 清理回调（类似 Go 的 defer）
    char* buffer = malloc(1024);
    DEFER(free, buffer);
    
    FILE* fp = fopen("test.txt", "r");
    DEFER((ezctest_cleanup_func_t)fclose, fp);
    
    // 第二层：longjmp 异常处理
    ASSERT_NOT_NULL(fp);  // 即使失败，DEFER 也会执行
    
    // 第三层：C++ 异常捕获 / Windows SEH 捕获
    // 自动处理异常，确保清理
    
    // 第四层：多进程隔离
    // 进程崩溃也能自动回收资源
}
```

**四层防护对比表**：

| 场景 | DEFER | longjmp | 异常捕获 | 进程隔离 | 结果 |
|------|-------|---------|----------|----------|------|
| 正常通过 | ✅ | - | - | - | ✅ 资源释放 |
| EXPECT 失败 | ✅ | - | - | - | ✅ 资源释放 |
| ASSERT 失败 | ✅ | ✅ | - | - | ✅ 资源释放 |
| C++ 异常 | ✅ | ✅ | ✅ | - | ✅ 资源释放 |
| SEH 异常 | ✅ | ✅ | ✅ | - | ✅ 资源释放 |
| 进程崩溃 | - | - | - | ✅ | ✅ OS 回收 |

> 📌 **无论什么情况，资源都有保护机制！**

### 3️⃣ Setup/Teardown 测试夹具

类似 Google Test 的 Fixture，自动管理测试环境：

```c
// 每个测试前自动调用
SETUP(DatabaseTest) {
    init_database();
    connect_to_db();
}

// 每个测试后自动调用（无论成功或失败）
TEARDOWN(DatabaseTest) {
    disconnect_from_db();
    cleanup_database();
}

TEST(DatabaseTest, InsertUser) {
    // Setup 已自动执行
    User user = {"Alice", 25};
    EXPECT_TRUE(insert_user(&user));
    // Teardown 会自动执行
}

TEST(DatabaseTest, QueryUser) {
    // 每个测试都有独立的 Setup/Teardown
    EXPECT_NOT_NULL(query_user("Alice"));
}
```

### 4️⃣ 多进程隔离

每个测试在独立进程中运行，彻底隔离崩溃影响：

```c
TEST(CrashTest, SegmentationFault) {
    int* ptr = NULL;
    *ptr = 42;  // 段错误！
    // 不会影响其他测试
}

TEST(NormalTest, StillRuns) {
    // 即使上面的测试崩溃了，这个测试仍然会运行
    EXPECT_EQ(2 + 2, 4);
}
```

**智能决策**：
- ✅ 调试模式：自动禁用（方便调试）
- ✅ 单个测试：自动禁用（无需隔离）
- ✅ 多个测试：自动启用（最强保护）

**平台支持**：
- Linux：`fork()` 实现
- Windows：`CreateProcess()` 实现
- STM32：不支持（单片机无进程）

### 5️⃣ 强大的命令行功能

```bash
# 运行所有测试
./test

# 过滤测试（支持通配符）
./test --ctest_filter=Math.*
./test --ctest_filter=*Fast*
./test --ctest_filter=*:-*Slow*  # 排除慢速测试

# 重复运行（检测不稳定测试）
./test --ctest_repeat=100

# 随机顺序（检测测试依赖）
./test --ctest_shuffle

# 列出所有测试
./test --ctest_list_tests

# 彩色输出控制
./test --ctest_color=yes

# 禁用进程隔离（调试用）
./test --ezctest_no_exec
```

### 6️⃣ STM32 嵌入式支持

专为嵌入式优化的交互式测试模式：

```c
#define EZCTEST_STM32_MODE
#define EZCTEST_STM32_INTERACTIVE
#define EZCTEST_IMPLEMENTATION
#include "ezctest.h"

int main(void) {
    HAL_Init();
    SystemClock_Config();
    MX_USART1_UART_Init();  // printf 重定向
    
    RUN_TESTS_INTERACTIVE();  // 交互式命令行
    
    while (1) {}
}
```

**串口终端交互**：
```
EZCTest Interactive Mode
Type 'help' for commands

> run              # 运行所有测试
> run GPIO*        # 运行匹配的测试
> list             # 列出所有测试
> repeat 5         # 重复 5 次
> help             # 显示帮助
> exit             # 退出
```

---

## 🏆 无与伦比的编译器支持

### Microsoft Visual Studio 全系列

EZCTest 是**唯一**支持 Visual Studio 完整历史版本的 C 测试框架：

| 编译器 | 版本 | 发布年份 | 架构支持 | 状态 |
|--------|------|----------|----------|------|
| Visual Studio 6.0 | VC6 | 1998 | x86 | ✅ **完美支持** |
| Visual Studio .NET 2002 | VC7.0 | 2002 | x86 | ✅ 完美支持 |
| Visual Studio .NET 2003 | VC7.1 | 2003 | x86 | ✅ 完美支持 |
| Visual Studio 2005 | VC8.0 | 2005 | x86 | ✅ 完美支持 |
| Visual Studio 2008 | VC9.0 | 2008 | x86 | ✅ 完美支持 |
| Visual Studio 2010 | VC10.0 | 2010 | x86 | ✅ 完美支持 |
| Visual Studio 2012 | VC11.0 | 2012 | x86/x64 | ✅ 完美支持 |
| Visual Studio 2013 | VC12.0 | 2013 | x86/x64 | ✅ 完美支持 |
| Visual Studio 2015 | VC14.0 | 2015 | x86/x64 | ✅ 完美支持 |
| Visual Studio 2017 | VC15.0 | 2017 | x86/x64 | ✅ 完美支持 |
| Visual Studio 2019 | VC16.0 | 2019 | x86/x64 | ✅ 完美支持 |
| Visual Studio 2022 | VC17.0 | 2022 | x86/x64 | ✅ 完美支持 |
| Visual Studio 2026 | VC18.0 | 2026 | x86/x64 | ✅ 完美支持 |

> 🎯 **Comprehensive Verification**: Based on user feedback, verified with 945 test cases across **21 compiler environments** (including x86 and x64), zero warnings, zero errors!
>
> 🎓 **Special Note**: Visual Studio 6.0 is a classic version that accompanied my entire college years. Those were challenging but wonderful memories. Since VC6 doesn't support many modern compiler features (such as `#pragma` in macros), I spent considerable time implementing automatic test registration using an innovative **Magic Number + Memory Scanning Mechanism**. This is a technical breakthrough, and I haven't seen other testing frameworks work so elegantly on VC6.

### 其他编译器

| 编译器 | 最低版本 | 状态 |
|--------|----------|------|
| GCC | 3.2.3+ | ✅ 完美支持 |
| Clang | 3.0+ | ✅ 完美支持 |
| MinGW | 3.2.3+ | ✅ 完美支持 |
| ARM GCC (STM32) | 任意 | ✅ 完美支持 |
| Intel C++ | 11.0+ | ✅ 支持 |
| TinyCC | 0.9.27+ | ✅ 支持 |

> 💡 **Classic Compilers**: Based on user feedback, verified on GCC 3.2.3 (2003) and MinGW-GCC 3.2.3, achieving perfect support for classic compilers.

### Platform Support

| Platform | Architecture | Status |
|----------|--------------|--------|
| Windows Vista+ | x86/x64 | ✅ Perfect Support |
| Windows XP | x86 | ✅ Supported (Requires VC6-2013) |
| Linux | x86/x64/ARM | ✅ Perfect Support |
| macOS | x64/ARM64 | ✅ Perfect Support |
| STM32 | ARM Cortex-M | ✅ Deeply Optimized |
| Other Embedded | ARM/RISC-V | ✅ Supported |

> 💡 **Platform Compatibility**: Based on user feedback, perfectly runs on all the above platforms.

---

## 📊 与其他框架对比

| 特性 | **EZCTest** | Google Test | Catch2 | Unity | CMocka |
|------|-------------|-------------|--------|-------|--------|
| **语言** | C99/C++ | C++11 | C++11 | C89 | C99 |
| **文件数** | 1 个头文件 | 多文件库 | 1 个头文件 | 2 个文件 | 多文件 |
| **依赖** | 零依赖 | 需链接 | 零依赖 | 零依赖 | 需链接 |
| **Setup/Teardown** | ✅ | ✅ | ✅ | ✅ | ✅ |
| **DEFER 清理** | ✅ 独有 | ❌ | ❌ | ❌ | ❌ |
| **多进程隔离** | ✅ | ✅ | ❌ | ❌ | ❌ |
| **C++ 异常捕获** | ✅ | ✅ | ✅ | ❌ | ❌ |
| **Windows SEH** | ✅ 独有 | ❌ | ❌ | ❌ | ❌ |
| **浮点数断言** | ✅ | ✅ | ✅ | ✅ | ❌ |
| **过滤器/通配符** | ✅ | ✅ | ✅ | ❌ | ❌ |
| **彩色输出** | ✅ | ✅ | ✅ | ❌ | ✅ |
| **STM32 支持** | ✅ 深度优化 | ❌ | ❌ | ✅ | ❌ |
| **交互式模式** | ✅ 独有 | ❌ | ❌ | ❌ | ❌ |
| **VC6 支持** | ✅ 独有 | ❌ | ❌ | ✅ | ❌ |
| **老编译器** | ✅ | ❌ | ❌ | ✅ | ❌ |
| **内存占用** | 极小 | 大 | 大 | 极小 | 小 |

**总结**：
- ✅ **最完整**的资源保护机制（四层防护）
- ✅ **最广泛**的编译器支持（21 个环境验证：VC6 到 VS2026，GCC 3.2.3+）
- ✅ **最灵活**的使用方式（单文件、零依赖）
- ✅ **最友好**的嵌入式支持（STM32 交互式）
- ✅ **最可靠**的质量保证（945 个测试用例，零警告零错误）

---

## 📚 Complete Documentation

| Document | Description |
|----------|-------------|
| 💾 **[Example Code](demo.c)** | Complete examples |
| 🌐 **[中文 README](readme_ch.md)** | Chinese version |

---

## 🎯 Typical Use Cases

### Desktop Applications
```c
// Single-file testing
#define EZCTEST_IMPLEMENTATION
#include "ezctest.h"

TEST(Application, UserLogin) {
    User* user = create_user("alice", "pass123");
    DEFER(destroy_user, user);
    
    ASSERT_NOT_NULL(user);
    EXPECT_TRUE(login(user));
}
```

### Embedded Systems
```c
// STM32 hardware testing
TEST(Hardware, GPIOToggle) {
    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
    EXPECT_EQ(GPIO_PIN_SET, HAL_GPIO_ReadPin(LED_GPIO_Port, LED_Pin));
}
```

### System Programming
```c
// Multi-threading tests
TEST(Threading, ConcurrentAccess) {
    pthread_mutex_t* lock = create_mutex();
    DEFER(destroy_mutex, lock);
    
    ASSERT_NOT_NULL(lock);
    EXPECT_EQ(0, pthread_mutex_lock(lock));
}
```

### CI/CD Integration
```yaml
# GitHub Actions
- name: Run Tests
  run: |
    ./build/test --ctest_color=yes
    ./build/test --ctest_filter=*:-*Slow*
```

---

## 💡 Design Philosophy

EZCTest's design follows these principles:

1. **Extremely Simple** - "Include one header and use" is not a slogan, but a promise
2. **Zero Surprises** - Intuitive API design, similar to Google Test, zero learning cost
3. **Resource Safety First** - Innovative four-layer protection, making resource leaks history
4. **Backward Compatibility** - Support for compilers from 25 years ago, cherishing every legacy codebase
5. **Battle-Tested** - Proven through 10+ years of production environments, trustworthy

> 💬 **"Making C testing as modern as C++, yet as simple and reliable as C itself."**  
> — wanglanchun

---

## 🚀 快速开始

### 1. 下载头文件

只需下载 `ezctest.h` 到你的项目中。

### 2. 编写测试

```c
#define EZCTEST_IMPLEMENTATION
#include "ezctest.h"

TEST(Math, Addition) {
    EXPECT_EQ(1 + 1, 2);
    EXPECT_EQ(10 + 20, 30);
}

TEST(String, Comparison) {
    EXPECT_STREQ("hello", "hello");
    EXPECT_STRNE("hello", "world");
}

int main(int argc, char* argv[]) {
    return RUN_ALL_TESTS(argc, argv);
}
```

### 3. 编译运行

**Windows (MSVC):**
```cmd
cl /TC test.c
test.exe
```

**Windows (MinGW) / Linux:**
```bash
gcc -std=c99 test.c -o test
./test
```

**macOS:**
```bash
clang -std=c99 test.c -o test
./test
```

### 4. Windows Unicode Support

For Windows Unicode programs (using `wmain` or `_tmain`), EZCTest provides fully automatic character encoding support:

**Smart Adaptation - Just Use RUN_ALL_TESTS:**
```cpp
#define EZCTEST_IMPLEMENTATION
#include "ezctest.h"

TEST(Math, Addition) {
    EXPECT_EQ(1 + 1, 2);
}

// Standard main function - works on all platforms
int main(int argc, char* argv[]) {
    return RUN_ALL_TESTS(argc, argv);  // Auto uses multibyte version
}

// Windows Unicode (wmain) - auto detected
int wmain(int argc, wchar_t* argv[]) {
    return RUN_ALL_TESTS(argc, argv);  // Auto uses wide character version
}

// Windows TCHAR (_tmain) - auto adapts
int _tmain(int argc, TCHAR* argv[]) {
    return RUN_ALL_TESTS(argc, argv);  // Auto selects based on _UNICODE
}
```

**Compilation:**
```cmd
# Unicode version (_UNICODE defined)
cl /D_UNICODE /DUNICODE test.cpp
test.exe --filter=Math*

# ANSI version (default)
cl test.c
test.exe --filter=Math*
```

**RUN_ALL_TESTS Smart Adaptation Rules:**

| Platform | _UNICODE Macro | Parameter Type | Auto Selection |
|----------|----------------|----------------|----------------|
| Windows | Defined | `wchar_t*` / `TCHAR*` | ✅ Wide character version |
| Windows | Not defined | `char*` / `TCHAR*` | ✅ Multibyte version |
| Linux/macOS | N/A | `char*` | ✅ Standard version |

> 💡 **Core Advantage**: `RUN_ALL_TESTS` is the only macro you need to remember! It automatically adapts based on platform and compilation options:
> - On Windows with `_UNICODE` defined: automatically uses wide character version
> - Other cases: uses standard multibyte character version
> - No manual selection needed, no multiple macros to remember, one macro for all scenarios

**Optional: Explicitly Specify Wide Character Version (Not Recommended):**
```cpp
// If you really need to explicitly specify (usually unnecessary)
int wmain(int argc, wchar_t* argv[]) {
    return RUN_ALL_TESTS_W(argc, argv);  // Explicitly use wide character
}
```

---

## 📈 性能特点

- ⚡ **轻量级** - 单个测试开销 < 1 微秒
- 💾 **低内存** - 每个测试仅占用 32 字节
- 🔥 **快速编译** - 单头文件，编译速度极快
- 📉 **零运行时开销** - 资源保护机制仅在需要时激活

---

## 📄 许可证

本项目采用 **MIT 许可证** 开源。

- ✅ 商业使用
- ✅ 修改
- ✅ 分发
- ✅ 私有使用

详见 [LICENSE.txt](LICENSE.txt) 和 [LICENSE_CN.txt](LICENSE_CN.txt)

---

## 🤝 贡献与反馈

我们欢迎：
- 🐛 Bug 报告
- 💡 功能建议
- 📖 文档改进
- 🌟 Star 支持

---

## 🙏 Acknowledgments
- Thanks to all users for their feedback and suggestions
- You can contact me directly via email: 1390wlc@163.com

---

<div align="center">

**用 EZCTest，让测试成为一种享受！**

🚀 [开始使用](doc/ezctest_quick_start.md) | 📖 [阅读文档](doc/ezctest_README.md) | 💬 [提交反馈](https://github.com)

---

**Made with ❤️ by wanglanchun**

*Integrating over a decade of engineering experience, running stably in numerous production environments*

</div>
