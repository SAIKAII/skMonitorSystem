#ifndef SKMNTSYS_INCLUDE_ENDPOINT_H_
#define SKMNTSYS_INCLUDE_ENDPOINT_H_

#include "to_json.h"
#include <functional>
#include <memory>
#include <mutex>
#include <unordered_set>
#include <boost/asio.hpp>
#include <string>

//#include <iostream>

using error_code = boost::system::error_code;

class WebSocketSSL;

class Endpoint{
public:
  static Endpoint *get_instance(){
    static Endpoint endpoint;
    return &endpoint;
  }

  std::unordered_set<std::shared_ptr<WebSocketSSL>> get_connections() noexcept{
    std::unique_lock<std::mutex> lock(connections_mutex_);
    auto copy = connections_;
    return copy;
  }

  void connection_open(std::shared_ptr<WebSocketSSL> wss){
    //std::cout << "connection_open()" << std::endl;
    std::unique_lock<std::mutex> lock(connections_mutex_);
    connections_.insert(wss);
  }

  void connection_close(std::shared_ptr<WebSocketSSL> wss, int status, const std::string &reason){
    std::unique_lock<std::mutex> lock(connections_mutex_);
    connections_.erase(wss);
    ToJSON::sort_method_ = ToJSON::NORMAL;
  }

  void connection_error(std::shared_ptr<WebSocketSSL> wss, const error_code &ec){
    std::unique_lock<std::mutex> lock(connections_mutex_);
    connections_.erase(wss);
    ToJSON::sort_method_ = ToJSON::NORMAL;
  }

private:
  Endpoint(){};
  std::unordered_set<std::shared_ptr<WebSocketSSL>> connections_;
  std::mutex connections_mutex_;
};

#endif
