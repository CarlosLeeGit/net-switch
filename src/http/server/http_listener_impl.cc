#include "netswitch/http/server/http_listener_impl.h"

#include "netswitch/base/log.h"
#include "netswitch/http/server/http_server.h"

namespace netswitch {
namespace http {
namespace server {

using namespace base;

ListenerConfig &ListenerConfig::SetKeepAliveCount(size_t count) {
  keep_alive_count_ = count;
  return *this;
}

std::chrono::milliseconds ListenerConfig::GetRWTimeout() { return rw_timeout_; }

size_t ListenerConfig::GetKeepAliveCount() { return keep_alive_count_; }

std::chrono::milliseconds ListenerConfig::GetKeepAliveTimeout() {
  return keep_alive_timeout_;
}

std::shared_ptr<HttpListener> HttpListener::GetListener(
    const std::string &url) {
  auto http_url = std::make_shared<HttpURL>(url);
  auto ret = http_url->Parse();
  if (ret != STATUS_OK) {
    NS_LOG_ERROR << "parse url failed, ret " << ret.ToString();
    return nullptr;
  }

  return std::make_shared<HttpListenerImpl>(http_url);
}

HttpListenerImpl::HttpListenerImpl(std::shared_ptr<HttpURL> url) : url_(url) {}

HttpListenerImpl::~HttpListenerImpl() {
  auto &server = HttpServer::GetInstance();
  server.DelListener(this);
}

void HttpListenerImpl::Handle(const HttpMethod &method,
                              const HandleFunc &func) {
  std::lock_guard<std::mutex> lock(func_lock_);
  method_to_func_[method] = func;
}

Status HttpListenerImpl::Serve(const ListenerConfig &config) {
  if (is_running_) {
    return STATUS_OK;
  }

  auto &server = HttpServer::GetInstance();
  auto ret = server.AddListener(shared_from_this());
  if (ret != STATUS_OK) {
    return ret;
  }

  is_running_ = true;
  return STATUS_OK;
}

base::Status HttpListenerImpl::Stop() {
  if (!is_running_) {
    return STATUS_OK;
  }

  auto &server = HttpServer::GetInstance();
  auto ret = server.DelListener(this);
  if (ret != STATUS_OK) {
    return ret;
  }

  is_running_ = false;
  return STATUS_OK;
}

std::string HttpListenerImpl::GetListenURL() { return url_->ToString(); }

std::string HttpListenerImpl::GetScheme() { return url_->GetScheme(); }

std::string HttpListenerImpl::GetServeIP() { return url_->GetIp(); }

std::string HttpListenerImpl::GetServePort() { return url_->GetPort(); }

std::string HttpListenerImpl::GetListenPath() { return url_->GetPath(); }

void HttpListenerImpl::Process(std::shared_ptr<HttpRequest> request) {
  auto method = request->Method();
  std::lock_guard<std::mutex> lock(func_lock_);
  auto item = method_to_func_.find(method);
  if (item == method_to_func_.end()) {
    request->Reply(HttpCodes::NOT_FOUND);
    return;
  }

  item->second(request);
}

}  // namespace server
}  // namespace http
}  // namespace netswitch