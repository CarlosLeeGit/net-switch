#include "http_common.h"

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

const HttpScheme HttpSchemes::HTTP = "http://";
const HttpScheme HttpSchemes::HTTPS = "https://";
}  // namespace http
}  // namespace netswitch