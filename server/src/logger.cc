#include "../include/logger.h"
#include <ctime>

std::string Logger::access_log_file_ = "./access_log.txt";
std::string Logger::run_log_file_ = "./run_log.txt";

void Logger::set_access_log_file(std::string &access_log_file){
  access_log_file_ = access_log_file;
}

void Logger::set_run_log_file(std::string &run_log_file){
  run_log_file_ = run_log_file;
}

void Logger::write_access_log(const std::string &ip, const std::string &user, const std::string &method, const std::string &path, const std::string &scheme){
  std::ofstream ofs(access_log_file_, std::ofstream::app);
  if(ofs){
    std::time_t now = std::time(0);
    std::string date(ctime(&now));
    date.pop_back();
    ofs << ip << "  " << user << "  [" << date << "]  " << method << "  " << path << "  " << scheme << std::endl;
    ofs.close();
  }
}

void Logger::write_access_log(const std::string &ip, const std::string &method, const std::string &path, const std::string &scheme){
  std::ofstream ofs(access_log_file_, std::ofstream::app);
  if(ofs){
    std::time_t now = std::time(0);
    std::string date(ctime(&now));
    date.pop_back();
    ofs << ip << "  guest" << "  [" << date << "]  " << method << "  " << path << "  " << scheme << std::endl;
    ofs.close();
  }
}

std::size_t Logger::read_log(std::ostream &os, bool access){
  std::ifstream ifs;
  if(true == access)
    ifs.open(access_log_file_);
  else
    ifs.open(run_log_file_);

  if(!ifs){
    return -1;
  }
  
  ifs.seekg(0, std::ios::end);
  std::size_t length = ifs.tellg();

  ifs.seekg(0, std::ios::beg);

  os << ifs.rdbuf();

  ifs.close();
  return length;
}

void Logger::write_run_log(const std::string &cmd, const char state, const unsigned short pid, const unsigned short ppid, const unsigned short userid, unsigned int pmem, unsigned int vmem, unsigned int cpu, unsigned int mem){
  std::ofstream ofs(run_log_file_, std::ofstream::app);
  if(ofs){
    std::time_t now = std::time(0);
    std::string date(ctime(&now));
    date.pop_back();
    ofs << "[" << date << "]  " << cmd << "  " << state << "  " << pid << "  " << ppid << "  " << userid << "  " << pmem << "  " << vmem << "  " << cpu << "  " << mem << std::endl;
    ofs.close();
  }
}
