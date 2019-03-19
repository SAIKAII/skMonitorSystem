#ifndef MNTSYS_INCLUDE_RESOURCE_INFO_H_
#define MNTSYS_INCLUDE_RESOURCE_INFO_H_

#include <string>

struct OverallInfo{
  unsigned short users;
  unsigned short total, running, sleeping, stopped, zombie;
  unsigned int mem_total, mem_free, mem_used, buffer_cached;
  unsigned int swap_total, swap_free, swap_used, avail;
  double avg_one, avg_five, avg_fiftheen;
};

struct ProcessInfo{
  char state;
  unsigned short pid, ppid, userid;
  unsigned int pmem, vmem; // pmem = physical memory vmem = virtual memory
  double cpu, mem;
  std::string cmdline;
};

#endif
