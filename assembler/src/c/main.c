#include <stdio.h>

#include "assembler.h"

int main() {
  Assembler assembler;

  Scanner scanner;
  initScanner(&scanner, "test: add G10, 23 + 1");

  Token tkn;
  while ((tkn = scanToken(&scanner)).type == TOKEN_END) {
    printf("%d\n", tkn.type);
  }

  initAssembler(&assembler, "jmp test\ntest: add G10, 23 + 1\nhalt");

  uint8_t *arr = assemble(&assembler);

  for (int i = 0; i < 10; i++) {
    printf("%d\n", arr[i]);
  }
}
