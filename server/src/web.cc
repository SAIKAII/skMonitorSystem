#include "../include/web.h"
#include "../include/crypto.h"
#include "../include/handler.h"
#include "../include/authentication.h"
#include <utility>

void Web::start(){
  bind();
  accept_and_run();
}

// 创建套接字
unsigned short Web::bind(){
  asio::ip::tcp::endpoint endpoint;
  if(config_.address_.size() > 0)
    endpoint = asio::ip::tcp::endpoint(asio::ip::address::from_string(config_.address_), config_.port_);
  else
    endpoint = asio::ip::tcp::endpoint(asio::ip::tcp::v4(), config_.port_);

  io_service_ = std::make_shared<asio::io_service>();
  acceptor_ = std::unique_ptr<asio::ip::tcp::acceptor>(new asio::ip::tcp::acceptor(*io_service_));
  acceptor_->open(endpoint.protocol());
  acceptor_->set_option(asio::socket_base::reuse_address(true));
  acceptor_->bind(endpoint);

  return acceptor_->local_endpoint().port();
}

void Web::accept_and_run(){
  acceptor_->listen();
  accept();

  if(io_service_->stopped())
    io_service_->reset();

  io_service_->run();
}

void Web::accept(){
  std::shared_ptr<Connection> connection(new Connection(*io_service_, context_));

  acceptor_->async_accept(connection->socket_->lowest_layer(), [this, connection](const error_code &ec){
    if(ec != asio::error::operation_aborted)
      accept();

    if(!ec){
      // 不使用nagle算法
      // asio::ip::tcp::no_delay option(true);
      // connection->socket_->set_option(option);

      connection->socket_->async_handshake(asio::ssl::stream_base::server, [this, connection](const error_code &ec){
        if(!ec)
          read_and_parse(connection);
      });
    }
  });
}

void Web::read_and_parse(std::shared_ptr<Connection> connection){
  connection->read_remote_endpoint();

  asio::async_read_until(*connection->socket_, connection->read_buffer_, "\r\n\r\n", [this, connection](const error_code &ec, std::size_t bytes_transferred){
    if(!ec){
      std::istream stream(&connection->read_buffer_);
      if(RequestMessage::parse(stream, connection->method_, connection->path_, connection->query_string_, connection->http_version_, connection->header_, connection->token_)){
        if(connection->header_.count("Sec-WebSocket-Key") > 0){
          // 拥有这个头部，说明是Web Socket的连接请求
          write_handshake(connection);
        }
        else{
          http_resolve(connection, bytes_transferred);
        }
      }
    }else
      connection->close();
  });
}

void Web::http_resolve(std::shared_ptr<Connection> connection, std::size_t bytes_transferred){
  std::cout << "HTTPS Request" << std::endl;
  if(connection->header_.count("Content-Length") > 0){
    std::size_t total = connection->read_buffer_.size();

    asio::async_read(*connection->socket_, connection->read_buffer_,
      asio::transfer_exactly(std::stoull(connection->header_.find("Content-Length")->second) - total), [this, connection](const error_code &ec, std::size_t /* bytes_transferred */){
      // 普通的HTTP请求
      if(!ec)
        respond(connection);
    });
  }else{
    respond(connection);
  }
}

void Web::write_handshake(std::shared_ptr<Connection> connection){
  auto write_buffer = std::make_shared<asio::streambuf>();
  static std::regex express("^/print/?$");
  // 如果发现不是合法路径请求
  if(!std::regex_match(connection->path_.begin(), connection->path_.end(), express)){
    return;
  }

  if(generate_handshake(write_buffer, connection)){
    asio::async_write(*connection->socket_, *write_buffer, [this, connection](const error_code &ec, std::size_t /*bytes_transferred*/){
      if(!ec){
        std::shared_ptr<WebSocketSSL> wss = std::make_shared<WebSocketSSL>(connection);
        Endpoint *endpoint = Endpoint::get_instance();
        endpoint->connection_open(wss);
        wss->read_message();
      }else{
        return;
      }
    });
  }
}

bool Web::generate_handshake(std::shared_ptr<asio::streambuf> &write_buffer, std::shared_ptr<Connection> connection){
  std::ostream handshake(write_buffer.get());

  auto header_it = connection->header_.find("Sec-WebSocket-Key");
  if(header_it == connection->header_.end())
    return false;

  static auto wss_magic_string = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
  auto sha1 = Crypto::sha1(header_it->second + wss_magic_string);

  handshake << "HTTP/1.1 101 Web Socket Protocl Handshake\r\n";
  handshake << "Upgrade: websocket\r\n";
  handshake << "Connection: Upgrade\r\n";
  handshake << "Sec-WebSocket-Accept: " << Crypto::base64_encode(sha1) << "\r\n";
  handshake << "\r\n";

  return true;
}

void Web::respond(std::shared_ptr<Connection> connection){
  std::cout << "Method: " << connection->method_ << std::endl;
  Handler *handler = Handler::get_instance();
  for(auto res_it : handler->all_resource_){
    std::regex express(res_it->first);
    std::smatch sm_res;
    if(std::regex_match(connection->path_, sm_res, express)){
      if(res_it->second.count(connection->method_) > 0){
        connection->path_match_ = std::move(sm_res);

        auto write_buffer = std::make_shared<asio::streambuf>();
        std::ostream response(write_buffer.get());
        res_it->second[connection->method_](response, connection);
        std::cout << "HTTPS write" << std::endl;

        asio::async_write(*connection->socket_, *write_buffer, [this, connection](const error_code &ec, std::size_t /* bytes_transferred */){
          if(!ec){
              read_and_parse(connection);
          }
        });
        break;
      }
    }
  }
}

void Web::stop_accept() noexcept{
  if(acceptor_){
    error_code ec;
    acceptor_->close(ec);
  }
}
