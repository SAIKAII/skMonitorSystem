#ifndef skMNTSYS_INCLUDE_WEBSOCKETSSL_H_
#define skMNTSYS_INCLUDE_WEBSOCKETSSL_H_

#include "connection.h"
#include <boost/asio.hpp>
#include <memory>
#include <functional>
#include <iostream>

namespace asio = boost::asio;
using error_code = boost::system::error_code;
using WSS = asio::ssl::stream<asio::ip::tcp::socket>;

class WebSocketSSL : public std::enable_shared_from_this<WebSocketSSL>{
public:
  WebSocketSSL(std::shared_ptr<Connection> connection) : connection_(connection){}
  // void upgrade(const std::shared_ptr<Connection> &connection);
  std::shared_ptr<Connection> get_connection() noexcept { return connection_; }
  void read_message();
  void read_message_content(std::size_t length, unsigned char fin_rsv_opcode);
  void send(const std::shared_ptr<asio::streambuf> &send_stream, unsigned char fin_rsv_opcode = 129);
  void send_close(int status, const std::string &reason = "");

private:
  std::shared_ptr<Connection> connection_;

  void create_header(std::ostream &stream, const std::shared_ptr<asio::streambuf> &send_stream, unsigned char fin_rsv_opcode);
};

#endif
