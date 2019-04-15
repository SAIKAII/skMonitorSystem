#ifndef SKMNTSYS_INCLUDE_USAGE_FORMAT_H_
#define SKMNTSYS_INCLUDE_USAGE_FORMAT_H_

#include "display_format.h"
#include "resource_info.h"
#include <boost/asio.hpp>
#include <list>
#include <iostream>
#include <memory>

class UsageFormat : public DisplayFormat{
  void data_to_json(std::shared_ptr<boost::asio::streambuf> &write_buffer, OverallInfo &overallinfo, std::list<ProcessInfo> &proc_list){
    std::ostream stream(write_buffer.get());
    int proc_size = proc_list.size();
    int count = 0;
    stream << "{\"processinfo\":[";
    for(auto proc : proc_list){
      stream << "{\"state\":\"" << proc.state << "\", \"pid\":" << proc.pid;
      stream << ", \"ppid\":" << proc.ppid << ", \"userid\":" << proc.userid;
      stream << ", \"cpu\":" << proc.cpu << ", \"mem\":" << proc.mem;
      stream << ", \"cmdline\":\"" << proc.cmdline << "\"}";
      ++count < proc_size ? stream << "," : stream << "]}";
    }
  }
};

#endif
