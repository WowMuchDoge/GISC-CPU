#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assembler.h"
#include "disassembler.h"

void readFile(char *filename, char buf[0xFFFF]) {
  FILE *fileptr;

  fileptr = fopen(filename, "r");

  if (!fileptr) {
    printf("Error opening '%s'.\n", filename);
    exit(-1);
  }

  fseek(fileptr, 0, SEEK_END);
  int len = ftell(fileptr);

  rewind(fileptr);

  fread(buf, len, len, fileptr);

  fclose(fileptr);
}

void writeBinary(uint8_t *bytes, int size, const char *filename) {
  FILE *fileptr;

  fileptr = fopen(filename, "wb");

  if (!fileptr) {
    printf("Unexpected error opening file.\n");
    exit(-1);
  }

  fwrite(bytes, sizeof(uint8_t), size, fileptr);

  fclose(fileptr);
}

int main(int count, char **args) {
  if (count == 1) {
    printf("Expected file argument.\n");
    exit(-1);
  } else if (count == 2) {
    printf("Expected output.\n");
    exit(-1);
  }

  char source[0xFFFF];

  readFile(args[1], source);

  Assembler assembler;

  initAssembler(&assembler, source, args[1]);

  assemble(&assembler);

  writeBinary(assembler.output, BYTE_MAX, args[2]);

  //disassemble(bytes, 50);

  // for (int i = 0; i < 50; i++) {
  //   printf("0x%04X: 0x%02x\n", i, bytes[i]);
  // }

  printf("%ld\n", sizeof(Scanner));

  freeAssembler(&assembler);
}
