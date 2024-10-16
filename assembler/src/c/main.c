#include <stdio.h>

#include "assembler.h"

int main()
{
  Assembler assembler;

  initAssembler(&assembler, "add G1, 23 + 2 ; This is a comment\nadd G2, 32 + 2");

  uint8_t *arr = assemble(&assembler);

  for (int i = 0; i < 10; i++)
  {
    printf("%d\n", arr[i]);
  }
}
