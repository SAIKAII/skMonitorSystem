#ifndef skMNTSYS_INCLUDE_HTTP_H_
#define skMNTSYS_INCLUDE_HTTP_H_

#include "connection.h"
#include "endpoint.h"
#include "handler.h"
#include <memory>
#include <functional>
#include <iostream>
#include <boost/asio.hpp>
// #include <map>
#include <unordered_map>
#include <string>
#include <fstream>

namespace asio = boost::asio;
using error_code = boost::system::error_code;


class HTTP : public std::enable_shared_from_this<HTTP>{
public:
  HTTP(std::shared_ptr<Connection> connection) : connection_(connection){}
  void respond();
  // void read_message(const std::shared_ptr<HTTP> &connection, Endpoint &endpoint) const;
  // void read_message_content(const std::shared_ptr<HTTP> &connection, std::size_t length, Endpoint &endpoint, unsigned char fin_rsv_opcode) const;

private:
  std::shared_ptr<Connection> connection_;
};


#endif
