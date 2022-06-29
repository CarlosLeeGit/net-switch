#ifndef NET_SWITCH_LOG_H_
#define NET_SWITCH_LOG_H_

#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>

namespace netswtich {
namespace base {

enum class LogLevel { DEBUG, INFO, WARN, ERROR };

class LogStream {
 public:
  LogStream(const std::string &file, const std::string &func, size_t line,
            LogLevel level)
      : file_(file), func_(func), line_(line), level_(level) {}

  std::stringstream &GetStream();

  ~LogStream();

 private:
  std::string file_;
  std::string func_;
  size_t line_;
  LogLevel level_;
  std::stringstream msg_;

  static std::unordered_map<LogLevel, std::string> level_to_string_;
};

}  // namespace base
}  // namespace netswtich

#define NS_LOG(level) \
  netswtich::base::LogStream(__FILE__, __func__, __LINE__, level).GetStream()
#define NS_LOG_DEBUG NS_LOG(netswtich::base::LogLevel::DEBUG)
#define NS_LOG_INFO NS_LOG(netswtich::base::LogLevel::INFO)
#define NS_LOG_WARN NS_LOG(netswtich::base::LogLevel::WARN)
#define NS_LOG_ERROR NS_LOG(netswtich::base::LogLevel::ERROR)

#endif  // NET_SWITCH_LOG_H_