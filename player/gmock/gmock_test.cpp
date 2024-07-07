
#include "third_party/GoogleTest/include/gmock/gmock.h"

#include "third_party/GoogleTest/include/gtest/gtest.h"

// 被测试的函数，计算两个整数的和
int add(int a, int b) { return a + b; }

// 测试用例，测试add函数的正确性
TEST(AddTest, PositiveNumbers) {
  EXPECT_EQ(add(2, 3), 5);
  EXPECT_EQ(add(0, 0), 1);
  EXPECT_EQ(add(-1, 1), 0);
}

// 测试用例，测试add函数的性能
TEST(AddTest, Performance) { EXPECT_GT(add(1000000000, 1000000000), 0); }
