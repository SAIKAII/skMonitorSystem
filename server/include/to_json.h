#ifndef MNTSYS_INCLUDE_TO_JSON_H_
#define MNTSYS_INCLUDE_TO_JSON_H_

#include "resource_info.h"
#include <vector>
#include <mutex>
#include <memory>
#include <boost/asio.hpp>

const unsigned short kMaxProcess = 250;

class ToJSON{
  namespace asio = boost::asio;

public:
  ToJSON();
  void set_overallinfo(OverallInfo &overallinfo);
  void set_processinfo(ProcessInfo &processinfo);
  void data_to_json(std::shared_ptr<asio::streambuf> &write_buffer);
  void notify_send_data();

private:
  OverallInfo overallinfo_;
  std::vector<ProcessInfo> procs_info_;
};

#endif
