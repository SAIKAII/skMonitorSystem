#ifndef skMNTSYS_INCLUDE_HANDLER_H_
#define skMNTSYS_INCLUDE_HANDLER_H_

#include "utility.h"
#include "connection.h"
#include <memory>
#include <string>
#include <fstream>
#include <vector>

using resource_type = std::map<std::string, std::unordered_map<std::string, std::function<void(std::ostream&, std::shared_ptr<Connection>)>>>;

class Handler{
public:
  static Handler *get_instance(){
    static Handler handler;
    return &handler;
  }
  void handler_init();

  resource_type resource_;
  resource_type default_resource_;
  std::vector<resource_type::iterator> all_resource_;

private:
  Handler(){}
};

#endif
