#ifndef SKMNTSYS_INCLUDE_CONNECTION_H_
#define SKMNTSYS_INCLUDE_CONNECTION_H_

#include "utility.h"
#include "message.h"
#include "display_format.h"
#include "authentication.h"
#include <memory>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <string>
#include <unordered_map>
#include <regex>
#include <list>
#include <mutex>
#include <atomic>
#include <utility>

#include <iostream>

namespace asio = boost::asio;
using error_code = boost::system::error_code;
using SocketSSL = boost::asio::ssl::stream<boost::asio::ip::tcp::socket>;

class WebSocketSSL;
class Web;

class Connection : public std::enable_shared_from_this<Connection>{
  friend class WebSocketSSL;
  friend class Web;
public:
  Connection(asio::io_service &io_service, asio::ssl::context &ctx) noexcept : socket_(new SocketSSL(io_service, ctx)), closed_(false){}
  std::string method_, path_, query_string_, http_version_;
  CaseInsensitiveMultimap header_; // header有可能多个同样的
  asio::ip::tcp::endpoint remote_endpoint_;
  std::unique_ptr<SocketSSL> socket_;
  std::smatch path_match_;
  asio::streambuf read_buffer_;
  std::shared_ptr<Message> fragmented_message_;
  std::string token_;
  std::shared_ptr<DisplayFormat> display_format_;

  std::string remote_endpoint_address() noexcept{
    try{
      return remote_endpoint_.address().to_string();
    }catch(...){
      return std::string();
    }
  }
  void read_remote_endpoint() noexcept {
    try{
      remote_endpoint_ = socket_->lowest_layer().remote_endpoint();
    }catch(...){}
  }
  unsigned short remote_endpoint_port() noexcept{ return remote_endpoint_.port(); }
  void close() noexcept{
    error_code ec;
    std::unique_lock<std::mutex> lock(socket_close_mutex_);
    socket_->lowest_layer().shutdown(asio::ip::tcp::socket::shutdown_both, ec);
    socket_->lowest_layer().close(ec);
    Authentication::delete_token(token_);
    //std::cout << "关闭TCP" << std::endl;
  }

private:
  std::mutex socket_close_mutex_;
  std::atomic<bool> closed_;
};

#endif
