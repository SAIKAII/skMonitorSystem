#ifndef MNTSYS_INCLUDE_GET_INFO_H_
#define MNTSYS_INCLUDE_GET_INFO_H_

#include "to_json.h"
#include "resource_info.h"
#include <dirent.h>
#include <vector>

class GetInfo{
public:
  GetInfo();
  void get_info(ToJSON &to_json);
  ~GetInfo();

private:
  void get_overallinfo(OverallInfo &overallinfo);
  void get_meminfo(OverallInfo &overallinfo);
  void get_processinfo(OverallInfo &overallinfo);
  void get_usage();
  void read_host_cpu_jiffies(unsigned int &cpu);
  void read_procs_cpu_jiffies(bool first_time);

  DIR *p_dir_;
  std::vector<ProcessInfo> procs_info_;
  std::vector<unsigned int> procs_cpu_time_;
};

#endif
