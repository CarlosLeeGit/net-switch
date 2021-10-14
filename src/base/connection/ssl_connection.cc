#include "netswitch/base/connection/ssl_connection.h"

#include "netswitch/base/log.h"

namespace netswitch {
namespace base {

SSLCtx::SSLCtx(bool is_server) {
  const SSL_METHOD *method;
  if (is_server) {
    method = TLS_server_method();
  } else {
    method = TLS_client_method();
  }

  ctx_ = SSL_CTX_new(method);
}

SSLCtx::~SSLCtx() { SSL_CTX_free(ctx_); }

SSL_CTX *SSLCtx::GetRaw() { return ctx_; }

Status SSLCtx::LoadCert(const void *cert_data, size_t data_size) {
  auto bio = BIO_new_mem_buf(cert_data, data_size);
  if (bio == nullptr) {
    return STATUS_FAIL;
  }

  auto cert = PEM_read_bio_X509(bio, nullptr, 0, nullptr);
  if (cert == nullptr) {
    return STATUS_FAIL;
  }

  auto ret = SSL_CTX_use_certificate(ctx_, cert);
  if (ret != 1) {
    return STATUS_FAIL;
  }

  X509_free(cert);
  BIO_free(bio);
  return STATUS_OK;
}

Status SSLCtx::LoadCertFile(const std::string &cert_file) {
  auto ret = 1;
  if (is_server_) {
    ret = SSL_CTX_use_certificate_chain_file(ctx_, cert_file.c_str());
  } else {
    ret =
        SSL_CTX_use_certificate_file(ctx_, cert_file.c_str(), SSL_FILETYPE_PEM);
  }

  if (ret != 1) {
    return STATUS_FAIL;
  }

  return STATUS_OK;
}

Status SSLCtx::LoadPrivKey(const void *key_data, size_t data_size) {
  auto bio = BIO_new_mem_buf(key_data, data_size);
  if (bio == nullptr) {
    return STATUS_FAIL;
  }

  auto priv_key = PEM_read_bio_PrivateKey(bio, nullptr, 0, nullptr);
  if (priv_key == nullptr) {
    return STATUS_FAIL;
  }

  auto ret = SSL_CTX_use_PrivateKey(ctx_, priv_key);
  if (ret != 1) {
    return STATUS_FAIL;
  }

  EVP_PKEY_free(priv_key);
  BIO_free(bio);
  return STATUS_OK;
}

Status SSLCtx::LoadPrivKeyFile(const std::string &priv_key_file) {
  auto ret = SSL_CTX_use_PrivateKey_file(ctx_, priv_key_file.c_str(),
                                         SSL_FILETYPE_PEM);
  if (ret != 1) {
    return STATUS_FAIL;
  }

  return STATUS_OK;
}

SSLPipe::SSLPipe(SocketFD fd, SSLSetupFunc &func) : Pipe(fd) {
  func(ctx_);
  ssl_ = SSL_new(ctx_.GetRaw());
  SSL_set_fd(ssl_, fd);
  SSL_connect(ssl_);
}

SSLPipe::~SSLPipe() { SSL_free(ssl_); }

size_t SSLPipe::Write(const void *data, size_t data_size) {
  size_t written = 0;
  auto ret = SSL_write_ex(ssl_, data, data_size, &written);
  if (ret != 0) {
    NS_LOG_ERROR << "ssl write failed, err " << ret;
    return 0;
  }

  return written;
}

size_t SSLPipe::Read(void *buffer, size_t buffer_size) {
  size_t read_size = 0;
  auto ret = SSL_read_ex(ssl_, buffer, buffer_size, &read_size);
  if (ret != 0) {
    NS_LOG_ERROR << "ssl write failed, err " << ret;
    return 0;
  }

  return read_size;
}

}  // namespace base
}  // namespace netswitch