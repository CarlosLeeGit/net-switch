#include "netswitch/base/status.h"

namespace netswitch {
namespace base {

Status::Status() {}

Status::Status(StatusCode code) : code_(code) {}

Status::Status(StatusCode code, const std::string &msg)
    : code_(code), msg_(msg) {}

bool Status::operator!=(StatusCode code) const { return code_ != code; }

bool Status::operator==(StatusCode code) const { return code_ == code; }

Status::operator bool() const { return code_ == STATUS_OK; }

}  // namespace base
}  // namespace netswitch