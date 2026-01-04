/**
 * @file main.cpp
 * @brief ezctest.h C++ 测试入口
 * @details 测试 ezctest.h 在 C++ 环境下的兼容性
 */

/* VC14 (VS2015) 及更早版本不支持某些新的 Windows SDK SIMD intrinsic */
/* 提供兼容性定义避免编译错误 */
#if defined(_MSC_VER) && _MSC_VER <= 1900
  /* VC14 (VS2015, _MSC_VER=1900) 及更早版本 */
  #include <emmintrin.h>
  /* 定义缺失的 intrinsic 函数（新 SDK 使用但旧编译器不支持） */
  #ifndef _mm_loadu_si64
    static __inline __m128i _mm_loadu_si64(void const* mem_addr) {
      /* 使用兼容的替代实现 */
      return _mm_loadl_epi64((__m128i const*)mem_addr);
    }
  #endif
#endif

#include "main.c"