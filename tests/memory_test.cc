/*! @file memory.cc
 *  @brief Tests for src/primitives/memory.h.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright © 2013 Samsung R&D Institute Russia
 */

#include <gtest/gtest.h>
#include <simd/memory.h>

#ifdef __AVX__

TEST(Memory, align_complement_f32) {
  float ptr[32] __attribute__ ((aligned (32)));  // NOLINT(whitespace/parens)
  EXPECT_EQ(0, align_complement_f32(ptr));
  EXPECT_EQ(7, align_complement_f32(&ptr[1]));
  EXPECT_EQ(5, align_complement_f32(&ptr[3]));
  EXPECT_EQ(1, align_complement_f32(&ptr[7]));
  EXPECT_EQ(0, align_complement_f32(&ptr[8]));
}

TEST(Memory, align_complement_i16) {
  int16_t ptr[32] __attribute__ ((aligned (32)));  // NOLINT(whitespace/parens)
  EXPECT_EQ(0, align_complement_i16(ptr));
  EXPECT_EQ(15, align_complement_i16(&ptr[1]));
  EXPECT_EQ(13, align_complement_i16(&ptr[3]));
  EXPECT_EQ(1, align_complement_i16(&ptr[15]));
  EXPECT_EQ(0, align_complement_i16(&ptr[16]));
}

TEST(Memory, align_complement_i32) {
  int32_t ptr[32] __attribute__ ((aligned (32)));  // NOLINT(whitespace/parens)
  EXPECT_EQ(0, align_complement_i32(ptr));
  EXPECT_EQ(7, align_complement_i32(&ptr[1]));
  EXPECT_EQ(5, align_complement_i32(&ptr[3]));
  EXPECT_EQ(1, align_complement_i32(&ptr[7]));
  EXPECT_EQ(0, align_complement_i32(&ptr[8]));
}

#endif

TEST(Memory, malloc_aligned) {
  auto ptr = malloc_aligned(100);
#ifdef __AVX__
  EXPECT_EQ(static_cast<uintptr_t>(0),
            reinterpret_cast<uintptr_t>(ptr) % 32);
#elif defined(__ARM_NEON__)
  EXPECT_EQ(static_cast<uintptr_t>(0),
            reinterpret_cast<uintptr_t>(ptr) % 64);
#endif
  free(ptr);
}

TEST(Memory, malloc_aligned_offset) {
  auto ptr = malloc_aligned_offset(100, 3);
#ifdef __AVX__
  EXPECT_EQ(static_cast<uintptr_t>(3),
            reinterpret_cast<uintptr_t>(ptr) % 32);
#elif defined(__ARM_NEON__)
  EXPECT_EQ(static_cast<uintptr_t>(3),
            reinterpret_cast<uintptr_t>(ptr) % 64);
#endif
  free(reinterpret_cast<char*>(ptr) - 3);
}

TEST(Memory, mallocf) {
  auto ptr = mallocf(100);
#ifdef __AVX__
  EXPECT_EQ(static_cast<uintptr_t>(0),
            reinterpret_cast<uintptr_t>(ptr) % 32);
#elif defined(__ARM_NEON__)
  EXPECT_EQ(static_cast<uintptr_t>(0),
            reinterpret_cast<uintptr_t>(ptr) % 64);
#endif
  free(ptr);
}

TEST(Memory, memsetf) {
  float ptr[102] __attribute__ ((aligned (32)));  // NOLINT(whitespace/parens)
  memsetf(&ptr[3], 3.0f, 99);
  for (int i = 3; i < 99; i++) {
    ASSERT_EQ(3.0f, ptr[i]);
  }
}

TEST(Memory, zeropadding) {
  float orig[100];
  memsetf(orig, 1.0f, 100);
  size_t nl;
  auto ptr = zeropadding(orig, 100, &nl);
#ifdef __AVX__
  EXPECT_EQ(static_cast<uintptr_t>(0),
            reinterpret_cast<uintptr_t>(ptr) % 32);
#elif defined(__ARM_NEON__)
  EXPECT_EQ(static_cast<uintptr_t>(0),
            reinterpret_cast<uintptr_t>(ptr) % 64);
#endif
  EXPECT_EQ(static_cast<size_t>(256), nl);
  for (int i = 100; i < 256; i++) {
    EXPECT_EQ(0.0f, ptr[i]);
  }
  free(ptr);
}

TEST(Memory, rmemcpyf) {
  float src[25] __attribute__ ((aligned (32)));  // NOLINT(*)
  const int len = sizeof(src) / sizeof(float);  // NOLINT(*)
  for (int i = 0; i < len; i++) {
    src[i] = i;
  }
  float dest[25] __attribute__ ((aligned (32)));  // NOLINT(*)
  rmemcpyf(dest + 1, src + 1, len - 1);
  for (int i = 1; i < len; i++) {
    ASSERT_EQ(dest[i], src[len - i]);
  }
}

#include "tests/google/src/gtest_main.cc"
