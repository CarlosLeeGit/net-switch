#include "netswitch/http/server/http_server.h"

#include "netswitch/base/log.h"
#include "netswitch/http/http_url.h"
#include "netswitch/http/server/http_listener_impl.h"
#include "netswitch/http/server/http_request_impl.h"

namespace netswitch {
namespace http {
namespace server {

using namespace base;

HttpServerCtx::HttpServerCtx(const std::string &scheme, const std::string &ip,
                             const std::string &port)
    : scheme_(scheme), ip_(ip), port_(port) {
  NS_LOG_INFO << "new [" << scheme << "] server at " << ip << ":" << port;
}

HttpServerCtx::~HttpServerCtx() { pipe_listener_->Stop(); }

const std::string &HttpServerCtx::GetScheme() { return scheme_; }

Status HttpServerCtx::AddListener(std::shared_ptr<HttpListener> listener) {
  auto path = listener->GetListenPath();
  std::lock_guard<std::mutex> lock(listener_lock_);
  if (path_to_listener_.find(path) != path_to_listener_.end()) {
    return STATUS_EXIST;
  }

  path_matcher_.AddNode(path);
  path_to_listener_[path] = listener;

  if (pipe_listener_ == nullptr) {
    pipe_listener_ = std::make_shared<PipeListener>(
        ip_, port_, [this](std::shared_ptr<Pipe> pipe) { OnConnect(pipe); });
    pipe_listener_->Start();
  }

  return STATUS_OK;
}

Status HttpServerCtx::DelListener(HttpListener *listener) {
  auto path = listener->GetListenPath();
  std::lock_guard<std::mutex> lock(listener_lock_);
  if (path_to_listener_.empty()) {
    return STATUS_OK;
  }

  path_matcher_.DelNode(path);
  path_to_listener_.erase(path);
  return STATUS_OK;
}

bool HttpServerCtx::IsEmpty() {
  std::lock_guard<std::mutex> lock(listener_lock_);
  return path_to_listener_.empty();
}

void HttpServerCtx::OnConnect(std::shared_ptr<Pipe> pipe) {
  auto http_request = std::make_shared<HttpRequestImpl>(pipe);
  auto ret = http_request->Parse();
  if (ret != STATUS_OK) {
    http_request->Reply(HttpCodes::BAD_REQUEST);
    return;
  }

  auto req_path = http_request->Path();
  std::lock_guard<std::mutex> lock(listener_lock_);
  std::string listener_path;
  ret = path_matcher_.Match(req_path, listener_path);
  if (ret != STATUS_OK) {
    http_request->Reply(HttpCodes::NOT_FOUND);
    return;
  }

  auto listener_item = path_to_listener_.find(listener_path);
  if (listener_item == path_to_listener_.end()) {
    http_request->Reply(HttpCodes::NOT_FOUND);
    return;
  }

  auto listener =
      std::dynamic_pointer_cast<HttpListenerImpl>(listener_item->second);
  if (listener == nullptr) {
    http_request->Reply(HttpCodes::NOT_FOUND);
    return;
  }

  listener->Process(http_request);
}

HttpServer &HttpServer::GetInstance() {
  static HttpServer instance;
  return instance;
}

Status HttpServer::AddListener(std::shared_ptr<HttpListener> listener) {
  auto scheme = listener->GetScheme();
  auto ip = listener->GetServeIP();
  auto port = listener->GetServePort();
  auto ip_port = ip + ":" + port;
  std::shared_ptr<HttpServerCtx> ctx;
  {
    std::lock_guard<std::mutex> lock(ctx_map_lock_);
    auto ctx_item = ctx_map_.find(ip_port);
    if (ctx_item == ctx_map_.end()) {
      ctx = std::make_shared<HttpServerCtx>(scheme, ip, port);
      ctx_map_[ip_port] = ctx;
    } else {
      ctx = ctx_item->second;
      if (ctx->GetScheme() != scheme) {
        NS_LOG_ERROR << "register listener failed, ip " << ip << ", port "
                     << port << " used in scheme " << scheme;
        return STATUS_FAIL;
      }
    }
  }

  return ctx->AddListener(listener);
}

Status HttpServer::DelListener(HttpListener *listener) {
  auto ip = listener->GetServeIP();
  auto port = listener->GetServePort();
  auto ip_port = ip + ":" + port;
  std::lock_guard<std::mutex> lock(ctx_map_lock_);
  auto ctx_item = ctx_map_.find(ip_port);
  if (ctx_item == ctx_map_.end()) {
    return STATUS_NOTFOUND;
  }

  auto &ctx = ctx_item->second;
  auto ret = ctx->DelListener(listener);
  if (ret != STATUS_OK) {
    return ret;
  }

  if (ctx->IsEmpty()) {
    ctx_map_.erase(ip_port);
  }

  return STATUS_OK;
}

}  // namespace server
}  // namespace http
}  // namespace netswitch