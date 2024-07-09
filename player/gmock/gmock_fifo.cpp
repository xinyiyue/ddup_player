#include <chrono>
#include <thread>

#include "log/ddup_log.h"
#include "player/fifo_controller.h"
#include "third_party/GoogleTest/include/gmock/gmock.h"
#include "third_party/GoogleTest/include/gtest/gtest.h"

using namespace testing;

#define TAG "MockFifo"

class MockFreeHandler : public FreeHandler {
 public:
  MOCK_METHOD(int, free_data, (void *), (override));
};

class FifoTest : public testing::Test {
 protected:
  void SetUp() override {
    // 在每个测试用例之前执行的代码
    handler_ = new MockFreeHandler();
    fifo_ = new Fifo("test_fifo", 10, sizeof(void *), COMMON_FIFO, handler_);
  }

  void TearDown() override {
    // 在每个测试用例之后执行的代码
    delete fifo_;
    delete handler_;
  }

  Fifo *fifo_;
  MockFreeHandler *handler_;
};

TEST_F(FifoTest, Fifo_save_value) {
  int a[10];
  for (int i = 0; i < 10; ++i) {
    a[i] = i;
    int p = a[i];
    fifo_->append(&p);
  }

  bool is_full = fifo_->is_full();
  ASSERT_EQ(is_full, true);

  for (int i = 0; i < 10; ++i) {
    int q = -1;
    fifo_->consume(&q);
    ASSERT_EQ(q, a[i]);
  }

  bool is_empty = fifo_->is_empty();
  ASSERT_EQ(is_empty, true);
}

TEST_F(FifoTest, Fifo_save_ptr) {
  int a[10];
  for (int i = 0; i < 10; ++i) {
    a[i] = i;
    int *p = &(a[i]);
    fifo_->append(&p);
  }

  bool is_full = fifo_->is_full();
  ASSERT_EQ(is_full, true);

  for (int i = 0; i < 10; ++i) {
    int *q = nullptr;
    fifo_->consume(&q);
    ASSERT_EQ(q, &(a[i]));
  }

  bool is_empty = fifo_->is_empty();
  ASSERT_EQ(is_empty, true);
}

class BufferBaseTest : public ::testing::Test {
 public:
  // 辅助函数，用于在后台线程中调用append函数
  void appendInBackground() {
    for (int i = 0; i < 11; i++) {
      arr[i] = i;
      int *p = &(arr[i]);
      producer_->append_buffer(&p);
    }
  }

  void appendInBackground2() {
    for (int i = 0; i < 11; i++) {
      arr[i] = i;
      int *p = &(arr[i]);
      producer_->append_buffer(&p);
      if (i == 5) std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    }
  }

  void appendInBackground3() {
    for (int i = 0; i < 8; i++) {
      arr[i] = i;
      int *p = &(arr[i]);
      producer_->append_buffer(&p);
    }
    LOGI(TAG, "%s:%d append data finish", __func__, __LINE__);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    LOGI(TAG, "%s:%d call consume abort", __func__, __LINE__);
    consumer_->consume_abort();
  }

 protected:
  void SetUp() override {
    producer_ = new BufferProducer("MockProducer");
    consumer_ = new BufferConsumer("MockConsumer");
    handler_ = new MockFreeHandler();
    fifo_ = new Fifo("test_fifo", 10, sizeof(void *), COMMON_FIFO, handler_);
    producer_->bind_fifo(fifo_);
    consumer_->bind_fifo(fifo_);
  }

  void TearDown() override {
    delete producer_;
    delete consumer_;
    delete fifo_;
    delete handler_;
  }

  BufferProducer *producer_;
  BufferConsumer *consumer_;
  Fifo *fifo_;
  MockFreeHandler *handler_;
  int arr[11];
};

TEST_F(BufferBaseTest, AppendCanBeAwaken) {
  std::thread appendThread(&BufferBaseTest::appendInBackground, this);

  // 在后台线程中调用append函数后，等待一段时间让producer将fifo写满阻塞住再调用consume函数
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  // 调用consume函数，期望能唤醒append函数
  for (int i = 0; i < 11; i++) {
    int *data = nullptr;
    bool result = consumer_->consume_buffer(&data);

    EXPECT_TRUE(result);
    EXPECT_EQ(data, &(arr[i]));
    EXPECT_EQ((*data), i);
  }
  appendThread.join();
}

TEST_F(BufferBaseTest, AppendCanBeAbort) {
  std::thread appendThread(&BufferBaseTest::appendInBackground, this);

  // 在后台线程中调用append函数后，等待一段时间让producer将fifo写满阻塞住再调用consume函数
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  // 调用abort函数，期望能唤醒append函数
  int *data = nullptr;
  bool result = producer_->append_abort();
  EXPECT_TRUE(result);
  appendThread.join();
}
TEST_F(BufferBaseTest, ConsumeCanBeAwaken) {
  //在appendInBackground2中写完5个数据后等待5000ms
  //让consumer将数据消费完阻塞住后继续append_data检查consumer是否能被唤醒继续读数据
  std::thread appendThread(&BufferBaseTest::appendInBackground2, this);
  for (int i = 0; i < 11; i++) {
    int *data = nullptr;
    bool result = consumer_->consume_buffer(&data);

    EXPECT_TRUE(result);
    EXPECT_EQ(data, &(arr[i]));
    EXPECT_EQ((*data), i);
  }
  appendThread.join();
}

TEST_F(BufferBaseTest, ConsumeAbort) {
  //在appendInBackground3中写完10个数据后等待5000ms
  //让consumer将数据消费完阻塞住后调用consumer_abort检查consumer是否能被唤醒然后退出
  std::thread appendThread(&BufferBaseTest::appendInBackground3, this);

  // 在后台线程中调用append函数后，等待一段时间再调用consume函数
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  LOGI(TAG, "%s:%d start consume data", __func__, __LINE__);
  while (1) {
    int *data = nullptr;
    bool result = consumer_->consume_buffer(&data);
    if (!result) {
      break;
    }
  }
  appendThread.join();
}

TEST_F(BufferBaseTest, ConsumeDiscard) {
  //在appendInBackground3中写完10个数据后producer阻塞住
  std::thread appendThread(&BufferBaseTest::appendInBackground, this);

  //等待一段时间再调用discard函数丢弃10个数据，然后唤醒producer继续append第11个数据
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  LOGI(TAG, "%s:%d start discard data", __func__, __LINE__);
  // 预期调用 MockFreeHandler 的 free_data 方法 10 次
  EXPECT_CALL(*handler_, free_data(_)).Times(10).WillRepeatedly(Return(0));
  bool result = consumer_->discard_buffer();
  EXPECT_TRUE(result);

  LOGI(TAG, "%s:%d start consume last data", __func__, __LINE__);
  int *data = nullptr;
  result = consumer_->consume_buffer(&data);
  LOGI(TAG, "%s:%d end consume last data", __func__, __LINE__);
  EXPECT_TRUE(result);
  EXPECT_EQ(data, &(arr[10]));
  EXPECT_EQ((*data), 10);

  appendThread.join();
}
