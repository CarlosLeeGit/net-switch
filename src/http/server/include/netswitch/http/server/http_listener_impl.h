#ifndef NET_SWITCH_HTTP_LISTENER_IMPL_H_
#define NET_SWITCH_HTTP_LISTENER_IMPL_H_

#include "netswitch/base/connection/connection.h"
#include "netswitch/base/status.h"
#include "netswitch/http/http_common.h"
#include "netswitch/http/http_listener.h"
#include "netswitch/http/http_url.h"

namespace netswitch {
namespace http {
namespace server {

class HttpListenerImpl : public HttpListener,
                         public std::enable_shared_from_this<HttpListenerImpl> {
 public:
  HttpListenerImpl(std::shared_ptr<HttpURL> url);

  ~HttpListenerImpl();

  void Handle(const HttpMethod &method, const HandleFunc &func) override;

  base::Status Serve(const ListenerConfig &config) override;

  base::Status Stop() override;

  std::string GetListenURL() override;

  std::string GetScheme() override;

  std::string GetServeIP() override;

  std::string GetServePort() override;

  std::string GetListenPath() override;

  void Process(std::shared_ptr<HttpRequest> request);

 private:
  std::shared_ptr<HttpURL> url_;
  std::mutex func_lock_;
  std::unordered_map<std::string, HandleFunc> method_to_func_;
  bool is_running_{false};
};

}  // namespace server
}  // namespace http
}  // namespace netswitch

#endif  // NET_SWITCH_HTTP_LISTENER_IMPL_H_