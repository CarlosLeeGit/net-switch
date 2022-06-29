#ifndef NET_SWITCH_PIPE_LINE_READER_H_
#define NET_SWITCH_PIPE_LINE_READER_H_

#include <memory>

#include "netswitch/base/connection/connection.h"
#include "netswitch/base/status.h"

namespace netswitch {
namespace base {

class PipeLineReader {
 public:
  PipeLineReader(std::shared_ptr<Pipe> pipe);

  ~PipeLineReader() = default;

  size_t ReadLine(void *buffer, size_t buffer_size);

 private:
  std::shared_ptr<Pipe> pipe_;
};

}  // namespace base
}  // namespace netswitch
#endif  // NET_SWITCH_PIPE_LINE_READER_H_