#ifndef skMNTSYS_INCLUDE_HANDLER_H_
#define skMNTSYS_INCLUDE_HANDLER_H_

#include "http.h"
#include <memory>
#include <string>

void handler_init(){
  HTTP::resource_["^/?(.*)$"]["GET"] = [](std::ostream &response, std::shared_ptr<Connection> &connection){
    std::string filename = "www/";
    std::string path = connection->path_match_[1];
    // 防止使用".."来访问www/目录外的内容
    std::size_t last_pos = path.rfind('.');
    std::size_t current_pos = 0;
    std::size_t pos;
    while((pos = path.find('.', current_pos)) != std::string::npos && pos != last_pos){
      current_pos = pos;
      path.erase(pos, 1);
      --last_pos;
    }

    filename += path;
    std::ifstream ifs;
    if(filename.find('.') == std::string::npos){
      if(filename[filename.length()-1] != '/')
        filename += '/';
      filename += "index.html";
    }
    ifs.open(filename);

    if(ifs){
      ifs.seekg(0, std::ios::end);
      std::size_t length = ifs.tellg();

      ifs.seekg(0, std::ios::beg);

      // 组建响应
      response << "HTTP/1.1 200 OK\r\nContent-Length: " << length << "\r\n\r\n" << ifs.rdbuf();

      ifs.close();
    }else{
      std::string content = "Could not open file " + filename;
      response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << content.length() << "\r\n\r\n" << content;
    }
  }
}

#endif
