#include "netswitch/http/http_common.h"

#include <unordered_map>

namespace netswitch {
namespace http {

const HttpMethod HttpMethods::HEAD = "HEAD";
const HttpMethod HttpMethods::GET = "GET";
const HttpMethod HttpMethods::POST = "POST";
const HttpMethod HttpMethods::PUT = "PUT";
const HttpMethod HttpMethods::DELETE = "DELETE";
const HttpMethod HttpMethods::CONNECT = "CONNECT";
const HttpMethod HttpMethods::OPTIONS = "OPTIONS";
const HttpMethod HttpMethods::TRACE = "TRACE";

const HttpCode HttpCodes::OK = 200;
const HttpCode HttpCodes::BAD_REQUEST = 400;
const HttpCode HttpCodes::NOT_FOUND = 404;

static std::unordered_map<HttpCode, std::string> http_code_to_string = {
    {HttpCodes::OK, "OK"},
    {HttpCodes::BAD_REQUEST, "Bad Request"},
    {HttpCodes::NOT_FOUND, "Not Found"},
};

std::string HttpCodesToString(HttpCode code) {
  auto item = http_code_to_string.find(code);
  if (item == http_code_to_string.end()) {
    return "";
  }

  return item->second;
}

const HttpScheme HttpSchemes::HTTP = "http://";
const HttpScheme HttpSchemes::HTTPS = "https://";
}  // namespace http
}  // namespace netswitch