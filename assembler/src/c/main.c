#include <stdio.h>
#include <string.h>

#include "assembler.h"

int main() {
  Scanner scanner;

  initScanner(&scanner, "\"Hello World\"add G0, 23 + 1");

  Token tkn;

  while ((tkn = scanToken(&scanner)).type != TOKEN_END) {
    char buf[64] = {'\0'};
    memcpy(buf, tkn.start, tkn.len);
    printf("%d\n", tkn.type);
  }
}
