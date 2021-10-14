#ifndef NET_SWITCH_HTTP_LISTENER_IMPL_H_
#define NET_SWITCH_HTTP_LISTENER_IMPL_H_

#include "http_listener.h"
#include "netswitch/http/http_connect.h"

namespace netswitch {
namespace http {

class HttpRequestImpl : public HttpRequest {
 public:
  HttpRequestImpl(std::shared_ptr<netswitch::HttpConnect> http_conn);

  std::string Body() override;

  std::unordered_multimap<std::string, std::string> Headers() override;

  std::string Path() override;

  HttpMethod Method() override;

  Status Reply(HttpCode status) override;

  Status Reply(HttpCode status,
               const std::unordered_multimap<std::string, std::string> &headers,
               const std::string &body) override;

  Status Parse();

 private:
  std::shared_ptr<netswitch::HttpConnect> http_conn_;
};

class HttpListenerImpl : public HttpListener,
                         public std::enable_shared_from_this<HttpListenerImpl> {
 public:
  HttpListenerImpl(const std::string &url);

  ~HttpListenerImpl();

  void Handle(const HttpMethod &method, const HandleFunc &func) override;

  std::string GetListenURL() override;

  std::string GetScheme() override;

  std::string GetServerIP() override;

  std::string GetServerPort() override;

  std::string GetListenPath() override;

  Status StartServer(const HttpServerConfig &config) override;

  void Process(std::shared_ptr<HttpRequest> request);

 private:
  std::string url_;
  std::unordered_map<std::string, HandleFunc> method_to_func_;
  bool is_running_{false};
};

}  // namespace http
}  // namespace netswitch

#endif  // NET_SWITCH_HTTP_LISTENER_IMPL_H_