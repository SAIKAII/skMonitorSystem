#ifndef skMNTSYS_INCLUDE_UTILITY_H_
#define skMNTSYS_INCLUDE_UTILITY_H_

#include <memory>
#include <string>
#include <regex>

class RequestMessage{
public:
  static bool parse(std::istream &stream, std::string &method, std::string &path, std::string &query_string, std::string &version, std::unordered_multimap<std::string, std::string> &header) noexcept{
    header.clear();
    std::string line;
    std::getline(stream, line);
    std::size_t method_end;
    std::regex regex_line("^([^ ]*) ([^ ]*) HTTP/([^ ]*)$");
    std::smatch sub_match;

    // getline读取到\n，并不读\n，所以最后剩下\r
    line.pop_back();
    if(std::regex_match(line, sub_match, regex_line)){
      method = sub_match[1];
      path = sub_match[2];
      version = sub_match[3];

      std::regex regex_path("^([^ ])*\?([^ ]*)$");
      if(std::regex_match(path, sub_match, regex_path))
        query_string = sub_match[2];

      bool matched;
      regex_line = "^([^:]*): ?(.*)$";
      do{
        std::getline(stream, line);
        line.pop_back();
        matched = std::regex_match(line, sub_match, regex_line);
        if(true == matched){
          header[sub_match[1]] = sub_match[2];
        }
      }while(true == matched);
    }else{
      return false;
    }
    return true;
  }
};

#endif
