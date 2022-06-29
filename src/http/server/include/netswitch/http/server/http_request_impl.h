#ifndef NET_SWITCH_HTTP_REQUEST_IMPL_H_
#define NET_SWITCH_HTTP_REQUEST_IMPL_H_

#include "netswitch/base/connection/connection.h"
#include "netswitch/base/connection/pipe_line_reader.h"
#include "netswitch/base/status.h"
#include "netswitch/http/http_common.h"
#include "netswitch/http/http_listener.h"

namespace netswitch {
namespace http {
namespace server {

class HttpRequestBodyImpl : public HttpRequestBody {
 public:
  HttpRequestBodyImpl(std::shared_ptr<base::Pipe> pipe);

  size_t Read(void *buffer, size_t buffer_size) override;

 private:
  std::shared_ptr<base::Pipe> pipe_;
};

class HttpRequestImpl : public HttpRequest {
 public:
  HttpRequestImpl(std::shared_ptr<base::Pipe> pipe);

  std::shared_ptr<HttpRequestBody> Body() override;

  std::unordered_multimap<std::string, std::string> Headers() override;

  std::string Path() override;

  HttpMethod Method() override;

  base::Status Reply(HttpCode status) override;

  base::Status Reply(
      HttpCode status,
      const std::unordered_multimap<std::string, std::string> &headers,
      const std::string &body) override;

  base::Status Parse();

 private:
  base::Status WriteHeaders(
      HttpCode status,
      const std::unordered_multimap<std::string, std::string> &headers);

  base::Status WriteBody();

  base::Status ParseRequestLine(base::PipeLineReader &line_reader);

  base::Status ParseRequestHeaders(base::PipeLineReader &line_reader);

  std::shared_ptr<base::Pipe> pipe_;

  std::string method_;
  std::string path_;
  std::string http_version_;
  HttpRequestHeaders headers_;
  std::shared_ptr<HttpRequestBody> body_;
};

}  // namespace server
}  // namespace http
}  // namespace netswitch

#endif  // NET_SWITCH_HTTP_REQUEST_IMPL_H_