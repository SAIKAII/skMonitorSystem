#ifndef skMNTSYS_INCLUDE_SENDDATA_H_
#define skMNTSYS_INCLUDE_SENDDATA_H_

#include "endpoint.h"
#include <semaphore.h>
#include <boost/asio.hpp>
#include <memory>
#include <mutex>
#include <thread>

class SendData : public enable_shared_from_this<SendData>{
  namespace asio = boost::asio;

public:
  SendData(std::shared_ptr<asio::streambuf> &write_buffer) : write_buffer_(write_buffer){}
  void send_to_client(const std::unordered_set<std::shared_ptr<WebSocket>> connections){
    
  }

private
  std::shared_ptr<asio::streambuf> write_buffer_;
};

#endif
