#include "../include/web.h"

void Web::start(){
  bind();
  accept_and_run();
}

// 创建套接字
unsigned short Web::bind(){
  asio::ip::tcp::endpoint endpoint;
  if(config.address.size() > 0)
    endpoint = asio::ip::tcp::endpoint(asio::ip::address::from_string(config_.address), config_.port);
  else
    endpoint = asio::ip::tcp::endpoint(asio::ip::tcp::v4(), config_.port);

  io_service_ = std::make_shared<asio::io_service>();
  acceptor_ = std::unique_ptr<asio::ip::tcp::acceptor>(new asio::ip::tcp::acceptor(*io_service_));
  acceptor_->open(endpoint.protocol());
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
  std::shared_ptr<Connection> connection(new Connection(*io_service_));

  acceptor->async_accept(*connection->socket_, [this, connection](const error_code &ec){
    if(ec != asio::error::operation_aborted)
      accept();

    if(!ec){
      // 不使用nagle算法
      asio::ip::tcp::no_delay option(true);
      connection->socket_->set_option(option);

      read_and_parse(connection);
    }
  });
}

void Web::read_and_parse(const std::shared_ptr<Connection> &connection){
  connection->read_remote_endpoint();
  std::streambuf read_buffer;

  asio::async_read_until(*connection->socket_, read_buffer, "\r\n\r\n", [this, connection](const error_code &ec, std::size_t /*bytes_transferred 这个用不到*/){
    if(!ec){
      std::istream stream(&read_buffer);
      if(RequesMessage::parse(stream, connection->method_, connection->path_, connection->query_string_, connection->http_version_, connection->header_)){
        if(connection->header_.find("Sec-WebSocket-Key")){
          // std::shared_ptr<WebSocket> websocket;
          // // 拥有这个头部，说明是Web Socket的连接请求
          // if(websocket = WebSocket::write_handshke(connection), nullptr == websocket)
          //   connection_error(connection)
          // connection_open(websocket);
        }
        else{
          // 否则是普通的HTTP请求
          auto http = std::make_shared<HTTP>(connection);
          http->respond();
        }
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

// void Web::connection_open(const std::shared_ptr<WebSocket> &connection) const{
//   {
//     std::unique_lock<std::mutex> lock(endpoint.connections_mutex_);
//     endpoint_.connections_.insert(connection);
//   }
//
//   if(endpoint_.on_open)
//     endpoint_.on_open(connection);
// }
//
// void Web::void connection_close(const std::shared_ptr<WebSocket> &connection, int status, const std::string reason) const{
//   {
//     std::unique_lock<std::mutex> lock(endpoint.connections_mutex_);
//     endpoint_.connections_.erase(connection);
//   }
//
//   if(endpoint_.on_close)
//     endpoint_.on_close(connection, status, reason);
// }
//
// void Web::void connection_error(const std::shared_ptr<Connection> &connection, const error_code &ec) const{
//   {
//
//   }
//
//   if(endpoint_.on_error)
//     endpoint_.on_error(connection, ec);
// }
