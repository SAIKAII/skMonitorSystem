#ifndef skMNTSYS_INCLUDE_WEB_H_
#define skMNTSYS_INCLUDE_WEB_H_

#include "config.h"
#include "utility.h"
#include "connection.h"
#include "websocketSSL.h"
#include "endpoint.h"
#include <boost/asio.hpp>
#include <regex>
#include <memory>
#include <boost/asio/ssl.hpp>

namespace asio = boost::asio;
using error_code = boost::system::error_code;

class Web : public std::enable_shared_from_this<Web>{
public:
  Web(unsigned short port, const std::string &cert_file,
    const std::string &private_key_file) noexcept : config_(port), context_(asio::ssl::context::sslv23){
      context_.use_certificate_chain_file(cert_file);
      context_.use_private_key_file(private_key_file, asio::ssl::context::pem);
    }
  void start();
  void stop() noexcept;

  std::unordered_set<std::shared_ptr<WebSocketSSL>> get_wss() noexcept;

  ~Web() noexcept {};

private:
  std::shared_ptr<asio::io_service> io_service_;
  std::unique_ptr<asio::ip::tcp::acceptor> acceptor_;
  Config config_;
  asio::ssl::context context_;

  unsigned short bind();
  void accept_and_run();
  void accept();
  void read_and_parse(std::shared_ptr<Connection> connection);
  void stop_accept() noexcept;

  // WebSocketSSL
  void write_handshake(std::shared_ptr<Connection> connection);
  bool generate_handshake(std::shared_ptr<asio::streambuf> &write_buffer, const std::shared_ptr<Connection> connection);

  void delete_disconnection();

  // HTTPS
  void http_resolve(std::shared_ptr<Connection> connection, std::size_t bytes_transferred);
  void respond(std::shared_ptr<Connection> connection);
  void keep_alive_connect(std::shared_ptr<Connection> connection);
};
#endif
