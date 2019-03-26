#include "include/web.h"
#include "include/handler.h"

int main(){
  Handler *handler = Handler::get_instance();
  handler->handler_init();
  Web web(80);
  web.start();
  return 0;
}
