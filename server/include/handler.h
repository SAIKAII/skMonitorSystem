#ifndef skMNTSYS_INCLUDE_HANDLER_H_
#define skMNTSYS_INCLUDE_HANDLER_H_

#include "utility.h"
#include "connection.h"
#include <memory>
#include <string>
#include <fstream>

using resource_type = std::map<std::string, std::unordered_map<std::string, std::function<void(std::ostream&, std::shared_ptr<Connection>, bool&)>>>;

class Handler{
public:
  static Handler *get_instance(){
    static Handler handler;
    return &handler;
  }
  void handler_init();

  resource_type resource_;

private:
  Handler(){}
};

#endif
