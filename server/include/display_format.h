#ifndef SKMNTSYS_INCLUDE_DISPLAY_FORMAT_H_
#define SKMNTSYS_INCLUDE_DISPLAY_FORMAT_H_

#include "resource_info.h"
#include <boost/asio.hpp>
#include <list>
#include <memory>

class DisplayFormat{
public:
  virtual void data_to_json(std::shared_ptr<boost::asio::streambuf> &write_buffer, OverallInfo &overallinfo, std::list<ProcessInfo> &proc_list) = 0;
};

#endif
