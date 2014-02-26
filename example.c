#include "portnotify.h"
#include <stdio.h>

int main(){
  printf("New port: %hu\n", port_notify());
  return 0;
}
