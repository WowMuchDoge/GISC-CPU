#ifndef SCANNER_H_
#define SCANNER_H_

#include <stdbool.h>
#include <stdint.h>

enum TokenType {
  // Opcodes
  TOKEN_ADD,
  TOKEN_SUB,
  TOKEN_LD,
  TOKEN_MV,
  TOKEN_JMP,
  TOKEN_ADDR,
  TOKEN_SUBR,
  TOKEN_XOR,
  TOKEN_AND,
  TOKEN_OR,
  TOKEN_NAND,
  TOKEN_NOT,
  TOKEN_SHFT,
  TOKEN_ST,
  TOKEN_RET,
  TOKEN_CMP,
  TOKEN_JE,
  TOKEN_JNE,
  TOKEN_JG,
  TOKEN_JL,
  TOKEN_PUSH,
  TOKEN_POP,
  TOKEN_CALL,
  TOKEN_HALT,

  // Registers
  TOKEN_SR,
  TOKEN_SP,
  TOKEN_PC,
  TOKEN_SC,
  TOKEN_G0,
  TOKEN_G1,
  TOKEN_G2,
  TOKEN_G3,
  TOKEN_G4,
  TOKEN_G5,
  TOKEN_G6,
  TOKEN_G7,
  TOKEN_G8,
  TOKEN_G9,
  TOKEN_G10,

  // Directives
  TOKEN_DIR_STRING,
  TOKEN_DIR_ORG,
  TOKEN_DIR_START,

  // Other
  TOKEN_COMMA,
  TOKEN_COLON,
  TOKEN_IDENTIFIER,
  TOKEN_NUMBER,
  TOKEN_STRING,
  TOKEN_DOT,
  TOKEN_END
};

typedef enum TokenType TokenType;

struct Token {
  TokenType type;
  char *start;
  int len;
  int val;
  int line;
};

typedef struct Token Token;

struct Scanner {
  char *cur;
  int line;
  bool errorFlag;
};

typedef struct Scanner Scanner;

void initScanner(Scanner *scanner, char *src);

Token scanToken(Scanner *scanner);

#endif
