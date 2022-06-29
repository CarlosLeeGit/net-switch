#include "netswitch/http/http_listener.h"

#include <gtest/gtest.h>

#include <thread>

class HttpListenerTest : public testing::Test {
 protected:
  void SetUp() override {}

  void TearDown() override {}
};

TEST_F(HttpListenerTest, HttpTest) {
  netswitch::http::server::ListenerConfig config;
  config.SetRWTimeout(std::chrono::seconds(10));
  config.SetKeepAliveCount(1);
  config.SetKeepAliveTimeout(std::chrono::seconds(1));

  auto listener = netswitch::http::server::HttpListener::GetListener(
      "http://127.0.0.1:32323/v1/test/index");
  ASSERT_NE(listener, nullptr);

  listener->Handle(
      netswitch::http::HttpMethods::GET,
      [](std::shared_ptr<netswitch::http::server::HttpRequest> &request) {
        request->Reply(netswitch::http::HttpCodes::OK);
      });
  listener->Serve(config);
  std::this_thread::sleep_for(std::chrono::seconds(99999));
  listener->Stop();
}