#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assembler.h"

char *readFile(char *filename) {
  FILE *fileptr;

  fileptr = fopen(filename, "r");

  if (!fileptr) {
    printf("Error opening '%s'.\n", filename);
    exit(-1);
  }

  fseek(fileptr, 0, SEEK_END);
  int len = ftell(fileptr);

  rewind(fileptr);

  char *text = malloc(len * sizeof(char));

  fread(text, len, len, fileptr);

  return text;
}

int main(int count, char **args) {
  if (count == 1) {
    printf("Expected second argument.\n");
    exit(-1);
  }

  char *s = readFile(args[1]);

  Assembler assembler;

  initAssembler(&assembler, s);

  byte *bytes = assemble(&assembler);

  for (int i = 0; i < 25; i++) {
    printf("%d\n", bytes[i]);
  }

  free(s);
}
