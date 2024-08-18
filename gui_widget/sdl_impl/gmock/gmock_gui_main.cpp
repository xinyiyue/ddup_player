#include "third_party/GoogleTest/include/gmock/gmock.h"
#include "third_party/GoogleTest/include/gtest/gtest.h"

int main(int argc, char** argv) {
  // 初始化 Google Test 框架
  ::testing::InitGoogleTest(&argc, argv);

  // 初始化 Google Mock 框架
  ::testing::InitGoogleMock(&argc, argv);

  // 执行所有的测试用例
  return RUN_ALL_TESTS();
}