#include "netswitch/base/log.h"

namespace netswtich {
namespace base {

std::unordered_map<LogLevel, std::string> LogStream::level_to_string_ = {
    {LogLevel::DEBUG, "DEBUG"},
    {LogLevel::INFO, "INFO"},
    {LogLevel::WARN, "WARN"},
    {LogLevel::ERROR, "ERROR"},
};

std::stringstream &LogStream::GetStream() {
  msg_ << "[" << level_to_string_[level_] << "]"
       << "[" << file_ << ":" << line_ << ":" << func_ << "] ";
  return msg_;
}

LogStream::~LogStream() { std::cout << msg_.str() << std::endl; }

}  // namespace base
}  // namespace netswtich