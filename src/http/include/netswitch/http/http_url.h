#ifndef NET_SWITCH_HTTP_URL_H_
#define NET_SWITCH_HTTP_URL_H_

#include <string>

#include "status.h"

namespace netswitch {
namespace http {
class HttpURL {
 public:
  HttpURL(const std::string &url);

  Status Parse();

  std::string GetScheme();

  std::string GetIp();

  std::string GetPort();

  std::string GetPath();

  std::string ToString();

 private:
  std::string url_;
  std::string scheme_;
  std::string ip_;
  std::string port_;
  std::string path_;
};

}  // namespace http
}  // namespace netswitch

#endif  // NET_SWITCH_HTTP_URL_H_