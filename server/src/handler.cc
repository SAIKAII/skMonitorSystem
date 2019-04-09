#include "../include/handler.h"
#include "../include/authentication.h"
#include "../include/to_json.h"

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

void Handler::handler_init(){
    resource_[std::string("^/?(.*)$")][std::string("GET")] = [](std::ostream &response, std::shared_ptr<Connection> connection){
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
      if(0 == path.length())
        filename += "first.html";
    }else
      filename = "www/index.html";

    if(filename.find('.') == std::string::npos){
      filename = "www/index.html";
    }
    std::cout << "filename: " << filename << std::endl;

    read_file_and_generate_response(response, connection, filename, std::string());
  };

  resource_[std::string("^/?login.action$")][std::string("POST")] = [](std::ostream &response, std::shared_ptr<Connection> connection){
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

    std::cout << "POST method" << std::endl;
    if(username == "admin" && pwd == "admin"){
      std::string token = Authentication::generator_token(username + pwd);
      connection->token_ = token;

      std::string filename = "www/first.html";
      read_file_and_generate_response(response, connection, filename, token);
      std::cout << "post success!" << std::endl;
      return;
    }

    std::string content = "You have no right to access.";
    response << "HTTP/1.1 403 Forbidden\r\nContent-Length: " << content.length() << "\r\n\r\n" << content;
  };

  resource_[std::string("^/?power_change.action$")][std::string("POST")] = [](std::ostream &response, std::shared_ptr<Connection> connection){
    if(!Authentication::auth_token(connection->token_)){
      std::string content = "You have no right to do this.";
      response << "HTTP/1.1 403 Forbidden\r\nContent-Length: " << content.length() << "\r\n\r\n" << content;
      return;
    }

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
      std::cout << "CPU first" << std::endl;
      ToJSON::sort_method_ = ToJSON::CPU_FIRST;
    }else if("mem_first" == m){
      std::cout << "MEM first" << std::endl;
      ToJSON::sort_method_ = ToJSON::MEM_FIRST;
    }else{
      std::cout << "Normal" << std::endl;
      ToJSON::sort_method_ = ToJSON::NORMAL;
    }

    std::string content = "Request success.";
    response << "HTTP/1.1 200 OK\r\nContent-Length: " << content.length() << "\r\n\r\n" << content;
  };
}
