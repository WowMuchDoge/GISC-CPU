#include <stdint.h>
#include <stdio.h>

#include "vm.h"

int main() {
  VM vm;

  uint8_t arr[MEMORY_SIZE];

  arr[0] = 5;
  arr[1] = 3;
  arr[2] = 0;
  arr[3] = 1;
  arr[4] = 14;
  arr[5] = 24;
  arr[6] = 23;

  initCpu(&vm, arr);

  run(&vm);
}
