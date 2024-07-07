
#include "log/ddup_log.h"
#include "player/fifo_controller.h"
#include "third_party/GoogleTest/include/gmock/gmock.h"
#include "third_party/GoogleTest/include/gtest/gtest.h"

using namespace testing;

class MockFreeHandler : public FreeHandler {
 public:
  MOCK_METHOD(int, free_data, (void *), (override));
};

// 测试夹具
class FifoTest : public testing::Test {
 protected:
  void SetUp() override {
    handler_ = new MockFreeHandler();
    // 在每个测试用例之前执行的代码
    fifo_ = new Fifo("test_fifo", 10, sizeof(int), COMMON_FIFO, handler_);
  }

  void TearDown() override {
    // 在每个测试用例之后执行的代码
    delete fifo_;
    delete handler_;
  }

  Fifo *fifo_;
  MockFreeHandler *handler_;
};

TEST_F(FifoTest, Discard) {
  // 模拟填充 Fifo
  int a[10];
  for (int i = 0; i < 10; ++i) {
    a[i] = i;
    int *p = &(a[i]);
    //int **q = &p;
    fifo_->append((void **)&p);
  }

  // 预期调用 MockFreeHandler 的 free_data 方法 5 次
  EXPECT_CALL(*handler_, free_data(_)).Times(10).WillRepeatedly(Return(0));

  // 清空 Fifo 中的一部分数据
  int discard_result = fifo_->discard();

  // 断言清空成功
  ASSERT_EQ(discard_result, 0);

  ASSERT_TRUE(fifo_->is_empty());
}
