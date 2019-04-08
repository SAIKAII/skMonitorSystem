#include "../include/handler.h"
#include "../include/authentication.h"

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
    resource_[std::string("^/?(.*)$")][std::string("GET")] = [](std::ostream &response, std::shared_ptr<Connection> connection, bool &read_https){
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
    if(filename.rfind("first.html") == std::string::npos)
      read_https = true;

    std::cout << "read https: " << read_https << std::endl;
    read_file_and_generate_response(response, connection, filename, std::string());
  };

  resource_[std::string("^/?(.*).action$")][std::string("POST")] = [](std::ostream &response, std::shared_ptr<Connection> connection, bool &read_https){
    std::cout << "POST method" << std::endl;
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
    read_https = true;
  };
}
