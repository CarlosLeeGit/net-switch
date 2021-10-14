#ifndef NET_SWITCH_SSL_CONNECTION_H_
#define NET_SWITCH_SSL_CONNECTION_H_

#include "netswitch/base/connection/connection.h"
#include "openssl/ssl.h"

namespace netswitch {
namespace base {

class SSLCtx {
 public:
  SSLCtx(bool is_server = true);

  ~SSLCtx();

  SSL_CTX *GetRaw();

  Status LoadCert(const void *cert_data, size_t data_size);

  Status LoadCertFile(const std::string &cert_file);

  Status LoadPrivKey(const void *key_data, size_t data_size);

  Status LoadPrivKeyFile(const std::string &priv_key_file);

 private:
  bool is_server_;
  SSL_CTX *ctx_;
};

using SSLSetupFunc = std::function<void(SSLCtx &ctx)>;

class SSLPipe : public Pipe {
 public:
  SSLPipe(SocketFD fd, SSLSetupFunc &func);

  ~SSLPipe();

  size_t Write(const void *data, size_t data_size) override;

  size_t Read(void *buffer, size_t buffer_size) override;

 private:
  SSLCtx ctx_;
  SSL *ssl_;
};

class SSLPipeListener : public PipeListener {};

class SSLPipeConnector : public PipeConnector {};

}  // namespace base
}  // namespace netswitch

#endif  // NET_SWITCH_SSL_CONNECTION_H_