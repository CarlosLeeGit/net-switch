#ifndef NET_SWITCH_HTTP_LISTENER_H_
#define NET_SWITCH_HTTP_LISTENER_H_

#include <chrono>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

#include "http_common.h"
#include "netswitch/base/status.h"

namespace netswitch {
namespace http {
namespace server {

using HttpRequestHeaders = std::unordered_multimap<std::string, std::string>;

class HttpRequestBody {
 public:
  virtual size_t Read(void *buffer, size_t buffer_size) = 0;
};

class HttpRequest {
 public:
  virtual std::shared_ptr<HttpRequestBody> Body() = 0;

  virtual HttpRequestHeaders Headers() = 0;

  virtual std::string Path() = 0;

  virtual HttpMethod Method() = 0;

  virtual base::Status Reply(HttpCode status) = 0;

  virtual base::Status Reply(
      HttpCode status,
      const std::unordered_multimap<std::string, std::string> &headers,
      const std::string &body) = 0;
};

class ListenerConfig {
 public:
  template <typename Rep>
  ListenerConfig &SetRWTimeout(std::chrono::duration<Rep> timeout);

  ListenerConfig &SetKeepAliveCount(size_t count);

  template <typename Rep>
  ListenerConfig &SetKeepAliveTimeout(std::chrono::duration<Rep> timeout);

  std::chrono::milliseconds GetRWTimeout();

  size_t GetKeepAliveCount();

  std::chrono::milliseconds GetKeepAliveTimeout();

 private:
  std::chrono::milliseconds rw_timeout_;
  size_t keep_alive_count_;
  std::chrono::milliseconds keep_alive_timeout_;
};

template <typename Rep>
ListenerConfig &ListenerConfig::SetRWTimeout(
    std::chrono::duration<Rep> timeout) {
  rw_timeout_ = std::chrono::duration_cast<std::chrono::milliseconds>(timeout);
  return *this;
}

template <typename Rep>
ListenerConfig &ListenerConfig::SetKeepAliveTimeout(
    std::chrono::duration<Rep> timeout) {
  keep_alive_timeout_ =
      std::chrono::duration_cast<std::chrono::milliseconds>(timeout);
  return *this;
}

using HandleFunc = std::function<void(std::shared_ptr<HttpRequest> &request)>;

class HttpListener {
 public:
  static std::shared_ptr<HttpListener> GetListener(const std::string &url);

  virtual void Handle(const HttpMethod &method, const HandleFunc &func) = 0;

  virtual base::Status Serve(const ListenerConfig &config) = 0;

  virtual base::Status Stop() = 0;

  virtual std::string GetListenURL() = 0;

  virtual std::string GetScheme() = 0;

  virtual std::string GetServeIP() = 0;

  virtual std::string GetServePort() = 0;

  virtual std::string GetListenPath() = 0;
};
}  // namespace server
}  // namespace http
}  // namespace netswitch

#endif  // NET_SWITCH_HTTP_LISTENER_H_