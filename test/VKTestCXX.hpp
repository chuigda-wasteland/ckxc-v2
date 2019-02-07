#ifndef VKTESTCXX_HPP
#define VKTESTCXX_HPP

#include <cmath>
#include <cstdio>
#include <cstring>
#include <string>
#include <limits>
#include <type_traits>

#ifndef __cplusplus
#error VKTestCXX is for C++ only. For C tests, use VKTest.
#endif

template <typename T>
auto FloatEquals(T lhs, T rhs)
  -> std::enable_if_t<std::is_floating_point<T>::value, bool> {
  return (std::fabs(lhs - rhs) <= std::numeric_limits<T>::epsilon());
}

class VKTestStackMarker {
public:
  VKTestStackMarker(std::string const& testName)
    : m_TestName(testName), m_TotalCase(0), m_PassingCase(0) {
    std::fprintf(stderr, "Performing test: '%s'\n", testName.c_str());
  }

  ~VKTestStackMarker() {
    std::fprintf(stderr, "Test '%s' finished, %d of %d passing\n",
                 m_TestName.c_str(), m_PassingCase, m_TotalCase);
  }

  void AssertTrue(bool condition, char const* conditionString) {
    AddTestCase();
    fprintf(stderr, "Testing AssertTrue(%s)... ", conditionString);
    if (condition) AddPassingCase();
    else AddFailingCase();
  }

  void AssertFalse(bool condition, char const* conditionString) {
    AddTestCase();
    fprintf(stderr, "Testing AssertFalse(%s)... ", conditionString);
    if (!condition) AddPassingCase();
    else AddFailingCase();
  }

  template <typename T, typename U>
  void AssertEquals(T&& lhs, U&& rhs, 
                    char const* lhsString, char const* rhsString) {
    AddTestCase();
    fprintf(stderr, "Testing AssertEquals(%s, %s)... ", 
            lhsString, rhsString);
    if (std::forward<T>(lhs) == std::forward<U>(rhs)) AddPassingCase();
    else AddFailingCase();
  }

  template <typename T, typename U>
  void AssertNotEquals(T&& lhs, U&& rhs, 
                       char const* lhsString, char const* rhsString) {
    AddTestCase();
    fprintf(stderr, "Testing AssertEquals(%s, %s)... ", 
            lhsString, rhsString);
    if (std::forward<T>(lhs) != std::forward<U>(rhs)) AddPassingCase();
    else AddFailingCase();
  }

  void AssertEquals(double lhs, double rhs,
                    char const *lhsString, char const* rhsString) {
    AddTestCase();
    fprintf(stderr, "Testing AssertEquals(%s, %s)...",
            lhsString, rhsString);
    if (FloatEquals<double>(lhs, rhs)) AddPassingCase();
    else AddFailingCase();
  }

  void AssertNotEquals(double lhs, double rhs,
                       char const *lhsString, char const* rhsString) {
    AddTestCase();
    fprintf(stderr, "Testing AssertEquals(%s, %s)...",
            lhsString, rhsString);
    if (!FloatEquals<double>(lhs, rhs)) AddPassingCase();
    else AddFailingCase();
  }

  void AssertEquals(float lhs, float rhs,
                    char const *lhsString, char const* rhsString) {
    AddTestCase();
    fprintf(stderr, "Testing AssertEquals(%s, %s)...",
            lhsString, rhsString);
    if (FloatEquals<float>(lhs, rhs)) AddPassingCase();
    else AddFailingCase();
  }

  void AssertNotEquals(float lhs, float rhs,
                       char const *lhsString, char const* rhsString) {
    AddTestCase();
    fprintf(stderr, "Testing AssertEquals(%s, %s)...",
            lhsString, rhsString);
    if (!FloatEquals<float>(lhs, rhs)) AddPassingCase();
    else AddFailingCase();
  }

  void AssertEquals(char const* lhs, char const* rhs,
                    char const *lhsString, char const* rhsString) {
    AddTestCase();
    fprintf(stderr, "Testing AssertEquals(%s, %s)...",
            lhsString, rhsString);
    if (!strcmp(lhs, rhs)) AddPassingCase();
    else AddFailingCase();
  }

  void AssertNotEquals(char const* lhs, char const* rhs,
                       char const *lhsString, char const* rhsString) {
    AddTestCase();
    fprintf(stderr, "Testing AssertEquals(%s, %s)...",
            lhsString, rhsString);
    if (strcmp(lhs, rhs)) AddPassingCase();
    else AddFailingCase();
  }

  static void PreInitialize() {
    fprintf(stderr, "Starting VkTest...\n");
    m_GlobalTotalCase = 0;
    m_GlobalPassingCase = 0;
  }

  static void Summerize() {
    fprintf(stderr, "All tests finished, %d of %d passing\n",
            m_GlobalPassingCase, m_GlobalTotalCase);
  }

private:
  void AddTestCase() {
    ++m_TotalCase;
    ++m_GlobalTotalCase;
  }

  void AddPassingCase() {
    ++m_PassingCase;
    ++m_GlobalPassingCase;
    fprintf(stderr, "Passing\n");
  }

  void AddFailingCase() {
    fprintf(stderr, "Failing\n");
  }

  std::string m_TestName;
  int m_TotalCase, m_PassingCase;

  static int m_GlobalTotalCase, m_GlobalPassingCase;
};

int VKTestStackMarker::m_GlobalTotalCase;
int VKTestStackMarker::m_GlobalPassingCase;

#define VkTestSectionStart(name) VKTestStackMarker vkInternStkMarker(name)
#define VkAssertTrue(cond) vkInternStkMarker.AssertTrue(cond, #cond)
#define VkAssertFalse(cond) vkInternStkMarker.AssertFalse(cond, #cond)
#define VkAssertEquals(lhs, rhs) \
  vkInternStkMarker.AssertEquals(lhs, rhs, #lhs, #rhs)
#define VkAssertNotEquals(lhs, rhs) \
  vkInternStkMarker.AssertNotEquals(lhs, rhs, #lhs, #rhs)

#define VkTestStart() VKTestStackMarker::PreInitialize()
#define VkTestFinish() VKTestStackMarker::Summerize()

#endif
