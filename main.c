/**
 * @file main.c
 * @brief ezctest.h 完整示例 - 展示所有宏的用法
 * @details 这个文件演示了 ezctest 测试框架的所有功能
 */

/* 定义实现宏，必须在某个.c文件中定义一次 */
#define EZCTEST_IMPLEMENTATION
#include "ezctest.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 安全函数包装宏，消除 C4996 警告 */
/* VC8 (VS2005) 及以上版本支持 _s 安全函数 */
#if defined(_MSC_VER) && _MSC_VER >= 1400
  #define SAFE_STRCPY(dst, size, src) strcpy_s(dst, size, src)
  #define SAFE_FOPEN(fp, filename, mode) fopen_s(&fp, filename, mode)
#else
  /* VC6, VC7, GCC 等使用标准函数 */
  #define SAFE_STRCPY(dst, size, src) strcpy(dst, src)
  #define SAFE_FOPEN(fp, filename, mode) fp = fopen(filename, mode)
#endif

/* ============================================================================
 * 全局变量和辅助函数示例
 * ========================================================================== */

/* 用于演示 Setup/Teardown 的全局变量 */
static int *g_test_data = NULL;
static int g_setup_count = 0;
static int g_teardown_count = 0;

/* 辅助函数：用于演示 DEFER 清理 */
static void cleanup_buffer(void *ptr) {
    if (ptr) {
        printf("  [DEFER] 清理缓冲区: %p\n", ptr);
        free(ptr);
    }
}

/* 辅助函数：用于演示 DEFER 清理文件 */
static void cleanup_file(void *ptr) {
    if (ptr) {
        fclose((FILE *)ptr);
        printf("  [DEFER] 关闭文件\n");
    }
}

/* ============================================================================
 * 基础断言测试 - EXPECT 系列（非致命）
 * ========================================================================== */

TEST(BasicAssertions, ExpectTrue) {
    /* EXPECT_TRUE: 期望条件为真 */
    EXPECT_TRUE(1 == 1);
    EXPECT_TRUE(5 > 3);
}

TEST(BasicAssertions, ExpectFalse) {
    /* EXPECT_FALSE: 期望条件为假 */
    EXPECT_FALSE(1 == 2);
    EXPECT_FALSE(3 > 5);
}

TEST(BasicAssertions, ExpectEQ) {
    /* EXPECT_EQ: 期望两个值相等 */
    int a = 42;
    int b = 42;
    EXPECT_EQ(a, b);
    EXPECT_EQ(100, 100);
}

TEST(BasicAssertions, ExpectNE) {
    /* EXPECT_NE: 期望两个值不相等 */
    int a = 42;
    int b = 43;
    EXPECT_NE(a, b);
    EXPECT_NE(100, 200);
}

TEST(BasicAssertions, ExpectLT) {
    /* EXPECT_LT: 期望第一个值小于第二个值 */
    EXPECT_LT(5, 10);
    EXPECT_LT(-1, 0);
}

TEST(BasicAssertions, ExpectLE) {
    /* EXPECT_LE: 期望第一个值小于或等于第二个值 */
    EXPECT_LE(5, 10);
    EXPECT_LE(10, 10);
}

TEST(BasicAssertions, ExpectGT) {
    /* EXPECT_GT: 期望第一个值大于第二个值 */
    EXPECT_GT(10, 5);
    EXPECT_GT(0, -1);
}

TEST(BasicAssertions, ExpectGE) {
    /* EXPECT_GE: 期望第一个值大于或等于第二个值 */
    EXPECT_GE(10, 5);
    EXPECT_GE(10, 10);
}

/* ============================================================================
 * 字符串断言测试
 * ========================================================================== */

TEST(StringAssertions, ExpectStrEQ) {
    /* EXPECT_STREQ: 期望两个字符串相等 */
    const char *str1 = "hello";
    const char *str2 = "hello";
    EXPECT_STREQ(str1, str2);
    EXPECT_STREQ("test", "test");
}

TEST(StringAssertions, ExpectStrNE) {
    /* EXPECT_STRNE: 期望两个字符串不相等 */
    const char *str1 = "hello";
    const char *str2 = "world";
    EXPECT_STRNE(str1, str2);
    EXPECT_STRNE("test", "demo");
}

/* ============================================================================
 * 指针断言测试
 * ========================================================================== */

TEST(PointerAssertions, ExpectNull) {
    /* EXPECT_NULL: 期望指针为 NULL */
    void *ptr = NULL;
    EXPECT_NULL(ptr);
}

TEST(PointerAssertions, ExpectNotNull) {
    /* EXPECT_NOT_NULL: 期望指针不为 NULL */
    int value = 42;
    int *ptr = &value;
    EXPECT_NOT_NULL(ptr);
}

/* ============================================================================
 * 内存块断言测试
 * ========================================================================== */

TEST(MemoryAssertions, ExpectEmpty) {
    /* EXPECT_EMPTY: 期望内存块全为零 */
    char buffer[10];
    memset(buffer, 0, sizeof(buffer));
    EXPECT_EMPTY(buffer, sizeof(buffer));
}

TEST(MemoryAssertions, ExpectNotEmpty) {
    /* EXPECT_NOT_EMPTY: 期望内存块不全为零 */
    char buffer[10];
    memset(buffer, 0, sizeof(buffer));
    buffer[5] = 'X';
    EXPECT_NOT_EMPTY(buffer, sizeof(buffer));
}

/* ============================================================================
 * 浮点数断言测试
 * ========================================================================== */

TEST(FloatAssertions, ExpectFloatEQ) {
    /* EXPECT_FLOAT_EQ: 期望两个 float 值近似相等（默认精度 1e-6f） */
    float a = 3.14159f;
    float b = 3.14159f;
    EXPECT_FLOAT_EQ(a, b);
    EXPECT_FLOAT_EQ(1.0f / 3.0f, 0.333333f);
}

TEST(FloatAssertions, ExpectDoubleEQ) {
    /* EXPECT_DOUBLE_EQ: 期望两个 double 值近似相等（默认精度 1e-10） */
    double a = 3.141592653589793;
    double b = 3.141592653589793;
    EXPECT_DOUBLE_EQ(a, b);
}

TEST(FloatAssertions, ExpectNear) {
    /* EXPECT_NEAR: 期望两个值的差在指定精度范围内 */
    double a = 3.14159;
    double b = 3.14160;
    float f1 = 1.0f;
    float f2 = 1.0001f;
    
    EXPECT_NEAR(a, b, 0.001);  /* 差值在 0.001 以内 */
    
    /* 也可以用于 float */
    EXPECT_NEAR(f1, f2, 0.001);
}

/* ============================================================================
 * 致命断言测试 - ASSERT 系列（失败会立即终止测试）
 * ========================================================================== */

TEST(FatalAssertions, AssertTrue) {
    /* ASSERT_TRUE: 断言条件为真，失败会立即终止测试 */
    ASSERT_TRUE(1 == 1);
    printf("  这行会执行\n");
}

TEST(FatalAssertions, AssertFalse) {
    /* ASSERT_FALSE: 断言条件为假 */
    ASSERT_FALSE(1 == 2);
    printf("  这行会执行\n");
}

TEST(FatalAssertions, AssertEQ) {
    /* ASSERT_EQ: 断言两个值相等 */
    int a = 42;
    int b = 42;
    ASSERT_EQ(a, b);
    printf("  断言通过后继续执行\n");
}

TEST(FatalAssertions, AssertNE) {
    /* ASSERT_NE: 断言两个值不相等 */
    ASSERT_NE(10, 20);
}

TEST(FatalAssertions, AssertComparisons) {
    /* ASSERT_LT, ASSERT_LE, ASSERT_GT, ASSERT_GE: 比较断言 */
    ASSERT_LT(5, 10);
    ASSERT_LE(10, 10);
    ASSERT_GT(10, 5);
    ASSERT_GE(10, 10);
}

TEST(FatalAssertions, AssertStrEQ) {
    /* ASSERT_STREQ: 断言字符串相等 */
    ASSERT_STREQ("hello", "hello");
}

TEST(FatalAssertions, AssertStrNE) {
    /* ASSERT_STRNE: 断言字符串不相等 */
    ASSERT_STRNE("hello", "world");
}

TEST(FatalAssertions, AssertNull) {
    /* ASSERT_NULL: 断言指针为 NULL */
    void *ptr = NULL;
    ASSERT_NULL(ptr);
}

TEST(FatalAssertions, AssertNotNull) {
    /* ASSERT_NOT_NULL: 断言指针不为 NULL */
    int value = 42;
    int *ptr = &value;
    ASSERT_NOT_NULL(ptr);
}

TEST(FatalAssertions, AssertFloatEQ) {
    /* ASSERT_FLOAT_EQ: 断言 float 值近似相等 */
    ASSERT_FLOAT_EQ(3.14f, 3.14f);
}

TEST(FatalAssertions, AssertDoubleEQ) {
    /* ASSERT_DOUBLE_EQ: 断言 double 值近似相等 */
    ASSERT_DOUBLE_EQ(2.718281828, 2.718281828);
}

TEST(FatalAssertions, AssertNear) {
    /* ASSERT_NEAR: 断言两个值的差在指定精度范围内 */
    ASSERT_NEAR(3.14159, 3.14160, 0.001);
}

TEST(FatalAssertions, AssertEmpty) {
    /* ASSERT_EMPTY: 断言内存块全为零 */
    char buffer[5];
    memset(buffer, 0, sizeof(buffer));
    ASSERT_EMPTY(buffer, sizeof(buffer));
}

TEST(FatalAssertions, AssertNotEmpty) {
    /* ASSERT_NOT_EMPTY: 断言内存块不全为零 */
    char buffer[5] = {1, 0, 0, 0, 0};
    ASSERT_NOT_EMPTY(buffer, sizeof(buffer));
}

/* ============================================================================
 * Setup/Teardown 测试
 * ========================================================================== */

/* SETUP: 在测试套件的每个测试之前执行 */
SETUP(FixtureDemo) {
    int i;
    /* 分配测试所需的资源 */
    g_test_data = (int *)malloc(10 * sizeof(int));
    if (g_test_data) {
        for (i = 0; i < 10; i++) {
            g_test_data[i] = i * 10;
        }
    }
    g_setup_count++;
    printf("  [SETUP] 初始化测试环境 (第 %d 次)\n", g_setup_count);
}

/* TEARDOWN: 在测试套件的每个测试之后执行 */
TEARDOWN(FixtureDemo) {
    /* 释放测试资源 */
    if (g_test_data) {
        free(g_test_data);
        g_test_data = NULL;
    }
    g_teardown_count++;
    printf("  [TEARDOWN] 清理测试环境 (第 %d 次)\n", g_teardown_count);
}

TEST(FixtureDemo, TestWithSetup1) {
    /* 这个测试会先执行 SETUP，再执行测试，最后执行 TEARDOWN */
    ASSERT_NOT_NULL(g_test_data);
    EXPECT_EQ(g_test_data[0], 0);
    EXPECT_EQ(g_test_data[5], 50);
    printf("  测试 1 使用了 Setup 初始化的数据\n");
}

TEST(FixtureDemo, TestWithSetup2) {
    /* 每个测试都会独立执行一次 Setup 和 Teardown */
    ASSERT_NOT_NULL(g_test_data);
    EXPECT_EQ(g_test_data[9], 90);
    printf("  测试 2 也使用了 Setup 初始化的数据\n");
}

TEST(FixtureDemo, TestWithSetup3) {
    /* 无论测试成功还是失败，Teardown 都会执行 */
    ASSERT_NOT_NULL(g_test_data);
    /* 修改数据不会影响下一个测试 */
    g_test_data[0] = 999;
    EXPECT_EQ(g_test_data[0], 999);
    printf("  测试 3 修改了数据，但不影响其他测试\n");
}

/* ============================================================================
 * DEFER 清理机制测试（类似 Go 的 defer）
 * ========================================================================== */

TEST(DeferDemo, BasicDefer) {
    /* DEFER: 注册清理回调，测试结束时自动执行（LIFO 顺序） */
    char *buffer1;
    char *buffer2;
    
    /* 分配一些资源 */
    buffer1 = (char *)malloc(100);
    ASSERT_NOT_NULL(buffer1);
    SAFE_STRCPY(buffer1, 100, "Hello");
    
    /* 注册清理函数 - 第一个 */
    DEFER(cleanup_buffer, buffer1);
    printf("  注册了 buffer1 的清理函数\n");
    
    /* 分配更多资源 */
    buffer2 = (char *)malloc(200);
    ASSERT_NOT_NULL(buffer2);
    SAFE_STRCPY(buffer2, 200, "World");
    
    /* 注册清理函数 - 第二个 */
    DEFER(cleanup_buffer, buffer2);
    printf("  注册了 buffer2 的清理函数\n");
    
    /* 使用资源 */
    EXPECT_STREQ(buffer1, "Hello");
    EXPECT_STREQ(buffer2, "World");
    
    printf("  测试结束，DEFER 清理函数将按 LIFO 顺序执行\n");
    /* 测试结束时会自动执行：
     * 1. cleanup_buffer(buffer2)  // 后注册的先执行
     * 2. cleanup_buffer(buffer1)  // 先注册的后执行
     */
}

TEST(DeferDemo, DeferWithFile) {
    /* DEFER 可以用于各种资源的清理 */
    FILE *fp;
    char *buffer;
    
    /* 创建一个临时文件 */
    SAFE_FOPEN(fp, "test_defer.txt", "w");
    ASSERT_NOT_NULL(fp);
    
    /* 注册文件关闭函数 */
    DEFER(cleanup_file, fp);
    printf("  注册了文件关闭函数\n");
    
    /* 写入数据 */
    fprintf(fp, "DEFER test\n");
    fflush(fp);
    
    /* 分配内存 */
    buffer = (char *)malloc(50);
    ASSERT_NOT_NULL(buffer);
    DEFER(cleanup_buffer, buffer);
    printf("  注册了内存清理函数\n");
    
    /* 即使这里发生断言失败，清理函数也会执行 */
    EXPECT_NOT_NULL(buffer);
    
    printf("  测试结束，将先清理内存，再关闭文件（LIFO）\n");
}

TEST(DeferDemo, DeferWithAssertFailure) {
    /* 演示 DEFER 在断言失败时也会执行 */
    
    char *important_resource = (char *)malloc(1024);
    ASSERT_NOT_NULL(important_resource);
    DEFER(cleanup_buffer, important_resource);
    
    printf("  已分配重要资源并注册清理函数\n");
    printf("  即使后面的测试失败，资源也会被正确清理\n");
    
    /* 这个断言会通过 */
    EXPECT_EQ(1, 1);
    
    /* 清理函数会在测试结束时执行 */
}

/* ============================================================================
 * EXPECT vs ASSERT 区别演示
 * ========================================================================== */

TEST(ExpectVsAssert, ExpectContinues) {
    /* EXPECT 失败后会继续执行（这里演示正常情况） */
    printf("  开始测试\n");
    
    EXPECT_EQ(1, 1);  /* 这个会成功 */
    printf("  第一个 EXPECT 断言成功\n");
    
    EXPECT_LT(3, 4);  /* 这个也会成功 */
    printf("  第二个 EXPECT 断言成功\n");
    
    EXPECT_EQ(5, 5);  /* 这个会成功 */
    printf("  所有 EXPECT 都成功，测试完成\n");
    printf("  提示：如果 EXPECT 失败，测试会继续执行，记录失败但不停止\n");
}

TEST(ExpectVsAssert, AssertStops) {
    /* ASSERT 失败后会立即停止测试（这里演示正常情况） */
    printf("  开始测试\n");
    
    ASSERT_EQ(1, 1);  /* 这个会成功 */
    printf("  第一个 ASSERT 断言成功，继续执行\n");
    
    ASSERT_EQ(2, 2);  /* 这个会成功 */
    printf("  第二个 ASSERT 断言成功\n");
    
    ASSERT_EQ(5, 5);  /* 这个会成功 */
    printf("  所有 ASSERT 都成功，测试完成\n");
    printf("  提示：如果 ASSERT 失败，测试会立即停止，不执行后续代码\n");
}

/* ============================================================================
 * 混合使用演示
 * ========================================================================== */

TEST(MixedDemo, ResourceManagement) {
    /* 综合演示：Setup/Teardown + DEFER + ASSERT/EXPECT */
    int *data;
    int i;
    
    printf("  这个测试套件没有 Setup/Teardown\n");
    
    /* 分配资源并使用 DEFER 管理 */
    data = (int *)malloc(100 * sizeof(int));
    ASSERT_NOT_NULL(data);  /* 分配失败则终止 */
    DEFER(free, data);
    
    /* 初始化数据 */
    for (i = 0; i < 100; i++) {
        data[i] = i;
    }
    
    /* 使用 EXPECT 检查多个值（即使失败也会继续检查） */
    EXPECT_EQ(data[0], 0);
    EXPECT_EQ(data[50], 50);
    EXPECT_EQ(data[99], 99);
    
    /* 使用 ASSERT 检查关键条件 */
    ASSERT_LT(data[10], data[20]);
    
    printf("  测试完成，DEFER 会自动清理内存\n");
}

TEST(MixedDemo, ComplexTest) {
    /* 复杂测试场景 */
    char *str1;
    char *str2;
    int len1;
    int len2;
    float ratio;
    
    /* 多个资源的分配和清理 */
    str1 = (char *)malloc(50);
    ASSERT_NOT_NULL(str1);
    DEFER(free, str1);
    SAFE_STRCPY(str1, 50, "Resource 1");
    
    str2 = (char *)malloc(50);
    ASSERT_NOT_NULL(str2);
    DEFER(free, str2);
    SAFE_STRCPY(str2, 50, "Resource 2");
    
    /* 字符串断言 */
    EXPECT_STREQ(str1, "Resource 1");
    EXPECT_STREQ(str2, "Resource 2");
    EXPECT_STRNE(str1, str2);
    
    /* 数值断言 */
    len1 = (int)strlen(str1);
    len2 = (int)strlen(str2);
    EXPECT_GT(len1, 0);
    EXPECT_GT(len2, 0);
    EXPECT_EQ(len1, len2);
    
    /* 浮点数断言 */
    ratio = (float)len1 / (float)len2;
    EXPECT_FLOAT_EQ(ratio, 1.0f);
    EXPECT_NEAR(ratio, 1.0, 0.001);
    
    printf("  复杂测试完成\n");
}

/* ============================================================================
 * 边界条件测试
 * ========================================================================== */

TEST(EdgeCases, ZeroValues) {
    /* 测试零值 */
    float zero_f;
    double zero_d;
    
    EXPECT_EQ(0, 0);
    EXPECT_LE(0, 0);
    EXPECT_GE(0, 0);
    
    zero_f = 0.0f;
    EXPECT_FLOAT_EQ(zero_f, 0.0f);
    
    zero_d = 0.0;
    EXPECT_DOUBLE_EQ(zero_d, 0.0);
}

TEST(EdgeCases, NegativeValues) {
    /* 测试负值 */
    EXPECT_LT(-10, 0);
    EXPECT_LT(-5, -1);
    EXPECT_GT(-1, -5);
    
    EXPECT_FLOAT_EQ(-3.14f, -3.14f);
    EXPECT_DOUBLE_EQ(-2.718, -2.718);
}

TEST(EdgeCases, LargeValues) {
    /* 测试大值 */
    int large;
    double pi;
    
    large = 1000000;
    EXPECT_EQ(large, 1000000);
    EXPECT_GT(large, 999999);
    
    pi = 3.141592653589793;
    EXPECT_NEAR(pi, 3.14159, 0.00001);
}

TEST(EdgeCases, EmptyStrings) {
    /* 测试空字符串 */
    const char *empty1 = "";
    const char *empty2 = "";
    EXPECT_STREQ(empty1, empty2);
    EXPECT_STRNE(empty1, "not empty");
}

/* ============================================================================
 * 演示失败的测试（可选，通常不包含在正式测试中）
 * ========================================================================== */

/* 取消注释以下测试可以看到失败时的输出 */

/*
TEST(FailureDemo, ExpectFailure) {
    // 故意失败的 EXPECT 测试
    EXPECT_EQ(1, 2);  // 这会失败但继续
    EXPECT_TRUE(0);   // 这也会失败但继续
    printf("  即使失败，测试也会继续到这里\n");
}

TEST(FailureDemo, AssertFailure) {
    // 故意失败的 ASSERT 测试
    printf("  开始测试\n");
    ASSERT_EQ(1, 2);  // 这会失败并立即停止
    printf("  这行永远不会输出\n");
}
*/

/* ============================================================================
 * 主函数
 * ========================================================================== */
#ifdef _UNICODE
int wmain(int argc, wchar_t *argv[]) {
#else
int main(int argc, char *argv[]) {
#endif
    int result;

#ifdef _WIN32
	SetConsoleOutputCP(CP_UTF8);
#endif

    /* RUN_ALL_TESTS: 运行所有测试的主入口 */
    /* 返回值：0 表示所有测试通过，1 表示有测试失败 */
    result = RUN_ALL_TESTS(argc, argv);
    return result;
}

/* ============================================================================
 * 编译和运行说明
 * ========================================================================== */

/*
 * Windows (MSVC):
 *   cl /W4 main.c
 *   main.exe
 *
 * Windows (MinGW):
 *   gcc -Wall -Wextra -std=c99 main.c -o main.exe
 *   main.exe
 *
 * Linux (GCC):
 *   gcc -Wall -Wextra -std=c99 main.c -o main
 *   ./main
 *
 * 使用命令行参数：
 *   ./main --filter=Basic*         # 只运行 Basic 开头的测试
 *   ./main --filter=*Float*        # 运行包含 Float 的测试
 *   ./main --filter=FixtureDemo.*  # 运行 FixtureDemo 套件的所有测试
 *   ./main --repeat=5              # 重复运行 5 次
 *   ./main --list                  # 列出所有测试
 */

