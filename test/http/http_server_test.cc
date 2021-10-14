// #include "http_listener.h"

// #include <gtest/gtest.h>

// #include <thread>

// class HttpListenerTest : public testing::Test {
//  protected:
//   void SetUp() override {}

//   void TearDown() override {}
// };

// TEST_F(HttpListenerTest, HttpTest) {
//   netswitch::HttpListenerBuilder builder(
//       "http://127.0.0.1:32323/v1/test/jsjsjsj");
//   builder.SetRWTimeout(std::chrono::seconds(10));
//   builder.SetKeepAliveCount(1);
//   builder.SetKeepAliveTimeout(std::chrono::seconds(1));
//   auto listener = builder.Build();
//   listener->Handle(netswitch::HttpMethods::GET,
//                    [](std::shared_ptr<netswitch::HttpRequest> &request) {
//                      request->Reply(netswitch::HttpCodes::OK);
//                    });
//   listener->Start();
//   std::this_thread::sleep_for(std::chrono::seconds(99999));
//   listener->Stop();
// }