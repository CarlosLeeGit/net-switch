#include "http_listener_impl.h"

#include "http_listener.h"
#include "http_server.h"

namespace netswitch {
namespace http {

HttpRequestImpl::HttpRequestImpl(
    std::shared_ptr<netswitch::HttpConnect> http_conn)
    : http_conn_(http_conn) {}

std::string HttpRequestImpl::Body() { return ""; }

std::unordered_multimap<std::string, std::string> HttpRequestImpl::Headers() {
  return {};
}

std::string HttpRequestImpl::Path() { return "/"; }

HttpMethod HttpRequestImpl::Method() { return HttpMethods::GET; }

Status HttpRequestImpl::Reply(HttpCode status) { return STATUS_OK; }

Status HttpRequestImpl::Reply(
    HttpCode status,
    const std::unordered_multimap<std::string, std::string> &headers,
    const std::string &body) {
  return STATUS_OK;
}

Status HttpRequestImpl::Parse() { return STATUS_OK; }

HttpListenerImpl::HttpListenerImpl(const std::string &url) : url_(url) {}

HttpListenerImpl::~HttpListenerImpl() {
  auto &server = HttpServer::GetInstance();
  server.DelListener(this);
}

void HttpListenerImpl::Handle(const HttpMethod &method,
                              const HandleFunc &func) {
  method_to_func_[method] = func;
}

Status HttpListenerImpl::Start() {
  auto &server = HttpServer::GetInstance();
  auto ret = server.AddListener(url_, shared_from_this());
  if (!ret) {
    return ret;
  }

  is_running_ = true;
  return STATUS_OK;
}

void HttpListenerImpl::Process(std::shared_ptr<HttpRequest> request) {
  auto method = request->Method();
  auto item = method_to_func_.find(method);
  if (item == method_to_func_.end()) {
    request->Reply(HttpCodes::NOT_FOUND);
    return;
  }

  item->second(request);
}

HttpListenerBuilder::HttpListenerBuilder(const std::string &url) : url_(url) {}

HttpListenerBuilder &HttpListenerBuilder::SetKeepAliveCount(size_t count) {
  keep_alive_count_ = count;
  return *this;
}

HttpListenerBuilder &HttpListenerBuilder::SetSSLSettingCallback(
    const SSLSettingCallback &func) {
  ssl_setting_cb_ = func;
  return *this;
}

std::shared_ptr<HttpListener> HttpListenerBuilder::Build() {
  return std::make_shared<HttpListenerImpl>(url_);
}

}  // namespace http
}  // namespace netswitch