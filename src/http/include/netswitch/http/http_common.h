#ifndef NET_SWITCH_HTTP_COMMON_H_
#define NET_SWITCH_HTTP_COMMON_H_

#include <string>

namespace netswitch {
namespace http {

using HttpMethod = std::string;

class HttpMethods {
 public:
  static const HttpMethod HEAD;
  static const HttpMethod GET;
  static const HttpMethod POST;
  static const HttpMethod PUT;
  static const HttpMethod DELETE;
  static const HttpMethod CONNECT;
  static const HttpMethod OPTIONS;
  static const HttpMethod TRACE;
};

using HttpCode = uint32_t;

class HttpCodes {
 public:
  static const HttpCode OK;
  static const HttpCode BAD_REQUEST;
  static const HttpCode NOT_FOUND;
};

std::string HttpCodesToString(HttpCode code);

using HttpScheme = std::string;

class HttpSchemes {
 public:
  static const HttpScheme HTTP;
  static const HttpScheme HTTPS;
};

}  // namespace http
}  // namespace netswitch

#endif  // NET_SWITCH_HTTP_COMMON_H_