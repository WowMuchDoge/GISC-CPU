#include <stdio.h>

#include "vm.h"
#include <stdint.h>

int main() {
  VM vm;

  uint8_t arr[MEMORY_SIZE];

  arr[0] = OP_JMP;
  arr[1] = 0xF0;
  arr[2] = 0x01;
  arr[3] = OP_ADD;
  arr[4] = R_G1;
  arr[5] = 0x0F;
  arr[6] = OP_HALT;
  arr[0x01F0] = OP_ADD;
  arr[0x01F1] = R_G3;
  arr[0x01F2] = 0x01;
  arr[0x01F3] = OP_RET;

  initCpu(&vm, arr);

  run(&vm);

  printf("%04X\n", arr[0x0FFF]);
}
