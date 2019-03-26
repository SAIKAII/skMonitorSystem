#ifndef skMNTSYS_INCLUDE_CONFIG_H_
#define skMNTSYS_INCLUDE_CONFIG_H_

#include <string>

class Config{
public:
  Config(unsigned short port) noexcept : port_(port){}

  unsigned short port_;
  std::string address_;
};

#endif
