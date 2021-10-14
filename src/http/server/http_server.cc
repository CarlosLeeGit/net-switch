#include "http_server.h"

#include "http_listener_impl.h"
#include "http_url.h"

namespace netswitch {
namespace http {

Status HttpRouter::AddListener(std::shared_ptr<HttpListener> listener) {
  auto url = listener->GetListenURL();
  std::lock_guard<std::mutex> lock(listener_map_lock_);
  auto item = listener_map_.find(url);
  if (item != listener_map_.end()) {
    return STATUS_EXIST;
  }

  listener_map_[url] = listener;
  path_match_.AddNode(url);
  return STATUS_OK;
}

void HttpRouter::DelListener(HttpListener *listener) {
  auto url = listener->GetListenURL();
  std::lock_guard<std::mutex> lock(listener_map_lock_);
  auto item = listener_map_.find(url);
  if (item == listener_map_.end()) {
    return;
  }

  listener_map_.erase(url);
  path_match_.DelNode(url);
}

Status HttpRouter::Route(std::shared_ptr<HttpRequest> request) {
  auto path = request->Path();
  std::string listener_url;
  auto ret = path_match_.Match(path, listener_url);
  if (!ret) {
    return ret;
  }

  std::lock_guard<std::mutex> lock(listener_map_lock_);
  auto item = listener_map_.find(listener_url);
  if (item == listener_map_.end()) {
    return STATUS_NOTFOUND;
  }

  auto listener_impl = std::static_pointer_cast<HttpListenerImpl>(item->second);
  listener_impl->Process(request);
  return STATUS_OK;
}

HttpServer &HttpServer::GetInstance() {
  static HttpServer instance;
  return instance;
}

Status HttpServer::AddListener(std::shared_ptr<HttpListener> listener) {
  auto scheme = listener->GetScheme();
  auto ip = listener->GetServerIP();
  auto port = listener->GetServerPort();
  auto ret = AddServerSocket(scheme, ip, port);
  if (ret != STATUS_OK) {
    return ret;
  }

  ret = router_.AddListener(listener);
  if (ret != STATUS_OK) {
    DelServerSocket(ip, port);
    return ret;
  }

  return STATUS_OK;
}

Status HttpServer::StartListen(HttpListener *listener,
                               const HttpServerConfig &config) {
  std::lock_guard<std::mutex> lock(socket_map_lock_);
  auto endpoint = listener->GetServerIP() + ":" + listener->GetServerPort();
  auto item = socket_map_.find(endpoint);
  if (item == socket_map_.end()) {
    return STATUS_NOTFOUND;
  }

  return item->second->Start(config);
}

Status HttpServer::DelListener(HttpListener *listener) {
  // Del route
  // Del server reference
  router_.DelListener(listener);
  DelServerSocket(listener->GetServerIP(), listener->GetServerPort());
  return STATUS_OK;
}

void HttpServer::ClientSocketProcess(std::shared_ptr<SocketFD> fd,
                                     const void *cb_data) {
  if (cb_data == nullptr) {
    return;
  }

  auto scheme = (const HttpScheme *)cb_data;
  auto conn = HttpConnect::CreateConnect(*scheme, std::move(fd));
  auto request = std::make_shared<HttpRequestImpl>(conn);
  auto ret = request->Parse();
  if (!ret) {
    request->Reply(HttpCodes::BAD_REQUEST);
    return;
  }

  ret = router_.Route(request);
  if (ret != STATUS_OK) {
    request->Reply(HttpCodes::BAD_REQUEST);
    return;
  }
}

Status HttpServer::AddContext(const std::string &scheme, const std::string &ip,
                              const std::string &port) {
  std::lock_guard<std::mutex> lock(socket_map_lock_);
  std::shared_ptr<HttpServerCtx> sock_ctx;
  auto endpoint = ip + ":" + port;
  auto item = socket_map_.find(endpoint);
  if (item != socket_map_.end()) {
    sock_ctx = item->second;
    if (sock_ctx->GetScheme() != scheme) {
      return {STATUS_FAIL, "can't start different scheme at same address"};
    }
  } else {
    auto client_socket_cb =
        std::bind(&HttpServer::ClientSocketProcess, this, std::placeholders::_1,
                  std::placeholders::_2);
    const void *data = nullptr;
    if (scheme == HttpSchemes::HTTP) {
      data = &HttpSchemes::HTTP;
    } else {
      data = &HttpSchemes::HTTPS;
    }

    auto port_num = std::stoi(port);
    auto server_socket =
        std::make_shared<ServerSocket>(ip, port_num, client_socket_cb, data);
    sock_ctx = std::make_shared<HttpServerCtx>(server_socket, scheme);
    socket_map_[endpoint] = sock_ctx;
  }

  sock_ctx->AddRef();
  return STATUS_OK;
}

void HttpServer::DelContext(const std::string &ip, const std::string &port) {
  std::lock_guard<std::mutex> lock(socket_map_lock_);
  auto endpoint = ip + ":" + port;
  auto item = socket_map_.find(endpoint);
  if (item == socket_map_.end()) {
    return;
  }

  auto &ctx = item->second;
  ctx->DelRef();
  if (!ctx->IsValid()) {
    socket_map_.erase(item);
  }
}

}  // namespace http
}  // namespace netswitch