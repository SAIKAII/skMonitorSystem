#include "include/web.h"
#include "include/handler.h"

int main(){
  handler_init();
  Web web(80);
  web.start();
  return 0;
}
