#include "assembler.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define START_SIZE 32

void initAssembler(Assembler *assembler, char *src) {
  assembler->scanner = malloc(sizeof(Scanner));

  initScanner(assembler->scanner, src);

  assembler->output = malloc(START_SIZE * sizeof(byte));

  assembler->next = scanToken(assembler->scanner);

  assembler->byteHead = 0;
}

static Token advance(Assembler *assembler) {
  assembler->prev = assembler->next;
  assembler->next = scanToken(assembler->scanner);

  return assembler->prev;
}

static Token peek(Assembler *assembler) { return assembler->next; }

static Token consume(Assembler *assembler, TokenType cmp, char *msg) {
  if (cmp != peek(assembler).type) {
    printf("[Line %d] %s\n", peek(assembler).line, msg);
    exit(-1);
  }

  return advance(assembler);
}

static uint8_t consumeRegister(Assembler *assembler) {
  if (peek(assembler).type >= TOKEN_SR && peek(assembler).type <= TOKEN_G10) {
    return advance(assembler).type - 22;
  }

  printf("Expected register.\n");
  exit(-1);

  return 0;
}

static void pushByte(Assembler *assembler, byte b) {
  if (assembler->byteHead == BYTE_MAX - 1) {
    printf("[Line %d] Exceeded max instruction amount.\n",
           peek(assembler).line);
  }
  assembler->output[assembler->byteHead] = b;
}

static byte enumToOpcode(TokenType reg) { return reg - 22; }

byte *assemble(Assembler *assembler) {
  Token tkn;

  while ((tkn = scanToken(assembler->scanner)).type != TOKEN_END) {
    switch (tkn.type) {
    case TOKEN_ADD: {
      pushByte(assembler, OP_ADD);
      pushByte(assembler, consumeRegister(assembler));
      pushByte(assembler,
               consume(assembler, TOKEN_NUMBER, "Expected a number.").val);
      break;
    }
    case TOKEN_SUB: {
      pushByte(assembler, OP_SUB);
      pushByte(assembler, consumeRegister(assembler));
      pushByte(assembler,
               consume(assembler, TOKEN_NUMBER, "Expected a number.").val);
      break;
    }
    case TOKEN_LD: {
      pushByte(assembler, OP_LD);
      pushByte(assembler, consumeRegister(assembler));
      uint16_t address =
          consume(assembler, TOKEN_NUMBER, "Expected number.").val;
      pushByte(assembler, address);
      pushByte(assembler, address >> 8);
      break;
    }
    case TOKEN_MV: {
      pushByte(assembler, OP_MV);
      pushByte(assembler, consumeRegister(assembler));
      pushByte(assembler, consumeRegister(assembler));
      break;
    }
    case TOKEN_JMP: {
      pushByte(assembler, OP_JMP);
      uint16_t address =
          consume(assembler, TOKEN_NUMBER, "Expected number.").val;
      pushByte(assembler, address);
      pushByte(assembler, address >> 8);
      break;
    }
    case TOKEN_ADDR: {
      pushByte(assembler, OP_ADDR);
      pushByte(assembler, consumeRegister(assembler));
      pushByte(assembler, consumeRegister(assembler));
      break;
    }
    case TOKEN_SUBR: {

      pushByte(assembler, OP_SUBR);
      pushByte(assembler, consumeRegister(assembler));
      pushByte(assembler, consumeRegister(assembler));
      break;
    }
    case TOKEN_XOR: {
      pushByte(assembler, OP_XOR);
      pushByte(assembler, consumeRegister(assembler));
      pushByte(assembler, consumeRegister(assembler));
      break;
    }
    case TOKEN_AND: {
      pushByte(assembler, OP_AND);
      pushByte(assembler, consumeRegister(assembler));
      pushByte(assembler, consumeRegister(assembler));
      break;
    }
    case TOKEN_OR: {
      pushByte(assembler, OP_OR);
      pushByte(assembler, consumeRegister(assembler));
      pushByte(assembler, consumeRegister(assembler));
      break;
    }
    case TOKEN_NAND: {
      pushByte(assembler, OP_NAND);
      pushByte(assembler, consumeRegister(assembler));
      pushByte(assembler, consumeRegister(assembler));
      break;
    }
    case TOKEN_NOT: {
      pushByte(assembler, OP_NOT);
      pushByte(assembler, consumeRegister(assembler));
      break;
    }
    case TOKEN_SHFT: {
      pushByte(assembler, OP_SHFT);
      pushByte(assembler, consumeRegister(assembler));
      pushByte(assembler, consumeRegister(assembler));
      break;
    }
    case TOKEN_ST: {
      pushByte(assembler, OP_ST);
      pushByte(assembler, consumeRegister(assembler));
      uint16_t address =
          consume(assembler, TOKEN_NUMBER, "Expected number.").val;
      pushByte(assembler, address);
      pushByte(assembler, address >> 8);
      break;
    }
    case TOKEN_RET: {
      pushByte(assembler, OP_RET);
      break;
    }
    case TOKEN_CMP: {
      pushByte(assembler, OP_CMP);
      pushByte(assembler, consumeRegister(assembler));
      pushByte(assembler, consumeRegister(assembler));
      break;
    }
    case TOKEN_JE: {
      pushByte(assembler, OP_JE);
      uint16_t address =
          consume(assembler, TOKEN_NUMBER, "Expected number.").val;
      pushByte(assembler, address);
      pushByte(assembler, address >> 8);
      break;
    }
    case TOKEN_JNE: {
      pushByte(assembler, OP_JNE);
      uint16_t address =
          consume(assembler, TOKEN_NUMBER, "Expected number.").val;
      pushByte(assembler, address);
      pushByte(assembler, address >> 8);
      break;
    }
    case TOKEN_JG: {
      pushByte(assembler, OP_JG);
      uint16_t address =
          consume(assembler, TOKEN_NUMBER, "Expected number.").val;
      pushByte(assembler, address);
      pushByte(assembler, address >> 8);
      break;
    }
    case TOKEN_JL: {
      pushByte(assembler, OP_JL);
      uint16_t address =
          consume(assembler, TOKEN_NUMBER, "Expected number.").val;
      pushByte(assembler, address);
      pushByte(assembler, address >> 8);
      break;
    }
    case TOKEN_PUSH: {
      pushByte(assembler, OP_PUSH);
      pushByte(assembler, consumeRegister(assembler));
      break;
    }
    case TOKEN_POP: {
      pushByte(assembler, OP_PUSH);
      pushByte(assembler, consumeRegister(assembler));
      break;
    }
    case TOKEN_HALT: {
      pushByte(assembler, OP_HALT);
      break;
    }
    default:
      printf("Unkown token '%d'.\n", tkn.type);
      exit(-1);
    }
  }

  return assembler->output;
}
