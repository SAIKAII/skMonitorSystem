#include "include/web.h"
#include "include/handler.h"
#include "include/get_info.h"
#include "include/to_json.h"
#include <chrono>
#include <ctime>
#include <iostream>
#include <thread>
#include <memory>

int main(){
  Handler *handler = Handler::get_instance();
  handler->handler_init();
  Web web(443, "server.crt", "server.key");
  std::thread t([](){
    GetInfo get_info;
    ToJSON to_json;
    while(true){
      std::this_thread::sleep_for(std::chrono::seconds(3));
      get_info.get_info(to_json);
    }
  });
  web.start();
  return 0;
}
