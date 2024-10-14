#include "assembler.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define START_SIZE 32
#define WORD_SIZE 16

#define EMPTY_BYTE (AssembledByte){EMPTY, ""}

void initAssembler(Assembler *assembler, char *src) {
  assembler->scanner = malloc(sizeof(Scanner));

  initScanner(assembler->scanner, src);

  assembler->next = scanToken(assembler->scanner);

  assembler->byteHead = 0;

  initTable(&assembler->symbolTable);
}

static Token advance(Assembler *assembler) {
  assembler->prev = assembler->next;
  assembler->next = scanToken(assembler->scanner);

  return assembler->prev;
}

static Token peek(Assembler *assembler) { return assembler->next; }

static Token consume(Assembler *assembler, TokenType cmp, char *msg) {
  if (cmp != peek(assembler).type) {
    char buffer[WORD_SIZE];
    printf("[Line %d] %s\n", peek(assembler).line, msg);
    exit(-1);
  }

  return advance(assembler);
}

static uint8_t consumeRegister(Assembler *assembler) {
  if (peek(assembler).type >= TOKEN_SR && peek(assembler).type <= TOKEN_G10) {
    return advance(assembler).type - 22;
  }

  printf("[Line %d] Expected register.\n", peek(assembler).line);
  exit(-1);

  return 0;
}

static void pushByte(Assembler *assembler, byte b) {
  if (assembler->byteHead == BYTE_MAX - 1) {
    printf("[Line %d] Exceeded max instruction amount.\n",
           peek(assembler).line);
  }
  assembler->output[assembler->byteHead++] = b;
}

static byte enumToOpcode(TokenType reg) { return reg - 22; }

static AssembledByte getIdentifier(Token token) {
  AssembledByte b;
  memset(b.identifier, '\0', MAX_IDENTIFIER_LEN);
  b.b = RESOLVE;
  memcpy(b.identifier, token.start, token.len);
  return b;
}

byte *assemble(Assembler *assembler) {
  Token tkn;

  AssembledByte bytes[BYTE_MAX] = {EMPTY_BYTE};
  int byteHead = 0;

  while ((tkn = advance(assembler)).type != TOKEN_END) {
    switch (tkn.type) {
    case TOKEN_ADD: {
      bytes[byteHead++] = (AssembledByte){OP_ADD, ""};
      bytes[byteHead++] = (AssembledByte){consumeRegister(assembler), ""};
      bytes[byteHead++] = (AssembledByte){
          consume(assembler, TOKEN_NUMBER, "Expected number.").val, ""};
      break;
    }
    case TOKEN_SUB: {
      bytes[byteHead++] = (AssembledByte){OP_SUB, ""};
      bytes[byteHead++] = (AssembledByte){consumeRegister(assembler), ""};
      bytes[byteHead++] = (AssembledByte){
          consume(assembler, TOKEN_NUMBER, "Expected number.").val, ""};
      break;
    }
    case TOKEN_LD: {
      bytes[byteHead++] = (AssembledByte){OP_LD, ""};
      bytes[byteHead++] = (AssembledByte){consumeRegister(assembler), ""};
      bytes[byteHead++] = getIdentifier(
          consume(assembler, TOKEN_IDENTIFIER, "Expected identifier."));
      byteHead++;
      break;
    }
    case TOKEN_MV: {
      bytes[byteHead++] = (AssembledByte){OP_MV, ""};
      bytes[byteHead++] = (AssembledByte){consumeRegister(assembler), ""};
      bytes[byteHead++] = (AssembledByte){consumeRegister(assembler), ""};
      break;
    }
    case TOKEN_JMP: {
      bytes[byteHead++] = (AssembledByte){OP_JMP, ""};
      bytes[byteHead++] = getIdentifier(
          consume(assembler, TOKEN_IDENTIFIER, "Expected identifier."));
      byteHead++;
      break;
    }
    case TOKEN_ADDR: {
      bytes[byteHead++] = (AssembledByte){OP_ADDR, ""};
      bytes[byteHead++] = (AssembledByte){consumeRegister(assembler), ""};
      bytes[byteHead++] = (AssembledByte){consumeRegister(assembler), ""};
      break;
    }
    case TOKEN_SUBR: {
      bytes[byteHead++] = (AssembledByte){OP_SUBR, ""};
      bytes[byteHead++] = (AssembledByte){consumeRegister(assembler), ""};
      bytes[byteHead++] = (AssembledByte){consumeRegister(assembler), ""};
      break;
    }
    case TOKEN_XOR: {
      bytes[byteHead++] = (AssembledByte){OP_XOR, ""};
      bytes[byteHead++] = (AssembledByte){consumeRegister(assembler), ""};
      bytes[byteHead++] = (AssembledByte){consumeRegister(assembler), ""};
      break;
    }
    case TOKEN_AND: {
      bytes[byteHead++] = (AssembledByte){OP_AND, ""};
      bytes[byteHead++] = (AssembledByte){consumeRegister(assembler), ""};
      bytes[byteHead++] = (AssembledByte){consumeRegister(assembler), ""};
      break;
    }
    case TOKEN_OR: {
      bytes[byteHead++] = (AssembledByte){OP_OR, ""};
      bytes[byteHead++] = (AssembledByte){consumeRegister(assembler), ""};
      bytes[byteHead++] = (AssembledByte){consumeRegister(assembler), ""};
      break;
    }
    case TOKEN_NAND: {
      bytes[byteHead++] = (AssembledByte){OP_NAND, ""};
      bytes[byteHead++] = (AssembledByte){consumeRegister(assembler), ""};
      bytes[byteHead++] = (AssembledByte){consumeRegister(assembler), ""};
      break;
    }
    case TOKEN_NOT: {
      bytes[byteHead++] = (AssembledByte){OP_NOT, ""};
      bytes[byteHead++] = (AssembledByte){consumeRegister(assembler), ""};
      break;
    }
    case TOKEN_SHFT: {
      bytes[byteHead++] = (AssembledByte){OP_SHFT, ""};
      bytes[byteHead++] = (AssembledByte){consumeRegister(assembler), ""};
      bytes[byteHead++] = (AssembledByte){consumeRegister(assembler), ""};
      break;
    }
    case TOKEN_ST: {
      bytes[byteHead++] = (AssembledByte){OP_ST, ""};
      bytes[byteHead++] = (AssembledByte){consumeRegister(assembler), ""};
      bytes[byteHead++] = getIdentifier(
          consume(assembler, TOKEN_IDENTIFIER, "Expected identifier."));
      byteHead++;
      break;
    }
    case TOKEN_RET: {
      bytes[byteHead++] = (AssembledByte){OP_RET, ""};
      break;
    }
    case TOKEN_CMP: {
      bytes[byteHead++] = (AssembledByte){OP_AND, ""};
      bytes[byteHead++] = (AssembledByte){consumeRegister(assembler), ""};
      bytes[byteHead++] = (AssembledByte){consumeRegister(assembler), ""};
      break;
    }
    case TOKEN_JE: {
      bytes[byteHead++] = (AssembledByte){OP_JE, ""};
      bytes[byteHead++] = getIdentifier(
          consume(assembler, TOKEN_IDENTIFIER, "Expected identifier."));
      byteHead++;
      break;
    }
    case TOKEN_JNE: {
      bytes[byteHead++] = (AssembledByte){OP_JNE, ""};
      bytes[byteHead++] = getIdentifier(
          consume(assembler, TOKEN_IDENTIFIER, "Expected identifier."));
      byteHead++;
      break;
    }
    case TOKEN_JG: {
      bytes[byteHead++] = (AssembledByte){OP_JG, ""};
      bytes[byteHead++] = getIdentifier(
          consume(assembler, TOKEN_IDENTIFIER, "Expected identifier."));
      byteHead++;
      break;
    }
    case TOKEN_JL: {
      bytes[byteHead++] = (AssembledByte){OP_JL, ""};
      bytes[byteHead++] = getIdentifier(
          consume(assembler, TOKEN_IDENTIFIER, "Expected identifier."));
      byteHead++;
      break;
    }
    case TOKEN_PUSH: {
      bytes[byteHead++] = (AssembledByte){OP_PUSH, ""};
      bytes[byteHead++] = (AssembledByte){consumeRegister(assembler), ""};
      break;
    }
    case TOKEN_POP: {
      bytes[byteHead++] = (AssembledByte){OP_PUSH, ""};
      bytes[byteHead++] = (AssembledByte){consumeRegister(assembler), ""};
      break;
    }
    case TOKEN_HALT: {
      bytes[byteHead++] = (AssembledByte){OP_HALT, ""};
      break;
    }
    case TOKEN_IDENTIFIER: {
        char buf[MAX_IDENTIFIER_LEN] = {'\0'};
        memcpy(buf, tkn.start, tkn.len);
        consume(assembler, TOKEN_COLON, "Expected colon after identifier");
        addElement(&assembler->symbolTable, buf, byteHead);
    }
    default:
      printf("Unkown token '%d'.\n", tkn.type);
      exit(-1);
    }
  }

  return assembler->output;
}
