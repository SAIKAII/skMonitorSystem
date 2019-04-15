#include "../include/handler.h"
#include "../include/authentication.h"
#include "../include/to_json.h"
#include "../include/logger.h"
#include "../include/total_format.h"
#include "../include/mem_format.h"
#include "../include/usage_format.h"
#include <boost/asio.hpp>
#include <iostream>
#include <signal.h>
#include <sys/types.h>

void read_file_and_generate_response(std::ostream &response, std::shared_ptr<Connection> connection, const std::string &filename, std::string cookie){
  std::ifstream ifs(filename);
  if(ifs){
    ifs.seekg(0, std::ios::end);
    std::size_t length = ifs.tellg();

    ifs.seekg(0, std::ios::beg);

    // 组建响应
    response << "HTTP/1.1 200 OK\r\n";
    if(std::stof(connection->http_version_) > 1.05) // 持久连接
      response << "Connection: keep-alive\r\n";
    if(cookie.length() > 0)
      response << "Set-Cookie: my_cookie=" << cookie << ";Secure\r\n";
    response << "Content-Type: text/html; charset=utf-8\r\n";
    // response << "Content-Encoding: gzip\r\n";
    response << "Content-Length: " << length << "\r\n\r\n" << ifs.rdbuf();

    ifs.close();
  }else{
    std::string content = "Could not open file " + filename;
    response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << content.length() << "\r\n\r\n" << content;
  }
}

bool auth_token(std::ostream &response, std::shared_ptr<Connection> connection){
  if(!Authentication::auth_token(connection->token_)){
    Logger::write_access_log(connection->remote_endpoint_.address().to_string(), connection->method_, connection->path_, "HTTP/"+connection->http_version_);
    std::string content = "You have no right to do this.";
    response << "HTTP/1.1 403 Forbidden\r\nContent-Length: " << content.length() << "\r\n\r\n" << content;
    return false;
  }
  return true;
}

void Handler::handler_init(){
  resource_[std::string("^/?(index.html)?$")][std::string("GET")] = [](std::ostream &response, std::shared_ptr<Connection> connection){
    std::string filename;
    if(Authentication::auth_token(connection->token_)){
      Logger::write_access_log(connection->remote_endpoint_.address().to_string(), connection->token_, connection->method_, connection->path_, "HTTP/"+connection->http_version_);
      filename = "www/first.html";
    }else{
      Logger::write_access_log(connection->remote_endpoint_.address().to_string(), connection->method_, connection->path_, "HTTP/"+connection->http_version_);
      filename = "www/index.html";
    }
    read_file_and_generate_response(response, connection, filename, std::string());
  };

  resource_[std::string("^/?first.html$")][std::string("GET")] = [](std::ostream &response, std::shared_ptr<Connection> connection){
    if(!auth_token(response, connection))
      return;

    std::string filename = "www/first.html";
    connection->display_format_ = std::make_shared<TotalFormat>();
    read_file_and_generate_response(response, connection, filename, std::string());
  };

  resource_[std::string("^/?usage.html$")][std::string("GET")] = [](std::ostream &response, std::shared_ptr<Connection> connection){
    if(!auth_token(response, connection))
      return;

    std::string filename = "www/usage.html";
    connection->display_format_ = std::make_shared<UsageFormat>();
    read_file_and_generate_response(response, connection, filename, std::string());
  };

  resource_[std::string("^/?mem.html$")][std::string("GET")] = [](std::ostream &response, std::shared_ptr<Connection> connection){
    if(!auth_token(response, connection))
      return;

    std::string filename = "www/mem.html";
    connection->display_format_ = std::make_shared<MemFormat>();
    read_file_and_generate_response(response, connection, filename, std::string());
  };

  resource_[std::string("^/?login.action$")][std::string("POST")] = [](std::ostream &response, std::shared_ptr<Connection> connection){
    Logger::write_access_log(connection->remote_endpoint_.address().to_string(), connection->method_, connection->path_, "HTTP/"+connection->http_version_);
    std::istream is(&connection->read_buffer_);
    std::string username, pwd, username_pwd;

    std::getline(is, username_pwd);
    std::string::size_type start, end;
    if(start = username_pwd.find("=", username_pwd.find("username"))+1){
      end = username_pwd.find("&");
      username = end != std::string::npos ? std::string(username_pwd, start, end-start) : std::string(username_pwd, start);
    }
    if(start = username_pwd.find("=", username_pwd.find("pwd"))+1){
      end = username_pwd.find("&");
      pwd = end != std::string::npos ? std::string(username_pwd, start, end-start) : std::string(username_pwd, start);
    }

    if(username == "admin" && pwd == "admin"){
      std::string token = Authentication::generator_token(username + pwd);
      connection->token_ = token;

      std::string filename = "www/first.html";
      read_file_and_generate_response(response, connection, filename, token);
      return;
    }

    std::string content = "You have no right to access.";
    response << "HTTP/1.1 403 Forbidden\r\nContent-Length: " << content.length() << "\r\n\r\n" << content;
  };

  resource_[std::string("^/?power_change.action$")][std::string("POST")] = [](std::ostream &response, std::shared_ptr<Connection> connection){
    if(!auth_token(response, connection))
      return;

    Logger::write_access_log(connection->remote_endpoint_.address().to_string(), connection->token_, connection->method_, connection->path_, "HTTP/"+connection->http_version_);
    std::istream is(&connection->read_buffer_);
    std::string method;

    std::getline(is, method);
    std::string::size_type start, end;
    if((start = method.find("method")) == std::string::npos){
      std::string content = "Syntax fail.";
      response << "HTTP/1.1 400 Bad request\r\nContent-Length: " << content.length() << "\r\n\r\n" << content;
      return;
    }

    start = method.find("=") + 1;
    end = method.find("&", start);

    std::string m = end != std::string::npos ? std::string(method, start, end-start) : std::string(method, start);

    if("cpu_first" == m){
      //std::cout << "CPU first" << std::endl;
      ToJSON::sort_method_ = ToJSON::CPU_FIRST;
    }else if("mem_first" == m){
      //std::cout << "MEM first" << std::endl;
      ToJSON::sort_method_ = ToJSON::MEM_FIRST;
    }else{
      //std::cout << "Normal" << std::endl;
      ToJSON::sort_method_ = ToJSON::NORMAL;
    }

    std::string content = "Request success.";
    response << "HTTP/1.1 200 OK\r\nContent-Length: " << content.length() << "\r\n\r\n" << content;
  };

  resource_[std::string("^/?(.*).txt")]["GET"] = [](std::ostream &response, std::shared_ptr<Connection> connection){
    if(!auth_token(response, connection))
      return;

    Logger::write_access_log(connection->remote_endpoint_.address().to_string(), connection->token_, connection->method_, connection->path_, "HTTP/"+connection->http_version_);
    boost::asio::streambuf sb;
    std::ostream os(&sb);
    std::size_t length = -1;
    if(connection->path_ == "/access_log.txt")
      length = Logger::read_log(os, true);
    else if(connection->path_ == "/run_log.txt")
      length = Logger::read_log(os, false);

    if(-1 == length){
      std::string content = "Could not open file " + connection->path_;
      response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << content.length() << "\r\n\r\n" << content;
      return;
    }

    response << "HTTP/1.1 200 OK\r\n";
    if(std::stof(connection->http_version_) > 1.05)
      response << "Connection: keep-alive\r\n";
    response << "Content-Length: " << length << "\r\n\r\n" << std::istream(&sb).rdbuf();
  };

  resource_[std::string("^/?kill_process.action$")][std::string("POST")] = [](std::ostream &response, std::shared_ptr<Connection> connection){
    if(!auth_token(response, connection))
      return;

    std::istream is(&connection->read_buffer_);
    std::string processid, pid, method;

    std::getline(is, processid);
    std::string::size_type start, end;
    if(start = processid.find("=", processid.find("pid"))+1){
      end = processid.find("&");
      pid = end != std::string::npos ? std::string(processid, start, end-start) : std::string(processid, start);
    }
    if(start = processid.find("=", processid.find("method"))+1){
      end = processid.find("&");
      method = end != std::string::npos ? std::string(processid, start, end-start) : std::string(processid, start);
    }
    if("normal" == method)
      kill(std::stoi(pid), SIGTERM);
    else if("force" == method)
      kill(std::stoi(pid), SIGKILL);

    std::string content = "Command had been ran";
    response << "HTTP/1.1 200 OK\r\n";
    if(std::stof(connection->http_version_) > 1.05)
      response << "Connection: keep-alive\r\n";
    response << "Content-Length: " << content.length() << "\r\n\r\n" << content;
  };

  default_resource_[std::string("^/?(.*)$")][std::string("GET")] = [](std::ostream &response, std::shared_ptr<Connection> connection){
    std::string filename = "www/";
    std::string path = connection->path_match_[1];
    // 防止使用".."来访问www/目录外的内容
    // std::size_t last_pos = path.rfind('.');
    std::size_t current_pos = 0;
    std::size_t pos;
    while((pos = path.find('.', current_pos)) != std::string::npos && path[pos+1] == '.'){
    // while((pos = path.find('.', current_pos)) != std::string::npos && pos != last_pos){
      current_pos = pos;
      path.erase(pos, 1);
      // --last_pos;
    }

    if(0 < path.length())
      filename += path;

    if(Authentication::auth_token(connection->token_)){
      Logger::write_access_log(connection->remote_endpoint_.address().to_string(), connection->token_, connection->method_, connection->path_, "HTTP/"+connection->http_version_);
      if(0 == path.length())
        filename += "first.html";
    }else{
      Logger::write_access_log(connection->remote_endpoint_.address().to_string(), connection->method_, connection->path_, "HTTP/"+connection->http_version_);
      filename = "www/index.html";
    }

    if(filename.find('.') == std::string::npos){
      filename = "www/index.html";
    }
    //std::cout << "filename: " << filename << std::endl;

    read_file_and_generate_response(response, connection, filename, std::string());
  };

  for(auto it = resource_.begin(); it != resource_.end(); ++it)
    all_resource_.push_back(it);
  for(auto it = default_resource_.begin(); it != default_resource_.end(); ++it)
    all_resource_.push_back(it);
}
