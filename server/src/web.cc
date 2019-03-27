#include "../include/web.h"
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
  std::cout << "run complete!" << std::endl;
}

void Web::accept(){
  std::shared_ptr<Connection> connection(new Connection(*io_service_));

  acceptor_->async_accept(*connection->socket_, [this, connection](const error_code &ec){
    if(ec != asio::error::operation_aborted){
      std::cout << "accept()" << std::endl;
      accept();
    }

    if(!ec){
      // 不使用nagle算法
      asio::ip::tcp::no_delay option(true);
      connection->socket_->set_option(option);

      read_and_parse(connection);
    }
  });
}

void Web::read_and_parse(std::shared_ptr<Connection> connection){
  connection->read_remote_endpoint();

  asio::async_read_until(*connection->socket_, connection->read_buffer_, "\r\n\r\n", [this, connection](const error_code &ec, std::size_t /*bytes_transferred 这个用不到*/){
    if(!ec){
      std::istream stream(&connection->read_buffer_);
      if(RequestMessage::parse(stream, connection->method_, connection->path_, connection->query_string_, connection->http_version_, connection->header_)){
        if(connection->header_.count("Sec-WebSocket-Key") > 0){
          std::cout << "Web Socket Request" << std::endl;
          // 拥有这个头部，说明是Web Socket的连接请求
          WebSocket::write_handshake(connection);
        }
        else{
          std::cout << "HTTP Request" << std::endl;
          // 否则是普通的HTTP请求
          auto http = std::make_shared<HTTP>(connection);
          http->respond();
        }
      }else{
        std::cout << "Parse failed!" << std::endl;
      }

    }
  });
}

void Web::stop_accept() noexcept{
  if(acceptor_){
    error_code ec;
    acceptor_->close(ec);
  }
}
