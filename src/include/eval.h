#ifndef EVAL_H_
#define EVAL_H_

#include <stdint.h>

#define MAX_EXPR_LEN 128
#define MAX_STACK 128

struct Expr {
  char *cur;

  int outputHead;
  char output[MAX_EXPR_LEN];

  int opStackHead;
  char opStack[MAX_STACK];
};

typedef struct Expr Expr;

void initExpr(Expr *expr, char *src);

uint8_t evaluate(Expr *expr);

void freeExpr(Expr *expr);

#endif
