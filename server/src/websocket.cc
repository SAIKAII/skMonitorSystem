#include "../include/websocket.h"
#include <regex>

std::shared_ptr<WebSocket> WebSocket::write_handshke(const std::shared_ptr<Connection> &connection){
  auto write_buffer = std::make_shared<asio::streambuf>();
  static std::regex express("^/print/?$");
  // 如果发现不是合法路径请求
  if(!std::regex_match(connection->path_.begin(), connection->path_.end(), express)){
    return;
  }

  if(WebSocket::generate_handshake(write_buffer, connection)){
    asio::async_write(*connection->socket_, *write_buffer, [this, connection, write_buffer](const error_code &ec, std::size_t /*bytes_transferred*/){
      if(!ec){
        return make_shared<WebSocket>(connection);
      }else{
        return nullptr;
      }
    });
  }
}

bool WebSocket::generate_handshake(const std::shared_ptr<asio::streambuf> &write_buffer, const std::shared_ptr<Connection> &connection){
  std::ostream handshake(write_buffer.get());

  auto header_it = connection->header_.find("Sec-WebSocket-Key");
  if(header_it == connection->header_.end())
    return false;

  static auto ws_magic_string = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
  auto sha1 = Crypto::sha1(header_it->second + ws_magic_string);

  handshake << "HTTP/1.1 101 Web Socket Protocl Handshake\r\n";
  handshake << "Upgrade: websocket\r\n";
  handshake << "Connection: Upgrade\r\n";
  handshake << "Sec-WebSocket-Accept: " << Crypto::Base64::encode(sha1) << "\r\n";
  handshake << "\r\n";

  return true;
}

void WebSocket::send(const std::shared_ptr<asio::streambuf> &send_stream, unsigned char fin_rsv_opcode){
  std::shared_ptr<asio::streambuf> header_stream = std::make_shared<asio::streambuf>();
  std::ostream stream(header_stream.get());
  create_header(stream, send_stream);

  auto self = this->shared_from_this();
  asio::async_write(*connection_->socket_, *header_stream, [self, send_stream](const error_code &ec, std::size_t /* bytes_transferred */){
    if(!ec){
      asio::async_write(*connection_->socket_, *send_stream, [self](const error_code &ec, std::size_t /* bytes_transferred */){
        // ...不处理
      });
    }
  });
}

void WebSocket::create_header(std::ostream &stream, const std::shared_ptr<asio::streambuf> &send_stream){
  std::size_t length = send_stream->size();

  stream.put(static_cast<char>(fin_rsv_opcode));
  if(length >= 126){
    std::size_t num_bytes;
    if(length > 0xffff){
      num_bytes = 8;
      stream.put(127);
    }else{
      num_bytes = 2;
      stream.put(126);
    }

    for(std::size_t c = num_bytes - 1; c != static_cast<std::size_t>(-1); --c)
      stream.put((static_cast<unsigned long long>(length) >> (8 * c)) % 256);
  }else
    stream.put(static_cast<char>(length));
}
