#ifndef skMNTSYS_INCLUDE_WEB_H_
#define skMNTSYS_INCLUDE_WEB_H_

#include "config.h"
#include "utility.h"
#include "connection.h"
#include "http.h"
#include "websocket.h"
#include <boost/asio.hpp>
#include <regex>
#include <memory>

namespace asio = boost::asio;
using error_code = boost::system::error_code;

class WebSocket;

class Web{
public:
  Web(unsigned short port) noexcept : config_(port){}
  void start();
  void stop() noexcept;
  ~Web() noexcept {};

private:

  std::shared_ptr<asio::io_service> io_service_;
  std::unique_ptr<asio::ip::tcp::acceptor> acceptor_;
  Config config_;

  unsigned short bind();
  void accept_and_run();
  void accept();
  void read_and_parse(std::shared_ptr<Connection> connection);
  void stop_accept() noexcept;
};
#endif
