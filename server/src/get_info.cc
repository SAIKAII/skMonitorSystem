#include "../include/get_info.h"
#include <sys/types.h>
#include <stdio.h>
#include <dirent.h>
#include <fstream>
#include <string>
#include <sstream>
#include <string.h>
#include <unistd.h>

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

GetInfo::GetInfo() : procs_info_(kMaxSlot), procs_cpu_time_(kMaxSlot){
  if(!(p_dir_ = opendir(kDir))){
    perror("opendir:");
    exit(1);
  }
}

void GetInfo::get_info(ToJSON &to_json){
  OverallInfo overallinfo;
  get_overallinfo(overallinfo);
  to_json.set_overallinfo(overallinfo);

  get_processinfo(overallinfo);
  to_json.set_processinfo(procs_info_);
  to_json.notify_send_data();
  procs_info_.clear(); // 已经把需要的内容交换出去了，这里剩下的只是上一次（已经作废）的数据了
}

void GetInfo::get_overallinfo(OverallInfo &overallinfo){
  // 读取用户数
  std::ifstream fin;
  fin.open(kOverAllInfoOfUsers);
  {
    std::string tmp;
    overallinfo.users = 0;
    while(std::getline(fin, tmp)){
      ++overallinfo.users;
    }
  }
  fin.close();

  // 读取平均负载值
  fin.open(kOverAllInfoOfAvg);
  fin >> overallinfo.avg_one;
  fin >> overallinfo.avg_five;
  fin >> overallinfo.avg_fiftheen;
  fin.close();

  // 读取物理内存与交换区负载信息
  get_meminfo(overallinfo);
}

void GetInfo::get_meminfo(OverallInfo &overallinfo){
  std::ifstream fin;
  fin.open(kOverAllInfoOfMem);
  std::string tmp;
  std::string invalid;
  int line = 0;
  while(line <= kMemLineEnd){
    std::getline(fin, tmp);
    std::stringstream sstr(tmp);
    sstr >> invalid;
    switch (line) {
      case kMemLineTotal:
        sstr >> overallinfo.mem_total;
        break;
      case kMemLineFree:
        sstr >> overallinfo.mem_free;
        break;
      case kMemLineAvail:
        sstr >> overallinfo.avail;
        break;
      case kMemLineBuffer:
        sstr >> overallinfo.buffer_cached;
        break;
      case kMemLineCached:
        int cached;
        sstr >> cached;
        overallinfo.buffer_cached += cached;
        overallinfo.mem_used = overallinfo.mem_total - overallinfo.mem_free - overallinfo.buffer_cached;
        break;
      case kSwapLineUsed:
        sstr >> overallinfo.swap_used;
        break;
      case kSwapLineTotal:
        sstr >> overallinfo.swap_total;
        break;
      case kSwapLineFree:
        sstr >> overallinfo.swap_free;
        break;
    }
    ++line;
  }
  fin.close();
}

void GetInfo::get_processinfo(OverallInfo &overallinfo){
  overallinfo.total = overallinfo.running = overallinfo.sleeping = 0;
  overallinfo.stopped = overallinfo.zombie = 0;
  dirent *ptr;
  std::ifstream fin;
  while(ptr = readdir(p_dir_)){
    ProcessInfo processinfo;
    if(ptr->d_name[0] < '0' && ptr->d_name[0] > '9')
      continue;
    ++overallinfo.total;
    char pathname[20];
    // 读取命令
    memset(pathname, 0, 20);
    snprintf(pathname, 20, kProcessInfoOfCmd, ptr->d_name);
    fin.open(pathname);
    std::getline(fin, processinfo.cmdline);
    fin.close();

    // 读取进程各信息
    memset(pathname, 0, 20);
    snprintf(pathname, 20, kProcessInfoOfMem, ptr->d_name);
    fin.open(pathname);
    int line = 0;
    std::string tmp, invalid;
    while(line <= kProcessEnd){
      std::getline(fin, tmp);
      std::stringstream sstr(tmp);
      sstr >> invalid;
      switch(line){
        case kProcessState:
          sstr >> processinfo.state;
          processinfo.state == 'R' ? ++overallinfo.running : processinfo.state == 'S' ? ++overallinfo.sleeping : processinfo.state == 'T' ? ++overallinfo.stopped : processinfo.state == 'Z' ? ++overallinfo.zombie : NULL;
          break;
        case kProcessPid:
          sstr >> processinfo.pid;
          break;
        case kProcessPpid:
          sstr >> processinfo.ppid;
          break;
        case kProcessUid:
          sstr >> processinfo.userid;
          break;
        case kProcessVmem:
          sstr >> processinfo.vmem;
          break;
        case kProcessRSS:
          sstr >> processinfo.pmem;
          processinfo.mem = processinfo.pmem / overallinfo.mem_total;
          break;
      }
      ++line;
    }
    fin.close();
    procs_info_.push_back(processinfo);
  } // 到这里就结束了各个进程的信息获取，除了CPU和内存占用率还没得到
  get_usage();
  rewinddir(p_dir_);
}

void GetInfo::get_usage(){
  unsigned int cpu1 = 0;
  read_host_cpu_jiffies(cpu1);
  read_procs_cpu_jiffies(true);

  // 前面到这里，就采样了被“减时间”，后面休息一秒后采样“减时间”
  sleep(1);

  unsigned int cpu2 = 0;
  read_host_cpu_jiffies(cpu2);
  read_procs_cpu_jiffies(false);
  unsigned int cpu_time = cpu2 - cpu1;
  int index = 0;
  for(auto item : procs_cpu_time_){
    procs_info_[index++].cpu = item / cpu_time;
  }
  procs_cpu_time_.clear(); // 既然已经获取所有进程的CPU占用率，就把容器清空，留做下次使用
}

void GetInfo::read_host_cpu_jiffies(unsigned int &cpu){
  std::ifstream fin;
  fin.open(kOverAllInfoOfUsage);
  std::string invalid, tmp;
  std::getline(fin, tmp);
  std::stringstream sstr(tmp);
  sstr >> invalid;
  unsigned int tm = 0;
  while(sstr >> tm, !sstr.eof()){
    cpu += tm;
  }
  fin.close();
}

void GetInfo::read_procs_cpu_jiffies(bool first_time){
  char pathname[17];
  int line, index = 0;
  std::ifstream fin;
  std::string invalid;
  for(auto item : procs_info_){
    memset(pathname, 0, 17);
    snprintf(pathname, 17, kProcessInfoOfUsage, item.pid);
    fin.open(pathname);
    line = 0;
    unsigned int tm = 0, sum = 0;
    while(line++ < kProcessTimeStart)
      fin >> invalid;
    while(line++ <= kProcessTimeEnd){
      fin >> tm;
      sum += tm;
    }
    fin.close();
    if(first_time)
      procs_cpu_time_.push_back(sum);
    else
      procs_cpu_time_[index] = sum - procs_cpu_time_[index++];
  }
}

GetInfo::~GetInfo(){
  if(NULL != p_dir_){
    closedir(p_dir_);
  }
}
