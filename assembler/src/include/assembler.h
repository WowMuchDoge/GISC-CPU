#ifndef ASSEMBLER_H_
#define ASSEMBLER_H_

#include <stdint.h>

#include "scanner.h"
#include "table.h"

#define BYTE_MAX 0xFFFF + 1
#define MAX_IDENTIFIER_LEN 64
#define MAX_REFERENCE_AMOUNT 512

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
  OP_CALL,
  OP_HALT,
};

struct Assembler {
  Scanner scanner;
  char *src;

  Token prev;
  Token next;

  byte output[BYTE_MAX];
  uint16_t byteHead;

  uint16_t startHead;
  uint16_t stringHead;
  uint16_t orgHead;

  Table symbolTable;

  char *filename;
};

typedef struct Assembler Assembler;

// Initialize assembler
void initAssembler(Assembler *assembler, char *src, char *filename);

// Assemble the file
byte *assemble(Assembler *assembler);

// Free assembler
void freeAssembler(Assembler *assembler);

#endif
