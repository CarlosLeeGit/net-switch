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

class HttpRequest {
 public:
  virtual std::string Body() = 0;

  virtual std::unordered_multimap<std::string, std::string> Headers() = 0;

  virtual std::string Path() = 0;

  virtual HttpMethod Method() = 0;

  virtual Status Reply(HttpCode status) = 0;

  virtual Status Reply(
      HttpCode status,
      const std::unordered_multimap<std::string, std::string> &headers,
      const std::string &body) = 0;
};

using SSLSettingCallback = std::function<Status(SSL_CTX &ctx)>;

class HttpServerConfig {
 public:
  template <typename Rep>
  HttpServerConfig &SetRWTimeout(std::chrono::duration<Rep> timeout);

  HttpServerConfig &SetKeepAliveCount(size_t count);

  template <typename Rep>
  HttpServerConfig &SetKeepAliveTimeout(std::chrono::duration<Rep> timeout);

  HttpServerConfig &SetSSLSettingCallback(const SSLSettingCallback &func);

  std::chrono::duration<std::chrono::milliseconds> GetRWTimeout();

  size_t GetKeepAliveCount();

  std::chrono::duration<std::chrono::milliseconds> GetKeepAliveTimeout();

  SSLSettingCallback GetSSLSettingCallback();

 private:
  std::chrono::milliseconds rw_timeout_;
  size_t keep_alive_count_;
  std::chrono::milliseconds keep_alive_timeout_;
  SSLSettingCallback ssl_setting_cb_;
};

template <typename Rep>
HttpServerConfig &HttpServerConfig::SetRWTimeout(
    std::chrono::duration<Rep> timeout) {
  rw_timeout_ = std::chrono::duration_cast<std::chrono::milliseconds>(timeout);
  return *this;
}

template <typename Rep>
HttpServerConfig &HttpServerConfig::SetKeepAliveTimeout(
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

  virtual std::string GetListenURL() = 0;

  virtual std::string GetScheme() = 0;

  virtual std::string GetServerIP() = 0;

  virtual std::string GetServerPort() = 0;

  virtual std::string GetListenPath() = 0;

  virtual Status StartServer(const HttpServerConfig &config) = 0;
};
}  // namespace http
}  // namespace netswitch

#endif  // NET_SWITCH_HTTP_LISTENER_H_