#ifndef skMNTSYS_INCLUDE_LOGGER_H_
#define skMNTSYS_INCLUDE_LOGGER_H_

#include <string>
#include <iostream>
#include <fstream>

class Logger{
public:
  static void set_access_log_file(std::string &log_file);
  static void set_run_log_file(std::string &run_file);
  static void write_access_log(const std::string &ip, const std::string &user, const std::string &method, const std::string &path, const std::string &scheme);
  static void write_access_log(const std::string &ip, const std::string &method, const std::string &path, const std::string &scheme);
  static std::size_t read_log(std::ostream &os, bool access);
  static void write_run_log(const std::string &cmd, const char state, const unsigned short pid, const unsigned short ppid, const unsigned short userid, unsigned int pmem, unsigned int vmem, unsigned int cpu, unsigned int mem);

private:
  static std::string access_log_file_;
  static std::string run_log_file_;
};

#endif
