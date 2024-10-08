#include "scanner.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define MAX_IDENTIFIER_LEN 64

void initScanner(Scanner *scanner, char *src) { scanner->cur = src; }

static bool isAlpha(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

static char peek(Scanner *scanner) { return *scanner->cur; }

static bool isAtEnd(Scanner *scanner) { return peek(scanner) == '\0'; }

static char advance(Scanner *scanner) {
  char r = peek(scanner);

  if (isAtEnd(scanner)) {
    return r;
  }

  scanner->cur++;
  return r;
}

static TokenType checkKeyword(char *str, char *end, int len, const char *comp,
                              TokenType type) {

  int a = end - str;
  int b = len;

  if (end - str == len - 1 && memcmp(str, comp, len) == 0) {
    return type;
  }

  return TOKEN_IDENTIFIER;
}

static TokenType getKeyword(char *start, char *end, int len) {
  switch (start[0]) {
  case 'r':
    return checkKeyword(start + 1, end, 2, "et", TOKEN_RET);
  case 'm':
    return checkKeyword(start + 1, end, 1, "v", TOKEN_MV);
  case 'x':
    return checkKeyword(start + 1, end, 2, "or", TOKEN_XOR);
  case 'j': {
    if (end - start >= 1) {
      switch (start[1]) {
      case 'm':
        return checkKeyword(start + 2, end, 1, "p", TOKEN_JMP);
      case 'e':
        return !isAlpha(start[2]) ? TOKEN_JE : TOKEN_IDENTIFIER;
      case 'n':
        return checkKeyword(start + 2, end, 1, "e", TOKEN_JNE);
      case 'g':
        return !isAlpha(start[2]) ? TOKEN_JG : TOKEN_IDENTIFIER;
      case 'l':
        return !isAlpha(start[2]) ? TOKEN_JL : TOKEN_IDENTIFIER;
      default:
        return TOKEN_IDENTIFIER;
      }
    }
  }
  case 'a': {
    if (end - start > 2) {
      return checkKeyword(start + 1, end, 3, "ddr", TOKEN_ADDR);
    } else if (end - start > 1) {
      return checkKeyword(start + 1, end, 2, "dd", TOKEN_ADD);
    }
  }
  case 's': {
    if (end - start > 2) {
      return checkKeyword(start + 1, end, 3, "ubr", TOKEN_ADDR);
    } else if (end - start > 1) {
      return checkKeyword(start + 1, end, 2, "ub", TOKEN_ADD);
    }
  }
  case 'S': {
    if (end - start == 1) {
      switch (start[1]) {
      case 'T':
        return !isAlpha(start[1]) ? TOKEN_ST : TOKEN_IDENTIFIER;
      case 'R':
        return !isAlpha(start[1]) ? TOKEN_SR : TOKEN_IDENTIFIER;
      case 'P':
        return !isAlpha(start[1]) ? TOKEN_SP : TOKEN_IDENTIFIER;
      }
    }
  }
  case 'n': {
    if (end - start > 1) {
      switch (start[1]) {
      case 'a':
        return checkKeyword(start + 2, end, 2, "nd", TOKEN_NAND);
      case 'o':
        return checkKeyword(start + 2, end, 1, "t", TOKEN_NOT);
      }
    }
  }
  case 'G': {
    if (len == 3) {
      return checkKeyword(start + 1, end, 2, "10", TOKEN_G10);
    } else if (len == 2) {
      switch (start[1]) {
      case '0':
        return TOKEN_G0;
      case '1':
        return TOKEN_G1;
      case '2':
        return TOKEN_G2;
      case '3':
        return TOKEN_G3;
      case '4':
        return TOKEN_G4;
      case '5':
        return TOKEN_G5;
      case '6':
        return TOKEN_G6;
      case '7':
        return TOKEN_G7;
      case '8':
        return TOKEN_G8;
      case '9':
        return TOKEN_G9;
      }
    }
  }
  }

  return TOKEN_IDENTIFIER;
}

static bool isNum(char c) { return c >= '0' && c <= '9'; }

static bool isAlphaNumeric(char c) { return isAlpha(c) || isNum(c); }

Token scanToken(Scanner *scanner) {
  while (peek(scanner) != '\0') {
    switch (*scanner->cur) {
    case ',':
      advance(scanner);
      return (Token){TOKEN_COMMA, ",", 0, 0};
    case ':':
      advance(scanner);
      return (Token){TOKEN_COLON, ":", 0, 0};
    case ' ':
      advance(scanner);
    default: {
      if (isAlpha(*scanner->cur)) {
        char *start = scanner->cur;

        while (isAlphaNumeric(*(++scanner->cur)))
          ;

        Token tkn = {TOKEN_IDENTIFIER, "", scanner->cur - start, 0};

        tkn.type = getKeyword(start, scanner->cur - 1, scanner->cur - start);

        return tkn;
      }
    }
    }
  }
  return (Token){TOKEN_END, "", 3, 0};
}
