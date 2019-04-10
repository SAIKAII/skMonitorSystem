#include "../include/logger.h"
#include <ctime>

std::string Logger::access_log_file_ = "./access_log.txt";

void Logger::set_access_log_file(std::string &access_log_file){
  access_log_file_ = access_log_file;
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

std::size_t Logger::read_access_log(std::ostream &os){
  std::ifstream ifs(access_log_file_);
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
