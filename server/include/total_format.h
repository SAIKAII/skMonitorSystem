#ifndef SKMNTSYS_INCLUDE_TOTAL_FORMAT_H_
#define SKMNTSYS_INCLUDE_TOTAL_FORMAT_H_

#include "display_format.h"
#include "resource_info.h"
#include <boost/asio.hpp>
#include <list>
#include <iostream>
#include <memory>

class TotalFormat : public DisplayFormat{
public:
  void data_to_json(std::shared_ptr<boost::asio::streambuf> &write_buffer, OverallInfo &overallinfo, std::list<ProcessInfo> &proc_list){
    std::ostream stream(write_buffer.get());
    stream << "{\"users\":" << overallinfo.users << ", \"total\":" << overallinfo.total;
    stream << ", \"running\":" << overallinfo.running << ", \"sleeping\":" << overallinfo.sleeping;
    stream << ", \"stopped\":" << overallinfo.stopped << ", \"zombie\":" << overallinfo.zombie;
    stream << ", \"mem_total\":" << overallinfo.mem_total << ", \"mem_free\":" << overallinfo.mem_free;
    stream << ", \"mem_used\":" << overallinfo.mem_used << ", \"buffer_cached\":" << overallinfo.buffer_cached;
    stream << ", \"swap_total\":" << overallinfo.swap_total << ", \"swap_free\":" << overallinfo.swap_free;
    stream << ", \"swap_used\":" << overallinfo.swap_used << ", \"avail\":" << overallinfo.avail;
    stream << ", \"avg_one\":" << overallinfo.avg_one << ", \"avg_five\":" << overallinfo.avg_five;
    stream << ", \"avg_fiftheen\":" << overallinfo.avg_fiftheen << "}";
  }
};

#endif
