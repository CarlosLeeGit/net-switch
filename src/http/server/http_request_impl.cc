#include "netswitch/http/server/http_request_impl.h"

#include <string>

#include "netswitch/base/connection/pipe_line_reader.h"
#include "netswitch/base/log.h"
#include "netswitch/base/utils.h"
#include "netswitch/http/server/http_server.h"

#define SERVER_MAX_REQUEST_LINE_BYTE 4096
#define SERVER_MAX_HEADER_LINE_BYTE 4096
#define SERVER_MAX_HEADER_LINE_NUM 128

namespace netswitch {
namespace http {
namespace server {

using namespace base;

HttpRequestBodyImpl::HttpRequestBodyImpl(std::shared_ptr<Pipe> pipe)
    : pipe_(pipe) {}

size_t HttpRequestBodyImpl::Read(void *buffer, size_t buffer_size) {
  return pipe_->Read(buffer, buffer_size);
}

HttpRequestImpl::HttpRequestImpl(std::shared_ptr<base::Pipe> pipe)
    : pipe_(pipe) {}

std::shared_ptr<HttpRequestBody> HttpRequestImpl::Body() { return body_; }

std::unordered_multimap<std::string, std::string> HttpRequestImpl::Headers() {
  return headers_;
}

std::string HttpRequestImpl::Path() { return path_; }

HttpMethod HttpRequestImpl::Method() { return method_; }

Status HttpRequestImpl::Reply(HttpCode status) {
  return WriteHeaders(status, {{"Content-Length", "0"}});
}

Status HttpRequestImpl::Reply(
    HttpCode status,
    const std::unordered_multimap<std::string, std::string> &headers,
    const std::string &body) {
  WriteHeaders(status, headers);
  WriteBody();
  return STATUS_OK;
}

Status HttpRequestImpl::Parse() {
  PipeLineReader line_reader(pipe_);

  auto ret = ParseRequestLine(line_reader);
  if (ret != STATUS_OK) {
    NS_LOG_ERROR << "parse request line failed";
    return STATUS_FAIL;
  }

  ret = ParseRequestHeaders(line_reader);
  if (ret != STATUS_OK) {
    NS_LOG_ERROR << "parse headers failed";
    return STATUS_FAIL;
  }

  body_ = std::make_shared<HttpRequestBodyImpl>(pipe_);
  return STATUS_OK;
}

base::Status HttpRequestImpl::WriteHeaders(
    HttpCode status,
    const std::unordered_multimap<std::string, std::string> &headers) {
  std::stringstream reply_header;
  reply_header << "HTTP/1.1 " << status << "\r\n";
  for (auto header : headers) {
    reply_header << header.first << ":" << header.second << "\r\n";
  }

  reply_header << "\r\n";
  auto reply_header_str = reply_header.str();
  pipe_->Write(reply_header_str.data(), reply_header_str.size());

  return STATUS_OK;
}

base::Status HttpRequestImpl::WriteBody() { return STATUS_OK; }

Status HttpRequestImpl::ParseRequestLine(PipeLineReader &line_reader) {
  std::vector<uint8_t> buffer(SERVER_MAX_REQUEST_LINE_BYTE);
  auto line_len = line_reader.ReadLine(buffer.data(), buffer.size());
  if (line_len < 0) {
    NS_LOG_ERROR << "read request line failed";
    return STATUS_FAIL;
  }

  auto request_line = std::string((char *)(buffer.data()), line_len);
  auto data = StringSplit(request_line, ' ');
  if (data.size() != 3) {
    NS_LOG_ERROR << "wrong request line, should be 3 part";
    return STATUS_FAIL;
  }

  method_ = std::move(data[0]);
  path_ = std::move(data[1]);
  http_version_ = std::move(data[2]);
  if (http_version_ != "HTTP/1.0" && http_version_ != "HTTP/1.1") {
    NS_LOG_ERROR << "wrong http version [" << http_version_ << "]";
    return STATUS_FAIL;
  }

  return STATUS_OK;
}

Status HttpRequestImpl::ParseRequestHeaders(PipeLineReader &line_reader) {
  while (true) {
    std::vector<uint8_t> buffer(SERVER_MAX_HEADER_LINE_BYTE);
    auto line_len = line_reader.ReadLine(buffer.data(), buffer.size());
    if (line_len < 0) {
      NS_LOG_ERROR << "read header line failed";
      return STATUS_FAIL;
    }

    if (line_len == 0) {
      // read header over
      break;
    }

    auto header_line = std::string((char *)(buffer.data()), line_len);
    auto split_pos = header_line.find(':');
    if (split_pos == header_line.npos) {
      NS_LOG_ERROR << "header must be key:value, parse " << header_line
                   << " failed";
      break;
    }

    auto key = header_line.substr(0, split_pos);
    auto value = header_line.substr(split_pos + 1);
    headers_.insert(std::make_pair(StringStrim(key), StringStrim(value)));
  }

  return STATUS_OK;
}

}  // namespace server
}  // namespace http
}  // namespace netswitch