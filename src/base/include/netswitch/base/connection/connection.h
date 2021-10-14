#ifndef NET_SWITCH_CONNECTION_H_
#define NET_SWITCH_CONNECTION_H_

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <atomic>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <string>

#include "netswitch/base/status.h"

namespace netswitch {
namespace base {
using SocketFD = int;

class Pipe {
 public:
  Pipe(SocketFD fd);

  virtual ~Pipe();

  virtual size_t Write(const void *data, size_t data_size);

  virtual size_t Read(void *buffer, size_t buffer_size);

  virtual void Close();

 protected:
  SocketFD fd_{-1};
};

using OnConnectCallback =
    std::function<void(std::shared_ptr<Pipe> pipe, const void *cb_data)>;

class PipeListener {
 public:
  PipeListener(const std::string &local_ip, const std::string &local_port,
               const OnConnectCallback &callback, void *cb_data);

  virtual ~PipeListener();

  Status Start();

  void Stop();

  bool IsRunning();

 private:
  Status Listen();

  void Process();

  std::string local_ip_;
  std::string local_port_;
  OnConnectCallback callback_;
  void *cb_data_;

  SocketFD server_fd_{-1};
  std::mutex state_lock_;
  bool running_{false};
  std::future<void> running_handle_;
};

class PipeConnector {
 public:
  PipeConnector() = default;

  PipeConnector(const std::string &local_ip, const std::string &local_port);

  std::shared_ptr<Pipe> Connect(const std::string &remote_ip,
                                const std::string &remote_port);

 private:
  std::string local_ip_;
  std::string local_port_;
};

}  // namespace base
}  // namespace netswitch

#endif  // NET_SWITCH_CONNECTION_H_