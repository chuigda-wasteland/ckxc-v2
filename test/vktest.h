/**
 * VK-Test is a tiny header-only test framework for C
 * Please note that this file should only be included in one file,
 * and this test should not be used for multi-thread case
 */

#ifndef VKTEST_H
#define VKTEST_H

#include <cstdio>
#include <cstdlib>
#include <cmath>

static unsigned __g_passing_tests, __g_all_tests;
static unsigned __g_passing_tests_section, __g_all_tests_section;

#define VK_TEST_BEGIN std::fprintf(stderr, "VKTest starting...\n");

#define VK_TEST_END \
  std::fprintf(stderr, "All test finished, %u of %u passing\n",\
               __g_passing_tests, __g_all_tests);

#define VK_TEST_SECTION_BEGIN(section_name) \
  { \
    std::fprintf(stderr, "Running test: %s...\n", section_name); \
    __g_passing_tests_section = 0; \
    __g_all_tests_section = 0; \
  }

#define VK_TEST_SECTION_END(section_name) \
  { \
    std::fprintf(stderr, "Test %s finished, %u of %u passing.\n", \
                 section_name, \
                 __g_passing_tests_section, __g_all_tests_section); \
  }

#define VK_ASSERT(condition) \
  { \
    __g_all_tests++; \
    __g_all_tests_section++; \
    std::fprintf(stderr, "Testing Assert(%s)... ", #condition); \
    if (condition) { \
      std::fprintf(stderr, "Passing\n"); \
      __g_passing_tests++; \
      __g_passing_tests_section++; \
    } \
    else { \
      std::fprintf(stderr, "Failing\n"); \
    } \
  }

#define VK_ASSERT_NOT(condition) \
  { \
    __g_all_tests++; \
    __g_all_tests_section++; \
    std::fprintf(stderr, "Testing AssertNot(%s)... ", #condition); \
    if (!condition) { \
      std::fprintf(stderr, "Passing\n"); \
      __g_passing_tests++; \
      __g_passing_tests_section++; \
    } \
    else { \
      std::fprintf(stderr, "Failing\n"); \
    } \
  }

#define VK_ASSERT_EQUALS(standard, expr) \
  { \
    __g_all_tests++; \
    __g_all_tests_section++; \
    std::fprintf(stderr, "Testing AssertEquals(%s, %s)... ", \
                 #standard, #expr); \
    if ((standard) == (expr)) { \
      std::fprintf(stderr, "Passing\n"); \
      __g_passing_tests++; \
      __g_passing_tests_section++; \
    } \
    else { \
      std::fprintf(stderr, "Failing\n"); \
    } \
  }

#define VK_EPSILON 1e-5

#define VK_ASSERT_EQUALS_F(standard, expr) \
  { \
    __g_all_tests++; \
    __g_all_tests_section++; \
    std::fprintf(stderr, "Testing AssertEquals(%s, %s)... ", \
                 #standard, #expr); \
    if (std::fabs((standard) - (expr)) <= VK_EPSILON) { \
      std::fprintf(stderr, "Passing\n"); \
      __g_passing_tests++; \
      __g_passing_tests_section++; \
    } \
    else { \
      std::fprintf(stderr, "Failing\n"); \
    } \
  }

#define VK_ASSERT_NOT_EQUALS(standard, expr) \
  { \
  __g_all_tests++; \
  __g_all_tests_section++; \
  std::fprintf(stderr, "Testing AssertNotEquals(%s, %s)... ", \
               #standard, #expr); \
    if ((standard) != (expr)) { \
      std::fprintf(stderr, "Passing\n"); \
      __g_passing_tests++; \
      __g_passing_tests_section++; \
    } \
    else { \
      std::fprintf(stderr, "Failing\n"); \
    } \
  }

#else
  #error Multiple inclusion for vktest?
#endif
