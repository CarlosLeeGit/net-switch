#include "http_url.h"

#include <regex>

#include "http_common.h"

namespace netswitch {
namespace http {

HttpURL::HttpURL(const std::string &url) : url_(url) {}

Status HttpURL::Parse() {
  std::regex url_pattern(
      "(https?://)([\\w\\-\\.]+)(:([0-9]+))?(/|(/[\\w\\-\\.]+)+)?");
  std::smatch url_match_result;
  auto ret = std::regex_match(url_, url_match_result, url_pattern);
  if (!ret) {
    return {STATUS_FAIL, "url not valid"};
  }

  if (url_match_result.size() != 7) {
    return {STATUS_FAIL, "url not valid"};
  }

  scheme_ = url_match_result[1];
  ip_ = url_match_result[2];
  port_ = url_match_result[4];
  path_ = url_match_result[5];

  if (port_.empty()) {
    port_ = scheme_ == HttpSchemes::HTTP ? "80" : "443";
  }

  if (path_.empty()) {
    path_ = "/";
  }

  return STATUS_OK;
}

std::string HttpURL::GetScheme() { return scheme_; }

std::string HttpURL::GetIp() { return ip_; }

std::string HttpURL::GetPort() { return port_; }

std::string HttpURL::GetPath() { return path_; }

std::string HttpURL::ToString() { return url_; }

}  // namespace http
}  // namespace netswitch