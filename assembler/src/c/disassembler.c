#include "disassembler.h"

#include <stdio.h>

const char *registerString[] = {
    [RG_SR] = "SR",   [RG_SP] = "SP",     [RG_PC] = "PC",   [RG_G0] = "GP 0",
    [RG_G1] = "GP 1", [RG_G2] = "GP 2",   [RG_G3] = "GP 3", [RG_G4] = "GP 4",
    [RG_G5] = "GP 5", [RG_G6] = "GP 6",   [RG_G7] = "GP 7", [RG_G8] = "GP 8",
    [RG_G9] = "GP 9", [RG_G10] = "GP 10",
};

const char *opCodeStrings[] = {
    [CODE_ADD] = "ADD",   [CODE_SUB] = "SUB",   [CODE_LD] = "LD",
    [CODE_MV] = "MV",     [CODE_JMP] = "JMP",   [CODE_ADDR] = "ADDR",
    [CODE_SUBR] = "SUBR", [CODE_XOR] = "XOR",   [CODE_AND] = "AND",
    [CODE_OR] = "OR",     [CODE_NAND] = "NAND", [CODE_NOT] = "NOT",
    [CODE_SHFT] = "SHFT", [CODE_ST] = "ST",     [CODE_RET] = "RET",
    [CODE_CMP] = "CMP",   [CODE_JE] = "JE",     [CODE_JNE] = "JNE",
    [CODE_JG] = "JG",     [CODE_JL] = "JL",     [CODE_PUSH] = "PUSH",
    [CODE_POP] = "POP",   [CODE_HALT] = "HALT",
};

static void printR(uint8_t r) {
  printf("Register '%s'", registerString[r]);
}

static void printA(uint16_t a) {
  printf("Address 0x%04X", a);
}

static void printRV(uint8_t r, int v) {
  printR(r);
  printf(", Value %d", v);
}

static void printRA(uint8_t r, uint16_t a) {
  printR(r);
  printf(", ");
  printA(a);
}

static void printRR(uint8_t r1, uint8_t r2) {
  printR(r1);
  printf(", ");
  printR(r2);
}

void disassemble(uint8_t *bytes, int size) {
  for (int i = 0; i < size; i++) {
    switch (bytes[i]) {
    case CODE_ADD:
    case CODE_SUB:
      printf("%s OP, ARGS [", opCodeStrings[bytes[i]]);
      printRV(bytes[i + 1], bytes[i + 2]);
      printf("]\n");
      i += 2;
      break;
    case CODE_MV:
    case CODE_ADDR:
    case CODE_SUBR:
    case CODE_XOR:
    case CODE_AND:
    case CODE_OR:
    case CODE_NAND:
    case CODE_SHFT:
    case CODE_CMP:
      printf("%s OP, ARGS [", opCodeStrings[bytes[i]]);
      printRR(bytes[++i], bytes[++i]);
      printf("]\n");
      break;
    case CODE_LD:
    case CODE_ST:
      printf("%s OP, ARGS [", opCodeStrings[bytes[i]]);
      printRA(bytes[i + 1], bytes[i + 2] + (bytes[i + 3] << 8));
      printf("]\n");
      i += 3;
      break;
    case CODE_NOT:
    case CODE_POP:
    case CODE_PUSH:
      printf("%s OP, ARGS [", opCodeStrings[bytes[i]]);
      printR(bytes[i + 1]);
      printf("]\n");
      i++;
      break;
    case CODE_JMP:
    case CODE_JE:
    case CODE_JNE:
    case CODE_JG:
    case CODE_JL:
      printf("%s OP, ARGS [", opCodeStrings[bytes[i]]);
      printA(bytes[i + 1] + (bytes[i + 2] << 8));
      printf("]\n");
      i += 2;
      break;
    case CODE_RET:
    case CODE_HALT:
      printf("OP %s\n", opCodeStrings[bytes[i]]);
      break;
    default:
      printf("Unkown op '%d'.\n", bytes[i]);
      break;
    }
  }
}
