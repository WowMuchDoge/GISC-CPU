#ifndef ERROR_H_
#define ERROR_H_

#include <stdint.h>

enum ErrorType {
  ERR_NONE = 1,
  ERR_MAX_INSTRUCTION_EXCEEDED,
  ERR_UNEXPECTED_TOKEN,
  ERR_EXPECTED_TOKEN,
  ERR_MAX_EXPR_LEN,
  ERR_EXPECTED_OPEN_PAREN,
  ERR_UNKOWN_CHARACTER,
  ERR_DIVISION_BY_ZERO,
  ERR_UNKNOWN_ELEMENT,
  ERR_OPENING_FILE,
  ERR_INVALID_ARGS,
  ERR_UNTERMINATED_STRING
};

enum ErrorSeverity { S_WARNING = 1, S_ERROR };

struct ErrorContext {
  int lineNumber;
  char *lineText;

  char *colStart;
  int colLen;

  uint8_t errType;
  uint8_t errSeverity;

  char *msg;
};

typedef struct ErrorContext ErrorContext;

// Create ErrorContext object
ErrorContext initErrorContext(int lineNumber, char *lineText, char *colStart,
                              int colLen, uint8_t errType, uint8_t errSeverity,
                              char *msg);

// Placeholder right now
void printContext(ErrorContext err);

#endif
