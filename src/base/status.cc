#include "netswitch/base/status.h"

#include <sstream>

namespace netswitch {
namespace base {

Status::Status() {}

Status::Status(StatusCode code) : code_(code) {}

Status::Status(StatusCode code, const std::string &msg)
    : code_(code), msg_(msg) {}

bool Status::operator!=(StatusCode code) const { return code_ != code; }

bool Status::operator==(StatusCode code) const { return code_ == code; }

Status::operator StatusCode() const { return code_; }

std::string Status::ToString() {
  std::stringstream ss;
  ss << "code: " << code_ << ", msg: " << msg_;
  return ss.str();
}

}  // namespace base
}  // namespace netswitch