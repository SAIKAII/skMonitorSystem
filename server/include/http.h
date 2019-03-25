#ifndef skMNTSYS_INCLUDE_HTTP_H_
#define skMNTSYS_INCLUDE_HTTP_H_

#include "connection.h"
#include "endpoint.h"
#include <memory>
#include <functional>
#include <iostream>
#include <boost/asio.hpp>

class HTTP : public std::enable_shared_from_this<HTTP>{
  namespace asio = boost::asio;
  using error_code = boost::system::error_code;

public:
  HTTP(const std::shared_ptr<Connection> &connection) : connection_(connection){}
  void respond(std::shared_ptr<Connection> &connection);
  // void read_message(const std::shared_ptr<HTTP> &connection, Endpoint &endpoint) const;
  // void read_message_content(const std::shared_ptr<HTTP> &connection, std::size_t length, Endpoint &endpoint, unsigned char fin_rsv_opcode) const;

  static std::map<std::string, std::unordered_map<std::string, std::function<void(std::ostream&, std::shared_ptr<Connection>&)>>> resource_;
  
private:
  const std::shared_ptr<Connection> connection_;
};

#endif
