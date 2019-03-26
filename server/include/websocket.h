#ifndef skMNTSYS_INCLUDE_WEBSOCKET_H_
#define skMNTSYS_INCLUDE_WEBSOCKET_H_

#include "connection.h"
#include "send_data.h"
#include "endpoint.h"
#include <memory>
#include <boost/asio.hpp>
#include <memory>
#include <functional>
#include <iostream>

namespace asio = boost::asio;
using error_code = boost::system::error_code;
using WS = asio::ip::tcp::socket;

class WebSocket : public std::enable_shared_from_this<WebSocket>{
public:
  static void write_handshke(const std::shared_ptr<Connection> connection);
  static bool generate_handshake(const std::shared_ptr<asio::streambuf> &write_buffer, const std::shared_ptr<Connection> connection);

  WebSocket(std::shared_ptr<Connection> connection) : connection_(connection){}
  // void upgrade(const std::shared_ptr<Connection> &connection);
  // void read_handshake(const std::shared_ptr<WebSocket> &connection);
  // void read_message(const std::shared_ptr<WebSocket> &connection, Endpoint &endpoint) const;
  // void read_message_content(const std::shared_ptr<WebSocket> &connection, std::size_t length, Endpoint &endpoint, unsigned char fin_rsv_opcode) const;
  void send(const std::shared_ptr<asio::streambuf> &send_stream, unsigned char fin_rsv_opcode = 129);
  // void send_close(int status, const std::string &reason = "", const std::function<void(const error_code &)> &callback = nullptr);



private:
  std::shared_ptr<Connection> connection_;

  void create_header(std::ostream &stream, const std::shared_ptr<asio::streambuf> &send_stream);
};

#endif
