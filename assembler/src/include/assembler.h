#ifndef ASSEMBLER_H_
#define ASSEMBLER_H_

#include <stdint.h>

#include "scanner.h"
#include "table.h"

#define BYTE_MAX 65536
#define MAX_IDENTIFIER_LEN 64

typedef uint8_t byte;

enum OPCODES {
  OP_ADD = 0x01,
  OP_SUB,
  OP_LD,
  OP_MV,
  OP_JMP,
  OP_ADDR,
  OP_SUBR,
  OP_XOR,
  OP_AND,
  OP_OR,
  OP_NAND,
  OP_NOT,
  OP_SHFT,
  OP_ST,
  OP_RET,
  OP_CMP,
  OP_JE,
  OP_JNE,
  OP_JG,
  OP_JL,
  OP_PUSH,
  OP_POP,
  OP_HALT,
  EMPTY,
  RESOLVE
};

struct Assembler {
  Scanner *scanner;
  char *src;

  Token prev;
  Token next;

  byte output[BYTE_MAX];
  uint16_t byteHead;

  Table symbolTable;
};

typedef struct Assembler Assembler;

struct AssembledByte {
  byte b;
  char identifier[MAX_IDENTIFIER_LEN];
};

typedef struct AssembledByte AssembledByte;

// Initialize assembler
void initAssembler(Assembler *assembler, char *src);

// Assemble the file
byte *assemble(Assembler *assembler);

// Free assembler
void freeAssembler(Assembler *assembler);

#endif
