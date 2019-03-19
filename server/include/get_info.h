#ifndef MNTSYS_INCLUDE_GET_INFO_H_
#define MNTSYS_INCLUDE_GET_INFO_H_

#include "to_json.h"
#include "resource_info.h"
#include <dirent.h>
#include <vector>

const char *kDir = "/proc";
const char *kOverAllInfoOfUsers = "/proc/key-users";
const char *kOverAllInfoOfAvg = "/proc/loadavg";
const char *kOverAllInfoOfMem = "/proc/meminfo";
const char *kOverAllInfoOfUsage = "/proc/stat";
const char *kProcessInfoOfCmd = "/proc/%s/cmdline";
const char *kProcessInfoOfMem = "/proc/%s/status";
const char *kProcessInfoOfUsage = "/proc/%u/stat";
const unsigned short kMemLineEnd = 15;
const unsigned short kMemLineTotal = 0;
const unsigned short kMemLineFree = 1;
const unsigned short kMemLineAvail = 2;
const unsigned short kMemLineBuffer = 3;
const unsigned short kMemLineCached = 4;
const unsigned short kSwapLineUsed = 5;
const unsigned short kSwapLineTotal = 14;
const unsigned short kSwapLineFree = 15;
const unsigned short kMaxSlot = 250;
const unsigned short kProcessState = 2;
const unsigned short kProcessPid = 5;
const unsigned short kProcessPpid = 6;
const unsigned short kProcessUid = 8;
const unsigned short kProcessVmem = 17;
const unsigned short kProcessRSS = 21;
const unsigned short kProcessEnd = 21;
const unsigned short kProcessTimeStart = 13;
const unsigned short kProcessTimeEnd = 16;



class GetInfo{
public:
  GetInfo();
  void get_info(ToJSON &to_json);
  ~GetInfo();

private:
  void get_overallinfo(OverallInfo &overallinfo);
  void get_meminfo(OverallInfo &overallinfo);
  void get_processinfo();
  void get_usage();
  void read_host_cpu_jiffies(unsigned short &cpu);
  void read_procs_cpu_jiffies(bool first_time);

  DIR *p_dir_;
  std::vector<ProcessInfo> procs_info_;
  std::vector<unsigned int> procs_cpu_time_;
};

#endif
