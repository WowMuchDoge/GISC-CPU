#include "assembler.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define START_SIZE 32
#define WORD_SIZE 16

#define MAX_ROM 0x7FFF;
#define STRING_BUFFER_LOCATION 0x9001

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
                   int queueHead, Token tkn, uint8_t op) {                     \
    pushByteMethod(assembler, op);                                             \
    pushByteMethod(assembler, consumeRegister(assembler));                     \
    consume(assembler, TOKEN_COMMA, "Expected ','.");                          \
    memcpy(labelQueue[queueHead++],                                            \
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
    pushByteMethod(assembler, consumeRegister(assembler));                     \
  }

#define GENERATE_A(pushByteMethod, pushTwoBytesMethod, name)                   \
  static void name(Assembler *assembler,                                       \
                   char labelQueue[MAX_REFERENCE_AMOUNT][MAX_IDENTIFIER_LEN],  \
                   int queueHead, Token tkn, uint8_t op) {                     \
    pushByteMethod(assembler, op);                                             \
    memcpy(labelQueue[queueHead++],                                            \
           (tkn = consume(assembler, TOKEN_IDENTIFIER, "Unexpected keyword.")) \
               .start,                                                         \
           tkn.len);                                                           \
    pushTwoBytesMethod(assembler, 0xFFFF);                                     \
  }

#define EMPTY_BYTE                                                             \
  (AssembledByte) { EMPTY, "" }

void initAssembler(Assembler *assembler, char *src) {
  assembler->scanner = malloc(sizeof(Scanner));

  initScanner(assembler->scanner, src);

  assembler->next = scanToken(assembler->scanner);

  assembler->byteHead = 0;
  assembler->startHead = 0;
  assembler->stringHead = STRING_BUFFER_LOCATION;

  memset(assembler->output, '\0', 0xFFFF);

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

static void pushTwoBytes(Assembler *assembler, uint16_t bytes) {
  pushByte(assembler, bytes);
  pushByte(assembler, bytes >> 8);
}

// When writing to the ROM, we are only given half of the space, so to load ops
// to an address above the max rom address we must instead utilize the st
// instruction to put the instructions in RAM

static void pushByteToRam(Assembler *assembler, byte b) {
  assembler->byteHead = assembler->startHead;

  pushByte(assembler, OP_ADD);
  pushByte(assembler, 0x04); // 0x04 is the enumerated register
  pushByte(assembler, b);

  pushByte(assembler, OP_ST);
  pushByte(assembler, 0x04);
  pushTwoBytes(assembler, assembler->orgHead++);

  assembler->startHead = assembler->byteHead;
}

static void pushTwoBytesToRam(Assembler *assembler, uint16_t b) {
  assembler->byteHead = assembler->startHead;

  pushByte(assembler, OP_ADD);
  pushByte(assembler, 0x04); // 0x04 is the enumerated register
  pushByte(assembler, b);

  pushByte(assembler, OP_ST);
  pushByte(assembler, 0x04);
  pushTwoBytes(assembler, assembler->orgHead++);

  pushByte(assembler, OP_ADD);
  pushByte(assembler, 0x05); // 0x05 is the enumerated register
  pushByte(assembler, b >> 8);

  pushByte(assembler, OP_ST);
  pushByte(assembler, 0x04);
  pushTwoBytes(assembler, assembler->orgHead++);

  assembler->startHead = assembler->byteHead;
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

static void pushInstruction(Assembler *assembler) {
  Token tkn = advance(assembler);

  char labelQueue[MAX_REFERENCE_AMOUNT][MAX_IDENTIFIER_LEN] = {{'\0'}};
  int queueHead = 0;
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
    consume(assembler, TOKEN_COLON, "Expected ':'' after identifier.");
    break;
  }
  case TOKEN_STRING: {
    for (int i = 0; i < tkn.len; i++) {
      if (assembler->byteHead >= 0x8000) {
        pushByteToRam(assembler, tkn.start[i]);
      } else{
        pushByte(assembler, tkn.start[i]);
      }
      printf("%c\n", tkn.start[i]);
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

byte *assemble(Assembler *assembler) {
  Token tkn;

  char labelQueue[MAX_REFERENCE_AMOUNT][MAX_IDENTIFIER_LEN] = {{'\0'}};
  int queueHead = 0;

  uint16_t stringBufferHead = STRING_BUFFER_LOCATION;

  Token *startTokens = malloc(START_SIZE * sizeof(Token));
  int tokenHead = 0;
  int size = START_SIZE;
  int tokenCount = 0;

  while ((tkn = peek(assembler)).type != TOKEN_END) {
    TokenType directive = consumeDirective(assembler).type;
    switch (directive) {
    case TOKEN_DIR_START: {
      while (!atEndDirective(assembler)) {
        if (tokenCount > size * 0.75) {
          size *= 2;
          startTokens = realloc(startTokens, size);
        }

        startTokens[tokenHead++] = advance(assembler);
      }
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
        pushInstruction(assembler);
      }
      break;
    }
    case TOKEN_DIR_STRING: {
      assembler->byteHead = assembler->stringHead;
      assembler->orgHead = assembler->stringHead;

      while (!atEndDirective(assembler)) {
        pushInstruction(assembler);
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

  free(startTokens);

  return assembler->output;
}
