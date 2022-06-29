#include "netswitch/base/connection/pipe_line_reader.h"

namespace netswitch {
namespace base {

PipeLineReader::PipeLineReader(std::shared_ptr<Pipe> pipe) : pipe_(pipe) {}

size_t PipeLineReader::ReadLine(void *buffer, size_t buffer_size) {
  auto ch_buffer = (char *)buffer;
  size_t buffer_write_pos = 0;
  uint8_t byte = 0;
  for (size_t buffer_write_pos = 0;; ++buffer_write_pos) {
    auto n = pipe_->Read(&byte, 1);
    if (n < 0) {
      return -1;
    }

    if (n == 0) {
      if (buffer_write_pos == 0) {
        return -1;
      }

      return buffer_write_pos;
    }

    if (byte == '\r') {
      // read '\n'
      pipe_->Read(&byte, 1);
      return buffer_write_pos;
    }

    if (buffer_write_pos >= buffer_size) {
      return -1;
    }

    ch_buffer[buffer_write_pos] = byte;
  }
}

}  // namespace base
}  // namespace netswitch