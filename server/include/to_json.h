#ifndef MNTSYS_INCLUDE_TO_JSON_H_
#define MNTSYS_INCLUDE_TO_JSON_H_

#include "resource_info.h"
#include <list>
#include <mutex>
#include <memory>
#include <boost/asio.hpp>

const unsigned int kMaxProcess = 250;

namespace asio = boost::asio;

class ToJSON{
public:
  ToJSON();
  void set_overallinfo(OverallInfo &overallinfo);
  void set_processinfo(std::list<ProcessInfo> &processinfo);
  void data_to_json(std::shared_ptr<asio::streambuf> &write_buffer);
  void notify_send_data();

  enum SortMethod{NORMAL, CPU_FIRST, MEM_FIRST};
  static SortMethod sort_method_;
private:
  OverallInfo overallinfo_;
  std::list<ProcessInfo> procs_info_;
};

#endif
