#include "assembler.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define START_SIZE 32
#define WORD_SIZE 16

#define EMPTY_BYTE                                                             \
  (AssembledByte) { EMPTY, "" }

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
    uint8_t c = peek(assembler).type;
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

byte *assemble(Assembler *assembler) {
  Token tkn;

  char labelQueue[MAX_REFERENCE_AMOUNT][MAX_IDENTIFIER_LEN] = {{'\0'}};
  int queueHead = 0;

  // This is the first pass. When we encounter a reference to a label, we set
  // the next two bits to 0xFF so we dont screw up the amount of bytes since an
  // address is two bytes and add the label string to a queue to later be
  // resolved. When we encounter a label we add it to the symbol table.

  while ((tkn = advance(assembler)).type != TOKEN_END) {
    switch (tkn.type) {
    case TOKEN_ADD: {
      pushByte(assembler, OP_ADD);
      pushByte(assembler, consumeRegister(assembler));
      consume(assembler, TOKEN_COMMA, "Expected ','.");
      pushByte(assembler,
               consume(assembler, TOKEN_NUMBER, "Expected number.").val);
      break;
    }
    case TOKEN_SUB: {
      pushByte(assembler, OP_SUB);
      pushByte(assembler, consumeRegister(assembler));
      consume(assembler, TOKEN_COMMA, "Expected ','.");
      pushByte(assembler,
               consume(assembler, TOKEN_NUMBER, "Expected number.").val);
      break;
    }
    case TOKEN_LD: {
      pushByte(assembler, OP_LD);
      pushByte(assembler, consumeRegister(assembler));
      consume(assembler, TOKEN_COMMA, "Expected ','.");
      memcpy(labelQueue[queueHead++],
             (tkn = consume(assembler, TOKEN_IDENTIFIER, "Unexpected keyword."))
                 .start,
             tkn.len);
      pushByte(assembler, 0xFF);
      pushByte(assembler, 0xFF);
      break;
    }
    case TOKEN_MV: {
      pushByte(assembler, OP_MV);
      pushByte(assembler, consumeRegister(assembler));
      consume(assembler, TOKEN_COMMA, "Expected ','.");
      pushByte(assembler, consumeRegister(assembler));
      break;
    }
    case TOKEN_JMP: {
      pushByte(assembler, OP_JMP);
      memcpy(labelQueue[queueHead++],
             (tkn = consume(assembler, TOKEN_IDENTIFIER, "Unexpected keyword."))
                 .start,
             tkn.len);
      pushByte(assembler, 0xFF);
      pushByte(assembler, 0xFF);
      break;
    }
    case TOKEN_ADDR: {
      pushByte(assembler, OP_ADDR);
      pushByte(assembler, consumeRegister(assembler));
      consume(assembler, TOKEN_COMMA, "Expected ','.");
      pushByte(assembler, consumeRegister(assembler));
      break;
    }
    case TOKEN_SUBR: {
      pushByte(assembler, OP_SUBR);
      pushByte(assembler, consumeRegister(assembler));
      consume(assembler, TOKEN_COMMA, "Expected ','.");
      pushByte(assembler, consumeRegister(assembler));
      break;
    }
    case TOKEN_XOR: {
      pushByte(assembler, OP_XOR);
      pushByte(assembler, consumeRegister(assembler));
      consume(assembler, TOKEN_COMMA, "Expected ','.");
      pushByte(assembler, consumeRegister(assembler));
      break;
    }
    case TOKEN_AND: {
      pushByte(assembler, OP_AND);
      pushByte(assembler, consumeRegister(assembler));
      consume(assembler, TOKEN_COMMA, "Expected ','.");
      pushByte(assembler, consumeRegister(assembler));
      break;
    }
    case TOKEN_OR: {
      pushByte(assembler, OP_OR);
      pushByte(assembler, consumeRegister(assembler));
      consume(assembler, TOKEN_COMMA, "Expected ','.");
      pushByte(assembler, consumeRegister(assembler));
      break;
    }
    case TOKEN_NAND: {
      pushByte(assembler, OP_AND);
      pushByte(assembler, consumeRegister(assembler));
      consume(assembler, TOKEN_COMMA, "Expected ','.");
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
      consume(assembler, TOKEN_COMMA, "Expected ','.");
      pushByte(assembler, consumeRegister(assembler));
      break;
    }
    case TOKEN_ST: {
      pushByte(assembler, OP_ST);
      pushByte(assembler, consumeRegister(assembler));
      consume(assembler, TOKEN_COMMA, "Expected ','.");
      memcpy(labelQueue[queueHead++],
             (tkn = consume(assembler, TOKEN_IDENTIFIER, "Unexpected keyword."))
                 .start,
             tkn.len);
      pushByte(assembler, 0xFF);
      pushByte(assembler, 0xFF);
      break;
    }
    case TOKEN_RET: {
      pushByte(assembler, OP_RET);
      break;
    }
    case TOKEN_CMP: {
      pushByte(assembler, OP_CMP);
      pushByte(assembler, consumeRegister(assembler));
      consume(assembler, TOKEN_COMMA, "Expected ','.");
      pushByte(assembler, consumeRegister(assembler));
      break;
    }
    case TOKEN_JE: {
      pushByte(assembler, OP_JE);
      memcpy(labelQueue[queueHead++],
             (tkn = consume(assembler, TOKEN_IDENTIFIER, "Unexpected keyword."))
                 .start,
             tkn.len);
      pushByte(assembler, 0xFF);
      pushByte(assembler, 0xFF);
      break;
    }
    case TOKEN_JNE: {
      pushByte(assembler, OP_JNE);
      memcpy(labelQueue[queueHead++],
             (tkn = consume(assembler, TOKEN_IDENTIFIER, "Unexpected keyword."))
                 .start,
             tkn.len);
      pushByte(assembler, 0xFF);
      pushByte(assembler, 0xFF);
      break;
    }
    case TOKEN_JG: {
      pushByte(assembler, OP_JG);
      memcpy(labelQueue[queueHead++],
             (tkn = consume(assembler, TOKEN_IDENTIFIER, "Unexpected keyword."))
                 .start,
             tkn.len);
      pushByte(assembler, 0xFF);
      pushByte(assembler, 0xFF);
      break;
    }
    case TOKEN_JL: {
      pushByte(assembler, OP_JL);
      memcpy(labelQueue[queueHead++],
             (tkn = consume(assembler, TOKEN_IDENTIFIER, "Unexpected keyword."))
                 .start,
             tkn.len);
      pushByte(assembler, 0xFF);
      pushByte(assembler, 0xFF);
      break;
    }
    case TOKEN_PUSH: {
      pushByte(assembler, OP_PUSH);
      pushByte(assembler, consumeRegister(assembler));
      break;
    }
    case TOKEN_POP: {
      pushByte(assembler, OP_POP);
      pushByte(assembler, consumeRegister(assembler));
      break;
    }
    case TOKEN_HALT: {
      pushByte(assembler, OP_HALT);
      break;
    }
    case TOKEN_IDENTIFIER: {
      // We encounter the label and push it to the symbol stack
      char buf[MAX_IDENTIFIER_LEN] = {'\0'};
      memcpy(buf, tkn.start, tkn.len);
      addElement(&assembler->symbolTable, buf, assembler->byteHead);
      char test = *(assembler->prev.start);
      consume(assembler, TOKEN_COLON, "Expected ':'' after identifier.");
      break;
    }
    default:
      printf("Unkown token '%d'.\n", tkn.type);
      exit(-1);
    }
  }

  int popQueueHead = 0;

  for (int i = 0; i < assembler->byteHead; i++) {
    // Last token is always halt (which is 0x17) so we should never go out of
    // bounds.
    if (assembler->output[i] == 0xFF && assembler->output[i + 1] == 0xFF) {
      char *label = labelQueue[popQueueHead++];

      uint16_t address = getElement(&assembler->symbolTable, label);

      assembler->output[i] = address;
      assembler->output[i + 1] = address >> 8;
    }
  }

  return assembler->output;
}
