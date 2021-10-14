#include "netswitch/base/prefix_tree.h"

#include <gtest/gtest.h>

class PathMatchTest : public testing::Test {
 protected:
  void SetUp() override {}

  void TearDown() override {}
};

TEST_F(PathMatchTest, AddNodeTest) {
  netswitch::base::PrefixTree root("/");
  auto ret = root.AddNode("/");
  EXPECT_EQ(ret, netswitch::base::STATUS_OK);
  ret = root.AddNode("/a/b/c");
  EXPECT_EQ(ret, netswitch::base::STATUS_OK);
  ret = root.AddNode("/aaa");
  EXPECT_EQ(ret, netswitch::base::STATUS_OK);
  ret = root.AddNode("/a/");
  EXPECT_EQ(ret, netswitch::base::STATUS_OK);
  ret = root.AddNode("/a/b");
  EXPECT_EQ(ret, netswitch::base::STATUS_OK);
  ret = root.AddNode("/a/b/c");
  EXPECT_EQ(ret, netswitch::base::STATUS_EXIST);
}

TEST_F(PathMatchTest, DelNodeTest) {
  netswitch::base::PrefixTree root("/");
  auto ret = root.AddNode("/");
  EXPECT_EQ(ret, netswitch::base::STATUS_OK);
  ret = root.AddNode("/a/b/c");
  EXPECT_EQ(ret, netswitch::base::STATUS_OK);
  root.DelNode("/a/b/c");
  ret = root.AddNode("/a/b/c");
  EXPECT_EQ(ret, netswitch::base::STATUS_OK);
}

TEST_F(PathMatchTest, MatchNodeTest) {
  netswitch::base::PrefixTree root("/");
  auto ret = root.AddNode("/");
  EXPECT_EQ(ret, netswitch::base::STATUS_OK);
  ret = root.AddNode("/a/b/c");
  EXPECT_EQ(ret, netswitch::base::STATUS_OK);
  root.DelNode("/a/b/c");
  ret = root.AddNode("/a/b/c");
  EXPECT_EQ(ret, netswitch::base::STATUS_OK);
  ret = root.AddNode("/b/c/d");
  EXPECT_EQ(ret, netswitch::base::STATUS_OK);
  ret = root.AddNode("/a/c/d");
  EXPECT_EQ(ret, netswitch::base::STATUS_OK);

  std::string match_result;
  ret = root.Match("/g", match_result);
  EXPECT_EQ(ret, netswitch::base::STATUS_OK);
  EXPECT_EQ(match_result, "/");
  ret = root.Match("/a/b", match_result);
  EXPECT_EQ(ret, netswitch::base::STATUS_OK);
  EXPECT_EQ(match_result, "/");
  ret = root.Match("/a/b/c/d", match_result);
  EXPECT_EQ(ret, netswitch::base::STATUS_OK);
  EXPECT_EQ(match_result, "/a/b/c");
  ret = root.Match("/a/b/d", match_result);
  EXPECT_EQ(ret, netswitch::base::STATUS_OK);
  EXPECT_EQ(match_result, "/");
  ret = root.Match("/b/b/d", match_result);
  EXPECT_EQ(ret, netswitch::base::STATUS_OK);
  EXPECT_EQ(match_result, "/");
  ret = root.Match("/a/c/d", match_result);
  EXPECT_EQ(ret, netswitch::base::STATUS_OK);
  EXPECT_EQ(match_result, "/a/c/d");
  ret = root.Match("/", match_result);
  EXPECT_EQ(ret, netswitch::base::STATUS_OK);
  EXPECT_EQ(match_result, "/");
  ret = root.Match("/c/", match_result);
  EXPECT_EQ(ret, netswitch::base::STATUS_OK);
  EXPECT_EQ(match_result, "/");
  ret = root.Match("/b/", match_result);
  EXPECT_EQ(ret, netswitch::base::STATUS_OK);
  EXPECT_EQ(match_result, "/");

  netswitch::base::PrefixTree root2("/");
  ret = root2.Match("/w", match_result);
  EXPECT_EQ(ret, netswitch::base::STATUS_NOTFOUND);
  EXPECT_EQ(match_result, "");
  root2.AddNode("/a");
  ret = root2.Match("/w", match_result);
  EXPECT_EQ(ret, netswitch::base::STATUS_NOTFOUND);
  EXPECT_EQ(match_result, "");
}