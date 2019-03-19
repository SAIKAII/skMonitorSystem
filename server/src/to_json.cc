#include "../include/to_json.h"

extern unsigned short kMaxProcess;

ToJSON::ToJSON() : procs_info_(kMaxProcess){}

void ToJSON::set_overallinfo(OverallInfo &overallinfo){
  overallinfo_ = overallinfo;
}

void ToJSON::set_processinfo(ProcessInfo &processinfo){
  procs_info_.swap(processinfo);
}
