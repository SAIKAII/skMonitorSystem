#ifndef MNTSYS_INCLUDE_TO_JSON_H_
#define MNTSYS_INCLUDE_TO_JSON_H_

#include "resource_info.h"
#include <vector>

const unsigned short kMaxProcess = 250;

class ToJSON{
public:
  ToJSON();
  void set_overallinfo(OverallInfo &overallinfo);
  void set_processinfo(ProcessInfo &processinfo);

private:
  OverallInfo overallinfo_;
  std::vector<ProcessInfo> procs_info_;
};

#endif
