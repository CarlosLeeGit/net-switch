#ifndef NET_SWITCH_HTTP_ROUTER_H_
#define NET_SWITCH_HTTP_ROUTER_H_

#include <mutex>
#include <unordered_map>

#include "netswitch/base/connection.h"
#include "netswitch/base/prefix_tree.h"
#include "netswitch/base/status.h"

namespace netswitch {
namespace http {

using UrlToListener =
    std::unordered_map<std::string, std::shared_ptr<HttpListener>>;

class HttpRouter {
 public:
  Status AddListener(std::shared_ptr<HttpListener> listener);

  void DelListener(HttpListener *listener);

  Status Route(std::shared_ptr<HttpRequest> request);

 private:
  std::mutex listener_map_lock_;
  UrlToListener listener_map_;
  PrefixTree path_match_;
};

class HttpServerCtx {
 public:
  HttpServerCtx(std::shared_ptr<ServerSocket> server_socket,
                const std::string &scheme)
      : server_socket_(server_socket), scheme_(scheme) {}

  const std::string &GetScheme();

  void AddRef();

  void DelRef();

  bool IsValid();

  Status Start(const HttpServerConfig &config);

 private:
  std::shared_ptr<ServerSocket> server_socket_;
  std::string scheme_;
};

using EndpointToSocket =
    std::unordered_map<std::string, std::shared_ptr<HttpServerSockCtx>>;

class HttpServer {
 public:
  static HttpServer &GetInstance();

  HttpServer(const HttpServer &) = delete;
  HttpServer(HttpServer &&) = delete;
  HttpServer &operator=(const HttpServer &) = delete;
  HttpServer &operator=(HttpServer &&) = delete;

  Status AddListener(std::shared_ptr<HttpListener> listener);

  Status StartListen(HttpListener *listener, const HttpServerConfig &config);

  Status DelListener(HttpListener *listener);

  void ClientSocketProcess(std::shared_ptr<SocketFD> fd, const void *cb_data);

 private:
  HttpServer() = default;

  Status AddContext(const std::string &scheme, const std::string &ip,
                    const std::string &port);

  void DelContext(const std::string &ip, const std::string &port);

  std::mutex socket_map_lock_;
  EndpointToSocket socket_map_;

  HttpRouter router_;
};
}  // namespace http
}  // namespace netswitch

#endif  // NET_SWITCH_HTTP_ROUTER_H_