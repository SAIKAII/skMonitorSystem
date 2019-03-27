#include "../include/http.h"
#include <regex>
#include <string>
#include <map>
#include <unordered_map>

void HTTP::respond(){
  Handler *handler = Handler::get_instance();
  for(auto res_it : handler->resource_){
    std::regex express(res_it.first);
    std::smatch sm_res;
    if(std::regex_match(connection_->path_, sm_res, express)){
      if(res_it.second.count(connection_->method_) > 0){
        connection_->path_match_ = std::move(sm_res);

        auto write_buffer = std::make_shared<asio::streambuf>();
        std::ostream response(write_buffer.get());
        res_it.second[connection_->method_](response, connection_);

        auto self = this->shared_from_this();
        asio::async_write(*connection_->socket_, *write_buffer, [self](const error_code &ec, std::size_t /* bytes_transferred */){
          // 暂不处理
          std::cout << "HTTP write complete!" << std::endl;
        });
        return;
      }
    }
  }
}
