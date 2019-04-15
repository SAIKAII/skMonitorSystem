#ifndef SKMNTSYS_INCLUDE_MEM_FORMAT_H_
#define SKMNTSYS_INCLUDE_MEM_FORMAT_H_

#include "display_format.h"
#include "resource_info.h"
#include <list>
#include <boost/asio.hpp>
#include <iostream>
#include <memory>

class MemFormat : public DisplayFormat{
public:
  void data_to_json(std::shared_ptr<boost::asio::streambuf> &write_buffer, OverallInfo &overallinfo, std::list<ProcessInfo> &proc_list){
    std::ostream stream(write_buffer.get());
    int proc_size = proc_list.size();
    int count = 0;
    stream << "{\"processinfo\":[";
    for(auto proc : proc_list){
      stream << "{\"pid\":\"" << proc.pid << "\", \"mem\":" << proc.mem;
      stream << ", \"ppid\":" << proc.ppid << ", \"userid\":" << proc.userid;
      stream << ", \"pmem\":" << proc.pmem << ", \"vmem\":" << proc.vmem;
      stream << ", \"cmdline\":\"" << proc.cmdline << "\"}";
      ++count < proc_size ? stream << "," : stream << "]}";
    }
  }
};

#endif
