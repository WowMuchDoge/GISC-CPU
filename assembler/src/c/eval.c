#include "eval.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void initExpr(Expr *expr, char *src) {
  expr->cur = src;

  expr->outputHead = 0;
  expr->opStackHead = 0;

  memset(expr->output, '\0', MAX_STACK);
}

int precedence[] = {['+'] = 1, ['-'] = 1, ['*'] = 2, ['/'] = 2, ['('] = 2};

static bool isNum(char c) { return c >= '0' && c <= '9'; }

static bool isOp(char c) {
  return c == '+' || c == '-' || c == '*' || c == '/';
}

static void pushOp(Expr *expr, char op) {
  while (expr->opStackHead > 0 &&
         precedence[op] < precedence[expr->opStack[expr->opStackHead - 1]]) {
    expr->output[expr->outputHead++] = expr->opStack[--(expr->opStackHead)];
  }

  expr->opStack[expr->opStackHead++] = op;
}

static void dumpOps(Expr *expr) {
  while (expr->opStackHead > 0) {
    if (expr->outputHead > MAX_EXPR_LEN) {
      printf("Exceeded max expression length.\n");
      exit(-1);
    }
    expr->output[expr->outputHead++] = expr->opStack[--(expr->opStackHead)];
  }
}

static void walkBack(Expr *expr) {
  while (expr->opStack[expr->opStackHead - 1] != '(') {
    if (expr->opStackHead == 0) {
      printf("Expected opening '('.\n");
      exit(-1);
    }
    expr->output[expr->outputHead++] = expr->opStack[--(expr->opStackHead)];
  }

  expr->opStackHead--;
}

static bool isMath(char c) { return isNum(c) || isOp(c) || c == ' '; }

static void compile(Expr *expr) {
  while (isMath(*expr->cur)) {
    if (isOp(*(expr->cur))) {
      pushOp(expr, *(expr->cur));
      expr->cur++;
    } else if (isNum(*(expr->cur))) {
      if (expr->outputHead > 127) {
        printf("Exceeded max expression size.\n");
        exit(-1);
      }
      while (isNum(*(expr->cur))) {
        expr->output[expr->outputHead++] = *(expr->cur++);
      }
      expr->output[expr->outputHead++] = ' ';
    } else if (*(expr->cur) == '(') {
      expr->opStack[expr->opStackHead++] = '(';
      expr->cur++;
    } else if (*(expr->cur) == ')') {
      walkBack(expr);
      expr->cur++;
    } else if (*(expr->cur) == ' ') {
      expr->cur++;
    } else {
      printf("Unkown character '%c'.\n", *(expr->cur));
      exit(-1);
    }
  }
  dumpOps(expr);
}

int evaluate(Expr *expr) {
  compile(expr);

  char *cur = expr->output;

  int stack[MAX_STACK];
  int stackHead = 0;

  while (*cur != '\0') {
    switch (*cur) {
    case '+': {
      int b = stack[--stackHead];
      int a = stack[--stackHead];

      stack[stackHead++] = a + b;

      cur++;
      break;
    }
    case '-': {
      int b = stack[--stackHead];
      int a = stack[--stackHead];

      stack[stackHead++] = a - b;
      cur++;
      break;
    }
    case '*': {
      int b = stack[--stackHead];
      int a = stack[--stackHead];

      stack[stackHead++] = a * b;
      cur++;
      break;
    }
    case '/': {
      int b = stack[--stackHead];
      int a = stack[--stackHead];

      if (b == 0) {
        printf("Division by 0 error.\n");
        exit(-1);
      }

      stack[stackHead++] = (uint8_t)(a / b);
      cur++;
      break;
    }
    case ' ': {
      cur++;
      break;
    }
    default:
      if (isNum(*cur)) {
        char *start = cur;

        while (isNum(*(cur++)))
          ;

        char *num = malloc(((cur - start) + 1) * sizeof(char));
        memcpy(num, start, (cur - start) * sizeof(char));

        num[cur - start] = '\0';

        stack[stackHead++] = atoi(num);
        free(num);
      } else {
        printf("Unkown element '%c'.\n", *cur);
        exit(-1);
      }
    }
  }

  return stack[0];
}
