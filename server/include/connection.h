#ifndef skMNTSYS_INCLUDE_CONNECTION_H_
#define skMNTSYS_INCLUDE_CONNECTION_H_

#include "send_data.h"
#include <memory>
#include <boost/asio.hpp>
#include <string>
#include <unordered_map>
#include <regex>
#include <list>
#include <mutex>
#include <atomic>
#include <regex>

class Connection : public std::enable_shared_from_this<Connection>{
  namespace asio = boost::asio;

public:
  Connection(std::unique_ptr<WS> &&socket/*隐式构造*/) noexcept : socket_(std::move(socket)), closed_(false){}
  std::string method_, path_, query_string_, http_version_;
  std::unordered_multimap<std::string, std::string> header_; // header有可能多个同样的
  asio::ip::tcp::endpoint remote_endpoint_;
  std::unique_ptr<asio::ip::tcp::socket> socket_;
  std::smatch path_match_;

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
  }

private:
  std::mutex socket_close_mutex_;
  std::atomic<bool> closed_;

};

#endif
