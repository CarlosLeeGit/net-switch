#ifndef NET_SWITCH_HTTP_SERVER_H_
#define NET_SWITCH_HTTP_SERVER_H_

#include <mutex>
#include <unordered_map>

#include "netswitch/base/connection/connection.h"
#include "netswitch/base/prefix_tree.h"
#include "netswitch/base/status.h"
#include "netswitch/http/http_listener.h"

namespace netswitch {
namespace http {
namespace server {

using PathToListener =
    std::unordered_map<std::string, std::shared_ptr<HttpListener>>;

class HttpServerCtx {
 public:
  HttpServerCtx(const std::string &scheme, const std::string &ip,
                const std::string &port);

  ~HttpServerCtx();

  const std::string &GetScheme();

  base::Status AddListener(std::shared_ptr<HttpListener> listener);

  base::Status DelListener(HttpListener *listener);

  bool IsEmpty();

  void OnConnect(std::shared_ptr<base::Pipe> pipe);

 private:
  std::shared_ptr<base::PipeListener> pipe_listener_;
  std::string scheme_;
  std::string ip_;
  std::string port_;

  std::mutex listener_lock_;
  base::PrefixTree path_matcher_{"/"};
  PathToListener path_to_listener_;
};

using IpPortToCtx =
    std::unordered_map<std::string, std::shared_ptr<HttpServerCtx>>;

class HttpServer {
 public:
  static HttpServer &GetInstance();

  HttpServer(const HttpServer &) = delete;
  HttpServer(HttpServer &&) = delete;
  HttpServer &operator=(const HttpServer &) = delete;
  HttpServer &operator=(HttpServer &&) = delete;

  base::Status AddListener(std::shared_ptr<HttpListener> listener);

  base::Status DelListener(HttpListener *listener);

 private:
  HttpServer() = default;

  std::mutex ctx_map_lock_;
  IpPortToCtx ctx_map_;
};
}  // namespace server
}  // namespace http
}  // namespace netswitch

#endif  // NET_SWITCH_HTTP_SERVER_H_