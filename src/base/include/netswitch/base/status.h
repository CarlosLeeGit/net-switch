#ifndef NET_SWITCH_STATUS_H_
#define NET_SWITCH_STATUS_H_

#include <string>

namespace netswitch {
namespace base {

enum StatusCode {
  STATUS_OK,
  STATUS_FAIL,
  STATUS_EXIST,
  STATUS_NOTFOUND,
};

class Status {
 public:
  Status();

  Status(StatusCode code);

  Status(StatusCode code, const std::string &msg);

  bool operator!=(StatusCode code) const;

  bool operator==(StatusCode code) const;

  operator StatusCode() const;

  operator bool() const;

 private:
  StatusCode code_{STATUS_OK};
  std::string msg_;
};
}  // namespace base
}  // namespace netswitch

#endif  // NET_SWITCH_STATUS_H_