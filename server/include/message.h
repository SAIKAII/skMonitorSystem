#ifndef skMNTSYS_INCLUDE_MESSAGE_H_
#define skMNTSYS_INCLUDE_MESSAGE_H_

#include <iostream>
#include <boost/asio.hpp>
#include <string>
#include <ios>

class WebSocketSSL;

class Message : public std::istream{
  friend class WebSocketSSL;
public:
  unsigned char fin_rsv_opcode_;

  Message() noexcept : std::istream(&streambuf_), length_(0){}
  Message(unsigned char fin_rsv_opcode, std::size_t length) noexcept : std::istream(&streambuf_), fin_rsv_opcode_(fin_rsv_opcode), length_(length){}
  std::size_t size() noexcept{
    return length_;
  }

  std::string string() noexcept{
    try{
      std::string str;
      auto size = streambuf_.size();
      str.resize(size);
      read(&str[0], static_cast<std::streamsize>(size));
      return str;
    }catch(...){
      return std::string();
    }
  }

private:
  std::size_t length_;
  boost::asio::streambuf streambuf_;
};

#endif
