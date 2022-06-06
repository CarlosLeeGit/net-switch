#include "netswitch/base/status.h"

#include <gtest/gtest.h>

using namespace netswitch::base;

class StatusTest : public testing::Test {
 protected:
  void SetUp() override {}

  void TearDown() override {}
};

TEST_F(StatusTest, StatusTest) {
  Status s(STATUS_OK);
  EXPECT_TRUE(s == STATUS_OK);
  EXPECT_TRUE(STATUS_OK == s);
  EXPECT_TRUE(STATUS_FAIL != s);
  Status s2 = STATUS_FAIL;
  EXPECT_NE(s, s2);
}