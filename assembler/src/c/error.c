#include "error.h"

#include <stdio.h>
#include <string.h>

#define MAX_LINE_LEN 256
#define LINE_NUM_WIDTH 6

static void formatLineNum(char *s, int n) {
  int numLen, i;

  for (numLen = 0, i = 1; i < n; i *= 10, numLen++)
    ;

  sprintf(s, " %d", n);

  for (int j = numLen + 1; j < LINE_NUM_WIDTH - numLen; j++) {
    s[j] = ' ';
  }

  s[4] = '|';
  s[5] = ' ';
}

void printError(int lineNum, char *line, char *start, int len, char *location,
                char *msg, char *filename) {
  printf("[%s:%d] %s Error: %s\n", filename, lineNum, location, msg);

  char buffer1[MAX_LINE_LEN] = {'\0'};

  formatLineNum(buffer1, lineNum);

  for (char *cur = line; *cur != '\n' && *cur != '\0'; cur++) {
    buffer1[(cur - line) + 6] = *cur;
  }

  printf("%s\n", buffer1);

  char buffer2[MAX_LINE_LEN] = {'\0'};

  sprintf(buffer2, "%s", "    | ");

  for (int i = 0; i < start - line; i++) {
    strcat(buffer2, " ");
  }

  strcat(buffer2, "^");

  for (int i = 0; i < len - 1; i++) {
    strcat(buffer2, "~");
  }

  printf("%s\n", buffer2);
}
