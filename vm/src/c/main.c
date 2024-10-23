#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "vm.h"

void readFile(char *filename, uint8_t buf[MEMORY_SIZE]) {
  FILE *fptr;

  fptr = fopen(filename, "r");

  if (!fptr) {
    printf("Cannot open file '%s'.", filename);
    exit(-1);
  }

  fseek(fptr, 0, SEEK_END);

  if (ftell(fptr) != MEMORY_SIZE) {
    printf("File must be of size 0xFFFF.\n");
    exit(-1);
  }

  rewind(fptr);

  fread(buf, sizeof(char), 0xFFFF, fptr);

  fclose(fptr);
}

int main(int count, char **args) {
  VM vm;

  uint8_t arr[MEMORY_SIZE];

  if (count < 2) {
    printf("Expected file to be read.\n");
  } else if (count > 2) {
    printf("Too many args.\n");
  }

  readFile(args[1], arr);

  initCpu(&vm, arr);

  run(&vm);
}
