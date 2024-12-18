#include "assembler.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"

#define START_SIZE 32
#define WORD_SIZE 16

#define MAX_ROM 0x7FFF;
#define STRING_BUFFER_LOCATION 0x9001

#define EMPTY_TOKEN                                                            \
  (Token) { 0, NULL, 0, 0, 0 }

#define GENERATE_RV(pushByteMethod, name)                                      \
  static void name(Assembler *assembler, uint8_t op) {                         \
    pushByteMethod(assembler, op);                                             \
    pushByteMethod(assembler, consumeRegister(assembler));                     \
    consume(assembler, TOKEN_COMMA, "Expected ','.");                          \
    pushByteMethod(assembler,                                                  \
                   consume(assembler, TOKEN_NUMBER, "Expected number.").val);  \
  }

#define GENERATE_RA(pushByteMethod, pushTwoBytesMethod, name)                  \
  static void name(Assembler *assembler,                                       \
                   char labelQueue[MAX_REFERENCE_AMOUNT][MAX_IDENTIFIER_LEN],  \
                   int *queueHead, Token tkn, uint8_t op) {                    \
    pushByteMethod(assembler, op);                                             \
    pushByteMethod(assembler, consumeRegister(assembler));                     \
    consume(assembler, TOKEN_COMMA, "Expected ','.");                          \
    memcpy(labelQueue[(*queueHead)++],                                         \
           (tkn = consume(assembler, TOKEN_IDENTIFIER, "Unexpected keyword.")) \
               .start,                                                         \
           tkn.len);                                                           \
    pushTwoBytesMethod(assembler, 0xFFFF);                                     \
  }

#define GENERATE_R(pushByteMethod, name)                                       \
  static void name(Assembler *assembler, uint8_t op) {                         \
    pushByteMethod(assembler, op);                                             \
    pushByteMethod(assembler, consumeRegister(assembler));                     \
  }

#define GENERATE_RR(pushByteMethod, name)                                      \
  static void name(Assembler *assembler, uint8_t op) {                         \
    pushByteMethod(assembler, op);                                             \
    pushByteMethod(assembler, consumeRegister(assembler));                     \
    consume(assembler, TOKEN_COMMA, "Expected comma between registers.");      \
    pushByteMethod(assembler, consumeRegister(assembler));                     \
  }

#define GENERATE_A(pushByteMethod, pushTwoBytesMethod, name)                   \
  static void name(Assembler *assembler,                                       \
                   char labelQueue[MAX_REFERENCE_AMOUNT][MAX_IDENTIFIER_LEN],  \
                   int *queueHead, Token tkn, uint8_t op) {                    \
    pushByteMethod(assembler, op);                                             \
    tkn = consume(assembler, TOKEN_IDENTIFIER, "Unexpected keyword.\n");       \
    memcpy(labelQueue[(*queueHead)++], tkn.start, tkn.len);                    \
    pushTwoBytesMethod(assembler, 0xFFFF);                                     \
  }

#define EMPTY_BYTE                                                             \
  (AssembledByte) { EMPTY, "" }

void initAssembler(Assembler *assembler, char *src, char *filename) {
  initScanner(&assembler->scanner, src);

  assembler->next = scanToken(&assembler->scanner);

  assembler->byteHead = 0;
  assembler->startHead = 0;
  assembler->stringHead = STRING_BUFFER_LOCATION;
  assembler->filename = filename;

  memset(assembler->output, '\0', BYTE_MAX);

  initTable(&assembler->symbolTable);
}

static Token advance(Assembler *assembler) {
  assembler->prev = assembler->next;
  assembler->next = scanToken(&assembler->scanner);

  return assembler->prev;
}

static Token peek(Assembler *assembler) { return assembler->next; }

static Token consume(Assembler *assembler, TokenType cmp, char *msg) {
  if (cmp != peek(assembler).type) {
    uint8_t c = peek(assembler).type;
    printError(peek(assembler).line, peek(assembler).lineStart, peek(assembler).start, peek(assembler).len, "Assembler", msg, assembler->filename);
    exit(-1);
  }

  return advance(assembler);
}

static uint8_t consumeRegister(Assembler *assembler) {
  if (peek(assembler).type >= TOKEN_SR && peek(assembler).type <= TOKEN_G10) {
    return (advance(assembler).type - TOKEN_SR) + 1;
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

static void pushTwoBytes(Assembler *assembler, uint16_t bytes) {
  pushByte(assembler, bytes);
  pushByte(assembler, bytes >> 8);
}

// When writing to the ROM, we are only given half of the space, so to load ops
// to an address above the max rom address we must instead utilize the st
// instruction to put the instructions in RAM

static void pushByteToRam(Assembler *assembler, byte b) {
  uint16_t temp = assembler->byteHead;
  assembler->byteHead = assembler->startHead;

  pushByte(assembler, OP_ADD);
  pushByte(assembler, TOKEN_G0 - 22); // 0x04 is the enumerated register
  pushByte(assembler, b);

  pushByte(assembler, OP_ST);
  pushByte(assembler, TOKEN_G0 - 22);
  pushTwoBytes(assembler, assembler->orgHead++);

  pushByte(assembler, OP_SUBR);
  pushByte(assembler, TOKEN_G0 - 22);
  pushByte(assembler, TOKEN_G0 - 22);

  assembler->startHead = assembler->byteHead;
  assembler->byteHead = temp;
}

static void pushTwoBytesToRam(Assembler *assembler, uint16_t b) {
  uint16_t temp = assembler->byteHead;
  assembler->byteHead = assembler->startHead;

  pushByteToRam(assembler, b);
  pushByteToRam(assembler, b >> 8);

  assembler->startHead = assembler->byteHead;
  assembler->byteHead = temp;
}

GENERATE_RV(pushByte, pushRegisterValue)
GENERATE_RV(pushByteToRam, pushRegisterValueToRam)

GENERATE_RA(pushByte, pushTwoBytes, pushRegisterAddress)
GENERATE_RA(pushByteToRam, pushTwoBytesToRam, pushRegisterAddressToRam)

GENERATE_R(pushByte, pushRegister)
GENERATE_R(pushByteToRam, pushRegisterToRam)

GENERATE_RR(pushByte, pushRegisterRegister)
GENERATE_RR(pushByteToRam, pushRegisterRegisterToRam)

GENERATE_A(pushByte, pushTwoBytes, pushAddress)
GENERATE_A(pushByteToRam, pushTwoBytesToRam, pushAddressToRam)

static void
pushInstruction(Assembler *assembler,
                char labelQueue[MAX_REFERENCE_AMOUNT][MAX_IDENTIFIER_LEN],
                int *queueHead) {
  Token tkn = advance(assembler);

  switch (tkn.type) {
  case TOKEN_ADD: {
    if (assembler->byteHead >= 0x8000) {
      pushRegisterValueToRam(assembler, OP_ADD);
    } else {
      pushRegisterValue(assembler, OP_ADD);
    }
    break;
  }
  case TOKEN_SUB: {
    if (assembler->byteHead >= 0x8000) {
      pushRegisterValueToRam(assembler, OP_SUB);
    } else {
      pushRegisterValue(assembler, OP_SUB);
    }
    break;
  }
  case TOKEN_LD: {
    if (assembler->byteHead >= 0x8000) {
      pushRegisterAddressToRam(assembler, labelQueue, queueHead, tkn, OP_LD);
    } else {
      pushRegisterAddress(assembler, labelQueue, queueHead, tkn, OP_LD);
    }
    break;
  }
  case TOKEN_MV: {
    if (assembler->byteHead >= 0x8000) {
      pushRegisterRegisterToRam(assembler, OP_MV);
    } else {
      pushRegisterRegister(assembler, OP_MV);
    }
    break;
  }
  case TOKEN_JMP: {
    if (assembler->byteHead >= 0x8000) {
      pushAddressToRam(assembler, labelQueue, queueHead, tkn, OP_JMP);
    } else {
      pushAddress(assembler, labelQueue, queueHead, tkn, OP_JMP);
    }
    break;
  }
  case TOKEN_ADDR: {
    if (assembler->byteHead >= 0x8000) {
      pushRegisterRegisterToRam(assembler, OP_ADDR);
    } else {
      pushRegisterRegister(assembler, OP_ADDR);
    }
    break;
  }
  case TOKEN_SUBR: {
    if (assembler->byteHead >= 0x8000) {
      pushRegisterRegisterToRam(assembler, OP_SUBR);
    } else {
      pushRegisterValueToRam(assembler, OP_ADD);
    }
    break;
  }
  case TOKEN_XOR: {
    if (assembler->byteHead >= 0x8000) {
      pushRegisterRegisterToRam(assembler, OP_XOR);
    } else {
      pushRegisterValueToRam(assembler, OP_ADD);
    }
    break;
  }
  case TOKEN_AND: {
    if (assembler->byteHead >= 0x8000) {
      pushRegisterRegisterToRam(assembler, OP_AND);
    } else {
      pushRegisterValueToRam(assembler, OP_ADD);
    }
    break;
  }
  case TOKEN_OR: {
    if (assembler->byteHead >= 0x8000) {
      pushRegisterRegisterToRam(assembler, OP_OR);
    } else {
      pushRegisterRegister(assembler, OP_OR);
    }
    break;
  }
  case TOKEN_NAND: {
    if (assembler->byteHead >= 0x8000) {
      pushRegisterRegisterToRam(assembler, OP_NAND);
    } else {
      pushRegisterRegister(assembler, OP_NAND);
    }
    break;
  }
  case TOKEN_NOT: {
    if (assembler->byteHead >= 0x8000) {
      pushRegisterToRam(assembler, OP_NOT);
    } else {
      pushRegister(assembler, OP_NOT);
    }
    break;
  }
  case TOKEN_SHFT: {
    if (assembler->byteHead >= 0x8000) {
      pushRegisterRegisterToRam(assembler, OP_NAND);
    } else {
      pushRegisterRegister(assembler, OP_NAND);
    }
    break;
  }
  case TOKEN_ST: {
    if (assembler->byteHead >= 0x8000) {
      pushRegisterAddressToRam(assembler, labelQueue, queueHead, tkn, OP_ST);
    } else {
      pushRegisterAddress(assembler, labelQueue, queueHead, tkn, OP_ST);
    }
    break;
  }
  case TOKEN_RET: {
    if (assembler->byteHead >= 0x8000) {
      pushByteToRam(assembler, OP_RET);
    } else {
      pushByte(assembler, OP_RET);
    }
    break;
  }
  case TOKEN_CMP: {
    if (assembler->byteHead >= 0x8000) {
      pushRegisterRegisterToRam(assembler, OP_CMP);
    } else {
      pushRegisterRegister(assembler, OP_CMP);
    }
    break;
  }
  case TOKEN_JE: {
    if (assembler->byteHead >= 0x8000) {
      pushAddress(assembler, labelQueue, queueHead, tkn, OP_JE);
    } else {
      pushAddress(assembler, labelQueue, queueHead, tkn, OP_JE);
    }
    break;
  }
  case TOKEN_JNE: {
    if (assembler->byteHead >= 0x8000) {
      pushRegisterValueToRam(assembler, OP_ADD);
    } else {
      pushRegisterValue(assembler, OP_ADD);
    }
    break;
  }
  case TOKEN_JG: {
    if (assembler->byteHead >= 0x8000) {
      pushAddressToRam(assembler, labelQueue, queueHead, tkn, TOKEN_JG);
    } else {
      pushAddress(assembler, labelQueue, queueHead, tkn, TOKEN_JG);
    }
    break;
  }
  case TOKEN_JL: {
    if (assembler->byteHead >= 0x8000) {
      pushAddressToRam(assembler, labelQueue, queueHead, tkn, OP_JL);
    } else {
      pushAddress(assembler, labelQueue, queueHead, tkn, OP_JL);
    }
    break;
  }
  case TOKEN_PUSH: {
    if (assembler->byteHead >= 0x8000) {
      pushRegisterToRam(assembler, OP_PUSH);
    } else {
      pushRegister(assembler, OP_PUSH);
    }
    break;
  }
  case TOKEN_POP: {
    if (assembler->byteHead >= 0x8000) {
      pushRegister(assembler, OP_POP);
    } else {
      pushRegister(assembler, OP_POP);
    }
    break;
  }
  case TOKEN_CALL: {
    if (assembler->byteHead >= 0x8000) {
      pushByteToRam(assembler, OP_CALL);
    } else {
      pushByte(assembler, OP_CALL);
    }
    break;
  }
  case TOKEN_HALT: {
    if (assembler->byteHead >= 0x8000) {
      pushByteToRam(assembler, OP_HALT);
    } else {
      pushByte(assembler, OP_HALT);
    }
    break;
  }
  case TOKEN_IDENTIFIER: {
    // We encounter the label and push it to the symbol stack
    char buf[MAX_IDENTIFIER_LEN] = {'\0'};
    memcpy(buf, tkn.start, tkn.len);
    addElement(&assembler->symbolTable, buf, assembler->byteHead);
    char test = *(assembler->prev.start);
    bool t = checkElement(&assembler->symbolTable, buf);
    consume(assembler, TOKEN_COLON, "Expected ':'' after identifier.");
    break;
  }
  case TOKEN_STRING: {
    for (int i = 0; i < tkn.len; i++) {
      if (assembler->byteHead >= 0x8000) {
        pushByteToRam(assembler, tkn.start[i]);
      } else {
        pushByte(assembler, tkn.start[i]);
      }
    }

    if (assembler->byteHead >= 0x8000) {
      pushByteToRam(assembler, '\0');
    } else {
      pushByte(assembler, '\0');
    }
    break;
  }
  default:
    printf("Unkown token '%d'.\n", tkn.type);
    exit(-1);
  }
}

static bool isDirective(TokenType t) {
  return t == TOKEN_DIR_START || t == TOKEN_DIR_STRING || t == TOKEN_DIR_ORG;
}

static Token consumeDirective(Assembler *assembler) {
  consume(assembler, TOKEN_DOT, "Expected '.' before directive.");
  if (!isDirective(peek(assembler).type)) {
    printf("Expected directive.");
    exit(-1);
  }

  return advance(assembler);
}

static bool atEndDirective(Assembler *assembler) {
  return peek(assembler).type == TOKEN_END || peek(assembler).type == TOKEN_DOT;
}

static void resetScanner(Assembler *assembler) {
  assembler->prev = EMPTY_TOKEN;
  assembler->next = scanToken(&assembler->scanner);
}

byte *assemble(Assembler *assembler) {
  Token tkn;

  char labelQueue[MAX_REFERENCE_AMOUNT][MAX_IDENTIFIER_LEN] = {{'\0'}};
  int queueHead = 0;

  uint16_t stringBufferHead = STRING_BUFFER_LOCATION;

  char *startTokens = NULL;
  char *startLine = NULL;
  int start = 0;

  while ((tkn = peek(assembler)).type != TOKEN_END) {
    TokenType directive = consumeDirective(assembler).type;
    char *temp = assembler->next.start;
    switch (directive) {
    case TOKEN_DIR_START: {
      startTokens = temp;
      startLine = assembler->scanner.lineStart;
      start = assembler->scanner.line;
      while (!atEndDirective(assembler))
        advance(assembler);
      break;
    }
    case TOKEN_DIR_ORG: {
      int address = consume(assembler, TOKEN_NUMBER,
                            "Expected address after '.org' directive.")
                        .val;

      if (address > UINT16_MAX) {
        printf("Address exceeds max address.");
      }

      assembler->byteHead = address;
      assembler->orgHead = address;

      while (!atEndDirective(assembler)) {
        pushInstruction(assembler, labelQueue, &queueHead);
      }
      break;
    }
    case TOKEN_DIR_STRING: {
      assembler->byteHead = assembler->stringHead;
      assembler->orgHead = assembler->stringHead;

      while (!atEndDirective(assembler)) {
        pushInstruction(assembler, labelQueue, &queueHead);
      }

      assembler->stringHead = assembler->orgHead;
      break;
    }
    default:
      // Unreachable...
      exit(-1);
    }
  }

  int popQueueHead = 0;

  if (startTokens) {
    assembler->scanner.cur = startTokens;
    assembler->scanner.lineStart = startLine;
    assembler->scanner.line = start;
    resetScanner(assembler);

    while (!atEndDirective(assembler)) {
      assembler->byteHead = assembler->startHead;
      assembler->orgHead = assembler->startHead;
      pushInstruction(assembler, labelQueue, &queueHead);
      assembler->startHead = assembler->byteHead;
    }
  }

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

void freeAssembler(Assembler *assembler) {
  freeTable(&assembler->symbolTable);
}