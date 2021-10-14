#ifndef NET_SWITCH_PREFIX_TREE_H_
#define NET_SWITCH_PREFIX_TREE_H_

#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>

namespace netswtich {
namespace base {

enum LogLevel { NS_LOG_DEBUG, NS_LOG_INFO, NS_LOG_WARN, NS_LOG_ERROR };

std::unordered_map<LogLevel, std::string> g_level_to_string = {
    {NS_LOG_DEBUG, "DEBUG"},
    {NS_LOG_INFO, "INFO"},
    {NS_LOG_WARN, "WARN"},
    {NS_LOG_ERROR, "ERROR"},
};

class LogStream {
 public:
  LogStream(const std::string &file, const std::string &func, size_t line,
            LogLevel level)
      : file_(file), func_(func), line_(line), level_(level) {}

  std::stringstream &GetStream() {
    msg_ << g_level_to_string[level_] << " "
         << "[" << file_ << ":" << func_ << ":" << line_ << "] ";
    return msg_;
  }

  ~LogStream() { std::cout << msg_.str() << std::endl; }

 private:
  std::string file_;
  std::string func_;
  size_t line_;
  LogLevel level_;
  std::stringstream msg_;
};

}  // namespace base
}  // namespace netswtich

#define NS_LOG(level) \
  netswtich::base::LogStream(__FILE__, __func__, __LINE__, level).GetStream()
#define NS_LOG_DEBUG NS_LOG(netswtich::base::LogLevel::NS_LOG_DEBUG)
#define NS_LOG_INFO NS_LOG(netswtich::base::LogLevel::NS_LOG_INFO)
#define NS_LOG_WARN NS_LOG(netswtich::base::LogLevel::NS_LOG_WARN)
#define NS_LOG_ERROR NS_LOG(netswtich::base::LogLevel::NS_LOG_ERROR)

#endif