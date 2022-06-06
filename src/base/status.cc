#include "netswitch/base/status.h"

namespace netswitch {
namespace base {

Status::Status() {}

Status::Status(StatusCode code) : code_(code) {}

Status::Status(StatusCode code, const std::string &msg)
    : code_(code), msg_(msg) {}

bool Status::operator!=(StatusCode code) const { return code_ != code; }

bool Status::operator==(StatusCode code) const { return code_ == code; }

Status::operator StatusCode() const { return code_; }

}  // namespace base
}  // namespace netswitch