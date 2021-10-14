#include "netswitch/base/connection/connection.h"

#include <gtest/gtest.h>

using namespace netswitch::base;

class ConnectionTest : public testing::Test {
 protected:
  void SetUp() override {}

  void TearDown() override {}
};

TEST_F(ConnectionTest, RawConnTest) {
  int data = 0;
  std::string server_msg = "hello client";
  std::string client_msg = "hello server";
  PipeListener listener(
      "0.0.0.0", "9090",
      [&](std::shared_ptr<Pipe> pipe, const void *cb_data) {
        EXPECT_EQ(cb_data, &data);
        char buffer[32] = {0};
        pipe->Read(buffer, client_msg.size());
        EXPECT_EQ(buffer, client_msg);
        pipe->Write(server_msg.data(), server_msg.size());
      },
      &data);
  listener.Start();
  PipeConnector conn;
  auto pipe = conn.Connect("0.0.0.0", "9090");
  pipe->Write(client_msg.data(), client_msg.size());
  char buffer[32] = {0};
  pipe->Read(buffer, server_msg.size());
  EXPECT_EQ(buffer, server_msg);
  listener.Stop();
}

TEST_F(ConnectionTest, TlsConnTest) {}