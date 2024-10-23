#include "error.h"

#include <stdio.h>
#include <string.h>

ErrorContext initErrorContext(int lineNumber, char *lineText, char *colStart,
                              int colLen, uint8_t errType, uint8_t errSeverity,
                              char *msg) {
  return (ErrorContext){.lineNumber = lineNumber,
                        .lineText = lineText,
                        .colStart = colStart,
                        .colLen = colLen,
                        .errType = errType,
                        .errSeverity = errSeverity,
                        .msg = msg};
}

void printContext(ErrorContext err) {
  printf("Line Number = %d,\nLine Text = %s,\nColumn Start = %s,\nColumn Len = "
         "%d,\nError Type = %d\nError Severity = %d\nMessage = %s.\n",
         err.lineNumber, err.lineText, err.colStart, err.colLen, err.errType,
         err.errSeverity, err.msg);
}
