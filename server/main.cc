#include "include/web.h"
#include "include/handler.h"
#include "include/get_info.h"
#include "include/to_json.h"
#include <chrono>
#include <ctime>
#include <iostream>
#include <thread>

int main(){
  Handler *handler = Handler::get_instance();
  handler->handler_init();
  Web web(80);
  std::thread t([](){
    GetInfo get_info;
    ToJSON to_json;
    while(true){
      std::this_thread::sleep_for(std::chrono::seconds(3));
      std::cout << "wake up!" << std::endl;
      get_info.get_info(to_json);
    }
  });
  web.start();
  return 0;
}
