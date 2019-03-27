#ifndef skMNTSYS_INCLUDE_WEBSOCKET_H_
#define skMNTSYS_INCLUDE_WEBSOCKET_H_

#include "connection.h"
#include "endpoint.h"
#include <boost/asio.hpp>
#include <memory>
#include <functional>
#include <iostream>

namespace asio = boost::asio;
using error_code = boost::system::error_code;
using WS = asio::ip::tcp::socket;

class WebSocket : public std::enable_shared_from_this<WebSocket>{
public:
  static void write_handshake(std::shared_ptr<Connection> connection);
  static bool generate_handshake(std::shared_ptr<asio::streambuf> &write_buffer, const std::shared_ptr<Connection> connection);

  WebSocket(std::shared_ptr<Connection> connection) : connection_(connection){
    endpoint_ = Endpoint::get_instance();
  }
  // void upgrade(const std::shared_ptr<Connection> &connection);
  void read_message();
  void read_message_content(std::size_t length, unsigned char fin_rsv_opcode);
  void send(const std::shared_ptr<asio::streambuf> &send_stream, unsigned char fin_rsv_opcode = 129);
  void send_close(int status, const std::string &reason = "");

  void connection_open(std::shared_ptr<WebSocket> ws);
  void connection_close(std::shared_ptr<WebSocket> ws, int status, const std::string reason);
  void connection_error(std::shared_ptr<WebSocket> ws, const error_code &ec);

private:
  Endpoint *endpoint_;

  std::shared_ptr<Connection> connection_;

  void create_header(std::ostream &stream, const std::shared_ptr<asio::streambuf> &send_stream, unsigned char fin_rsv_opcode);
};

// Endpoint WebSocket::endpoint_;

#endif
