#include <stdint.h>
#include <stdio.h>

#include "eval.h"
#include "scanner.h"
#include "vm.h"

int main() {
  VM vm;

  uint8_t arr[MEMORY_SIZE];

  // arr[0] = OP_CMP;
  // arr[1] = 0x0F;
  // arr[2] = 0x0F;
  // arr[3] = OP_JE;
  // arr[4] = 0xF0;
  // arr[5] = 0x01;
  // arr[6] = OP_HALT;
  // arr[0x01F0] = OP_ADD;
  // arr[0x01F1] = R_G3;
  // arr[0x01F2] = 0x01;
  // arr[0x01F3] = OP_RET;

  // initCpu(&vm, arr);

  // run(&vm);

  Expr expr;

  initExpr(&expr, "4 + (3 * 2) * (2 - 3) + 33");

  Scanner scanner;

  initScanner(&scanner, "mv G1, G10");

  Token tkn;

  while ((tkn = scanToken(&scanner)).type != TOKEN_END) {
    printf("%d\n", tkn.type);
  }
}
