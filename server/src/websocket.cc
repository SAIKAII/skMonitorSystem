#include "../include/websocket.h"
#include "../include/message.h"
#include "../include/crypto.h"
#include <regex>
#include <array>
#include <mutex>

void WebSocket::write_handshake(std::shared_ptr<Connection> connection){
  std::cout << "write_handshake()" << std::endl;
  auto write_buffer = std::make_shared<asio::streambuf>();
  static std::regex express("^/print/?$");
  // 如果发现不是合法路径请求
  if(!std::regex_match(connection->path_.begin(), connection->path_.end(), express)){
    return;
  }

  if(WebSocket::generate_handshake(write_buffer, connection)){
    asio::async_write(*connection->socket_, *write_buffer, [connection](const error_code &ec, std::size_t /*bytes_transferred*/){
      if(!ec){
        std::shared_ptr<WebSocket> ws = std::make_shared<WebSocket>(connection);
        ws->connection_open(ws);
        ws->read_message();
      }else{
        return;
      }
    });
  }
}

bool WebSocket::generate_handshake(std::shared_ptr<asio::streambuf> &write_buffer, std::shared_ptr<Connection> connection){
  std::cout << "generate_handshake()" << std::endl;
  std::ostream handshake(write_buffer.get());

  auto header_it = connection->header_.find("Sec-WebSocket-Key");
  if(header_it == connection->header_.end())
    return false;

  static auto ws_magic_string = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
  auto sha1 = Crypto::sha1(header_it->second + ws_magic_string);

  handshake << "HTTP/1.1 101 Web Socket Protocl Handshake\r\n";
  handshake << "Upgrade: websocket\r\n";
  handshake << "Connection: Upgrade\r\n";
  handshake << "Sec-WebSocket-Accept: " << Crypto::base64_encode(sha1) << "\r\n";
  handshake << "\r\n";

  return true;
}

void WebSocket::read_message(){
  std::cout << "read_message()" << std::endl;
  asio::async_read(*connection_->socket_, connection_->read_buffer_, asio::transfer_exactly(2), [this](const error_code &ec, std::size_t bytes_transferred){
    if(!ec){
      if(bytes_transferred == 0){
        read_message();
        return;
      }
      std::istream stream(&connection_->read_buffer_);

      std::array<unsigned char, 2> first_bytes;
      stream.read((char*)&first_bytes[0], 2);

      unsigned char fin_rsv_opcode = first_bytes[0];

      // 根据协议规定，客户端使用Web Socket协议发送报文的话，必须mask。否则断开连接
      if(first_bytes[1] < 128){
        const std::string reason("message from client not masked");
        send_close(1002, reason);
        connection_close(this->shared_from_this(), 1002, reason);
        return;
      }

      std::size_t length = (first_bytes[1] & 127); // 01111111b

      std::cout << "length: " << length << std::endl;

      if(length == 126){
        // 接下来2字节是内容长度指示（超过125字节，标记为126的情况）
        asio::async_read(*connection_->socket_, connection_->read_buffer_, asio::transfer_exactly(2), [this, fin_rsv_opcode](const error_code &ec, std::size_t /* bytes_transferred */){
          if(!ec){
            std::istream stream(&connection_->read_buffer_);
            std::array<unsigned char, 2> length_bytes;
            stream.read((char*)&length_bytes[0], 2);

            std::size_t length = 0;
            std::size_t num_bytes = 2;
            for(std::size_t c = 0; c < num_bytes; ++c)
              length += static_cast<std::size_t>(length_bytes[c]) << (8 * (num_bytes - 1 - c));

            read_message_content(length, fin_rsv_opcode);
          }else{
            connection_error(this->shared_from_this(), ec);
          }
        });
      }else if(length == 127){
        // 接下来8字节是内容长度指示（超过125字节，标记为127的情况）
        asio::async_read(*connection_->socket_, connection_->read_buffer_, asio::transfer_exactly(8), [this, fin_rsv_opcode](const error_code &ec, std::size_t /* bytes_transferred */){
          if(!ec){
            std::istream stream(&connection_->read_buffer_);

            std::array<unsigned char, 8> length_bytes;
            stream.read((char*)&length_bytes[0], 8);

            std::size_t length = 0;
            std::size_t num_bytes = 8;
            for(std::size_t c = 0; c < num_bytes; ++c)
              length += static_cast<std::size_t>(length_bytes[c]) << (8 * (num_bytes - 1 - c));

            read_message_content(length, fin_rsv_opcode);
          }else
            connection_error(this->shared_from_this(), ec);
        });
      }else
        read_message_content(length, fin_rsv_opcode);
    }else
      connection_error(this->shared_from_this(), ec);
  });
}

void WebSocket::read_message_content(std::size_t length, unsigned char fin_rsv_opcode){
  std::cout << "read_message_content()" << std::endl;
  // 这里的4是代表mask大小
  asio::async_read(*connection_->socket_, connection_->read_buffer_, asio::transfer_exactly(4 + length), [this, length, fin_rsv_opcode](const error_code &ec, std::size_t /* bytes_transferred */){
    if(!ec){
      std::istream istream(&connection_->read_buffer_);

      // 4字节的mask
      std::array<unsigned char, 4> mask;
      istream.read((char*)&mask[0], 4);

      std::shared_ptr<Message> message;

      // 如果分片
      if((fin_rsv_opcode & 0x80) == 0 || (fin_rsv_opcode & 0x0f) == 0){ // 0x80->1000 0000b  0x0f->0000 1111b
        // 第一个分片
        if(!connection_->fragmented_message_){
          connection_->fragmented_message_ = std::shared_ptr<Message>(new Message(fin_rsv_opcode, length));
          connection_->fragmented_message_->fin_rsv_opcode_ |= 0x80;
        }else
          connection_->fragmented_message_->length_ += length;
        message = connection_->fragmented_message_;
      }else
        message = std::shared_ptr<Message>(new Message(fin_rsv_opcode, length));
      std::ostream ostream(&message->streambuf_);
      for(std::size_t c = 0; c < length; ++c) // umask
        ostream.put(istream.get() ^ mask[c % 4]);

      // 如果是连接关闭
      if((fin_rsv_opcode & 0x0f) == 8){
        int status = 0;
        if(length >= 2){
          // 获取关闭连接的状态
          unsigned char byte1 = message->get();
          unsigned char byte2 = message->get();
          status = (static_cast<int>(byte1) << 8) + byte2;
        }

        auto reason = message->string();
        send_close(status, reason);
        connection_close(this->shared_from_this(), status, reason);
      }else if((fin_rsv_opcode & 0x80) == 0){ // 如果不是最后的分片
        this->read_message();
      }else{ // 其他情况一律按最后一个分片处理，非关闭连接
        if(endpoint_->on_message)
          endpoint_->on_message(this->shared_from_this());

        connection_->fragmented_message_ = nullptr;

        this->read_message();
      }
    }
  });
}

void WebSocket::send_close(int status, const std::string &reason){
  std::cout << "send_close()" << std::endl;
  if(connection_->closed_)
    return;
  connection_->closed_ = true;

  std::shared_ptr<asio::streambuf> send_stream = std::make_shared<asio::streambuf>();
  std::ostream ostream(send_stream.get());

  ostream.put(status >> 8);
  ostream.put(status % 258);

  ostream << reason;

  send(send_stream, 136);
}

// 向发送指定数据
void WebSocket::send(const std::shared_ptr<asio::streambuf> &send_stream, unsigned char fin_rsv_opcode){
  std::cout << "send()" << std::endl;
  if(connection_->closed_)
    return;
  std::shared_ptr<asio::streambuf> header_stream = std::make_shared<asio::streambuf>();
  std::ostream stream(header_stream.get());
  create_header(stream, send_stream, fin_rsv_opcode);

  auto self = this->shared_from_this(); // 这个是必须的，否则在发现关闭连接的时候，数据还没发完，对象就被销毁了

  asio::async_write(*connection_->socket_, *header_stream, [this, self, send_stream](const error_code &ec, std::size_t /* bytes_transferred */){
    if(!ec){
      asio::async_write(*connection_->socket_, *send_stream, [this](const error_code &ec, std::size_t /* bytes_transferred */){
        // ...不处理
      });
    }
  });
}

void WebSocket::create_header(std::ostream &stream, const std::shared_ptr<asio::streambuf> &send_stream, unsigned char fin_rsv_opcode){
  std::cout << "create_header" << std::endl;
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

void WebSocket::connection_open(std::shared_ptr<WebSocket> ws){
  std::cout << "connection_open()" << std::endl;
  {
    std::unique_lock<std::mutex> lock(endpoint_->connections_mutex_);
    endpoint_->connections_.insert(ws);
  }

  if(endpoint_->on_open)
    endpoint_->on_open(ws);
}

void WebSocket::connection_close(std::shared_ptr<WebSocket> ws, int status, const std::string reason){
  std::cout << "connection_close()" << std::endl;
  {
    std::unique_lock<std::mutex> lock(endpoint_->connections_mutex_);
    endpoint_->connections_.erase(ws);
  }

  if(endpoint_->on_close)
    endpoint_->on_close(ws, status, reason);
}

void WebSocket::connection_error(std::shared_ptr<WebSocket> ws, const error_code &ec){
  std::cout << "connection_error" << std::endl;
  {
    std::unique_lock<std::mutex> lock(endpoint_->connections_mutex_);
    endpoint_->connections_.erase(ws);
  }

  if(endpoint_->on_error)
    endpoint_->on_error(ws, ec);
}
