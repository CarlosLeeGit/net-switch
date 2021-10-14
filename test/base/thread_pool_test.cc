#include "netswitch/base/thread_pool.h"

#include <gtest/gtest.h>

#include <future>

class ThreadPoolTest : public testing::Test {
 protected:
  void SetUp() override {}

  void TearDown() override {}
};

TEST_F(ThreadPoolTest, SubmitTest) {
  netswitch::base::ThreadPool pool(1);
  auto result = pool.Submit([]() { return 1; });
  auto ret = result.wait_for(std::chrono::seconds(1));
  ASSERT_EQ(ret, std::future_status::ready);
  EXPECT_EQ(result.get(), 1);
}

TEST_F(ThreadPoolTest, WokerTest) {
  const size_t thread_count = 5;
  netswitch::base::ThreadPool pool(thread_count);
  bool running = true;
  std::atomic_int count(0);
  for (size_t i = 0; i < thread_count; ++i) {
    pool.Submit([&running, &count]() {
      count.fetch_add(1);
      while (running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
      }
    });
  }

  size_t try_count = 0;
  while (count < thread_count) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    ++try_count;
    if (try_count >= 10) {
      break;
    }
  }

  ASSERT_EQ(count, thread_count);
  running = false;
}

TEST_F(ThreadPoolTest, ParamTest) {
  netswitch::base::ThreadPool pool(1);
  int code = 12345;
  auto handle = pool.Submit([](int code) { return code; }, code);
  EXPECT_EQ(handle.get(), code);
}