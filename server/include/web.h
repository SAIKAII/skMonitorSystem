#ifndef skMNTSYS_INCLUDE_WEB_H_
#define skMNTSYS_INCLUDE_WEB_H_

#include "endpoint.h"
#include "config.h"
#include "utility.h"
#include "connection.h"
#include "http.h"
// #include "websocket.h"
#include <boost/asio.hpp>
#include <regex>
#include <memory>

namespace asio = boost::asio;
using error_code = boost::system::error_code;

class Web{
public:
  Web(unsigned short port) noexcept : config_(port){
    endpoint_ = Endpoint::get_instance();
  }
  void start();
  void stop() noexcept;
  ~Web() noexcept {};

private:
  Endpoint *endpoint_;
  std::shared_ptr<asio::io_service> io_service_;
  std::unique_ptr<asio::ip::tcp::acceptor> acceptor_;
  Config config_;

  unsigned short bind();
  void accept_and_run();
  void accept();
  void read_and_parse(std::shared_ptr<Connection> connection);
  void stop_accept() noexcept;
  // void connection_open(const std::shared_ptr<WebSocket> &connection) const;
  // void connection_close(const std::shared_ptr<WebSocket> &connection, int status, const std::string reason) const;
  // void connection_error(const std::shared_ptr<Connection> &connection, const error_code &ec) const;

};

#endif
