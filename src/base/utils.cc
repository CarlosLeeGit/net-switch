#include "netswitch/base/utils.h"

namespace netswitch {

namespace base {

std::vector<std::string> StringSplit(const std::string &str,
                                     const char split_char) {
  if (str.empty()) {
    return {};
  }

  std::vector<std::string> result;
  size_t start_pos = 0;
  while (true) {
    auto end_pos = str.find(split_char, start_pos);
    if (end_pos == str.npos) {
      auto sub_str = str.substr(start_pos);
      result.push_back(sub_str);
      return result;
    }

    auto sub_str = str.substr(start_pos, end_pos - start_pos);
    result.push_back(sub_str);
    start_pos = end_pos + 1;
    if (start_pos >= str.size()) {
      return result;
    }
  }

  return result;
}

std::string StringStrim(const std::string &str) {
  auto strim_str = str;
  strim_str.erase(0, strim_str.find_first_not_of(' '));
  strim_str.erase(strim_str.find_last_not_of(' ') + 1);
  return strim_str;
}

}  // namespace base
}  // namespace netswitch