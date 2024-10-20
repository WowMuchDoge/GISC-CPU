#ifndef DISASSEMBLER_H_
#define DISASSEMBLER_H_

#include <stdint.h>

enum Ops {
  CODE_ADD = 0x01,
  CODE_SUB,
  CODE_LD,
  CODE_MV,
  CODE_JMP,
  CODE_ADDR,
  CODE_SUBR,
  CODE_XOR,
  CODE_AND,
  CODE_OR,
  CODE_NAND,
  CODE_NOT,
  CODE_SHFT,
  CODE_ST,
  CODE_RET,
  CODE_CMP,
  CODE_JE,
  CODE_JNE,
  CODE_JG,
  CODE_JL,
  CODE_PUSH,
  CODE_POP,
  CODE_CALL,
  CODE_HALT,
};

enum Registers {
  RG_SR = 0x01,
  RG_SP,
  RG_PC,
  RG_G0,
  RG_G1,
  RG_G2,
  RG_G3,
  RG_G4,
  RG_G5,
  RG_G6,
  RG_G7,
  RG_G8,
  RG_G9,
  RG_G10
};

void disassemble(uint8_t *bytes, int size);

#endif
