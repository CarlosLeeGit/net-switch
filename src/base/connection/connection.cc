#include "netswitch/base/connection/connection.h"

#include <arpa/inet.h>
#include <memory.h>
#include <netinet/in.h>

#include "netswitch/base/log.h"
#include "netswitch/base/thread_pool.h"

namespace netswitch {
namespace base {

Pipe::Pipe(SocketFD fd) : fd_(fd) {}

Pipe::~Pipe() { Close(); }

size_t Pipe::Write(const void *data, size_t data_size) {
  return send(fd_, data, data_size, 0);
}

size_t Pipe::Read(void *buffer, size_t buffer_size) {
  return recv(fd_, buffer, buffer_size, 0);
}

void Pipe::Close() {
  if (fd_ == -1) {
    return;
  }

  close(fd_);
  fd_ = -1;
}

PipeListener::PipeListener(const std::string &local_ip,
                           const std::string &local_port,
                           const OnConnectCallback &callback)
    : local_ip_(local_ip), local_port_(local_port), callback_(callback) {}

PipeListener::~PipeListener() {
  running_ = false;
  if (server_fd_ != -1) {
    close(server_fd_);
  }
}

Status PipeListener::Start() {
  std::lock_guard<std::mutex> lock(state_lock_);
  if (running_) {
    return STATUS_OK;
  }

  auto ret = Listen();
  if (ret != STATUS_OK) {
    return ret;
  }

  running_ = true;
  return STATUS_OK;
}

void PipeListener::Stop() {
  std::lock_guard<std::mutex> lock(state_lock_);
  if (!running_) {
    return;
  }

  running_ = false;
  shutdown(server_fd_, SHUT_RDWR);
  close(server_fd_);
  server_fd_ = -1;
  running_handle_.get();
  NS_LOG_INFO << "server socket stop success";
}

bool PipeListener::IsRunning() { return running_; }

Status PipeListener::Listen() {
  server_fd_ = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd_ == -1) {
    return STATUS_FAIL;
  }

  struct sockaddr_in server_addr;
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  inet_aton(local_ip_.c_str(), &server_addr.sin_addr);
  auto port_num = std::stoi(local_port_);
  server_addr.sin_port = htons((uint16_t)port_num);
  auto ret = bind(server_fd_, (const struct sockaddr *)&server_addr,
                  sizeof(server_addr));
  if (ret == -1) {
    return STATUS_FAIL;
  }

  ret = listen(server_fd_, 10);
  if (ret == -1) {
    return STATUS_FAIL;
  }

  auto process = std::bind(&PipeListener::Process, this);
  running_handle_ = ThreadPool::GetInstance().Submit(process);
  return STATUS_OK;
}

void PipeListener::Process() {
  while (running_) {
    auto client_fd = accept(server_fd_, nullptr, nullptr);
    if (client_fd == -1) {
      continue;
    }

    auto pipe = std::make_shared<Pipe>(client_fd);
    ThreadPool::GetInstance().Submit(callback_, pipe);
  }
}

PipeConnector::PipeConnector(const std::string &local_ip,
                             const std::string &local_port)
    : local_ip_(local_ip), local_port_(local_port) {}

std::shared_ptr<Pipe> PipeConnector::Connect(const std::string &remote_ip,
                                             const std::string &remote_port) {
  auto sock = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in server_addr;
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  inet_aton(remote_ip.c_str(), &server_addr.sin_addr);
  server_addr.sin_port = htons((uint16_t)std::stoi(remote_port));
  auto ret = connect(sock, (sockaddr *)&server_addr, sizeof(server_addr));
  if (ret != 0) {
    return nullptr;
  }

  return std::make_shared<Pipe>(sock);
}

}  // namespace base
}  // namespace netswitch