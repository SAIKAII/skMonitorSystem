#ifndef skMNTSYS_INCLUDE_ENDPOINT_H_
#define skMNTSYS_INCLUDE_ENDPOINT_H_

// #include "websocket.h"
#include <functional>
#include <memory>
#include <mutex>
#include <unordered_set>
#include <boost/asio.hpp>

using error_code = boost::system::error_code;

class WebSocket;

class Endpoint{
  friend class WebSocket;
public:
  std::function<void(std::shared_ptr<WebSocket>)> on_open;
  std::function<void(std::shared_ptr<WebSocket>)> on_message;
  std::function<void(std::shared_ptr<WebSocket>, int, const std::string &)> on_close;
  std::function<void(std::shared_ptr<WebSocket>, const error_code &)> on_error;
  // std::function<void(std::shared_ptr<WebSocket>)> on_ping;
  // std::function<void(std::shared_ptr<WebSocket>)> on_pong;

  static Endpoint *get_instance(){
    static Endpoint endpoint;
    return &endpoint;
  }

  std::unordered_set<std::shared_ptr<WebSocket>> get_connections() noexcept{
    std::unique_lock<std::mutex> lock(connections_mutex_);
    auto copy = connections_;
    return copy;
  }

private:
  std::unordered_set<std::shared_ptr<WebSocket>> connections_;
  std::mutex connections_mutex_;
};

#endif
