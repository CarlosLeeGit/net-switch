#ifndef NET_SWITCH_UTILS_H_
#define NET_SWITCH_UTILS_H_

#include <string>
#include <vector>

#include "netswitch/base/status.h"

namespace netswitch {

namespace base {

std::vector<std::string> StringSplit(const std::string &str,
                                     const char split_char);

std::string StringStrim(const std::string &str);

}  // namespace base
}  // namespace netswitch
#endif  // NET_SWITCH_UTILS_H_