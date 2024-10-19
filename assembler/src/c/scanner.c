#include "scanner.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "eval.h"

#define MAX_IDENTIFIER_LEN 64

void initScanner(Scanner *scanner, char *src) {
  scanner->cur = src;
  scanner->line = 1;
  scanner->errorFlag = false;
}

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
  case 'o':
    return checkKeyword(start + 1, end, 2, "rg", TOKEN_DIR_ORG);
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
    if (len == 6) {
      return checkKeyword(start + 1, end, 5, "tring", TOKEN_DIR_STRING);
    } else if (len == 5) {
      return checkKeyword(start + 1, end, 4, "tart", TOKEN_DIR_START);
    } else if (end - start > 2) {
      return checkKeyword(start + 1, end, 3, "ubr", TOKEN_ADDR);
    } else if (end - start > 1) {
      return checkKeyword(start + 1, end, 2, "ub", TOKEN_ADD);
    } else if (len == 2) {
      return !isAlpha(start[2]) ? TOKEN_ST : TOKEN_IDENTIFIER;
    }
  }
  case 'S': {
    if (len == 2) {
      switch (start[1]) {
      case 'P':
        return TOKEN_SP;
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
  case 'h': {
    return checkKeyword(start + 1, end, 3, "alt", TOKEN_HALT);
  }
  }

  return TOKEN_IDENTIFIER;
}

static bool isNum(char c) { return c >= '0' && c <= '9'; }

static bool isAlphaNumeric(char c) { return isAlpha(c) || isNum(c); }

static void walkToWhitespace(Scanner *scanner) {
  while (peek(scanner) != ' ')
    advance(scanner);
}

static void walkToNewline(Scanner *scanner) {
  while (peek(scanner) != '\n')
    advance(scanner);
}

Token scanToken(Scanner *scanner) {
  while (peek(scanner) != '\0') {
    char c = peek(scanner);
    switch (*scanner->cur) {
    case ',':
      advance(scanner);
      return (Token){TOKEN_COMMA, scanner->cur - 1, 1, 0, scanner->line};
    case ':':
      advance(scanner);
      return (Token){TOKEN_COLON, scanner->cur - 1, 1, 0, scanner->line};
    case ' ':
      advance(scanner);
      break;
    case '\n':
      scanner->line++;
      advance(scanner);
      break;
    case ';':
      while (peek(scanner) != '\n' && peek(scanner) != '\0')
        advance(scanner);
      break;
    case '.':
      advance(scanner);
      return (Token){TOKEN_DOT, scanner->cur - 1, 1, 0, scanner->line};
    case '"': {
      advance(scanner);

      char *start = scanner->cur;
      char t = peek(scanner);

      while ((t = advance(scanner)) != '"') {
        if (isAtEnd(scanner)) {
          printf("Unterminated string.\n");
          exit(-1);
        }
      }

      return (Token){TOKEN_STRING, start, scanner->cur - start - 1, 0,
                     scanner->line};
    }
    default: {
      if (isAlpha(*scanner->cur)) {
        char *start = scanner->cur;

        while (isAlphaNumeric(*(++scanner->cur)))
          ;

        Token tkn = {TOKEN_IDENTIFIER, start, scanner->cur - start, 0,
                     scanner->line};

        tkn.type = getKeyword(start, scanner->cur - 1, scanner->cur - start);

        return tkn;
      } else if (isNum(*scanner->cur)) {
        char *start = scanner->cur;

        while (peek(scanner) != '\n' && peek(scanner) != '\0')
          advance(scanner);

        if (scanner->cur - start > MAX_EXPR_LEN) {
          printf("Exceeded max expression length.\n");
          walkToNewline(scanner);
          scanner->errorFlag = false;
          break;
        }

        char exprString[128] = {'\0'};

        memcpy(exprString, start, (scanner->cur - start) + 1);

        Expr expr;
        initExpr(&expr, exprString);

        return (Token){TOKEN_NUMBER, exprString, scanner->cur - start,
                       evaluate(&expr), scanner->line};
      } else {
        printf("Unkown character '%c'.\n", *scanner->cur);
        walkToWhitespace(scanner);
        scanner->errorFlag = true;
      }
    }
    }
  }
  return (Token){TOKEN_END, "", 3, 0};
}
