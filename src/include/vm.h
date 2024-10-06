#include <stdbool.h>
#include <stdint.h>

#define DEBUG

#define MEMORY_SIZE 65536

enum {
  OP_ADD = 1,
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
  OP_HALT
};

enum {
  R_SR = 0x01,
  R_SP,
  R_PC,
  R_G0,
  R_G1,
  R_G2,
  R_G3,
  R_G4,
  R_G5,
  R_G6,
  R_G7,
  R_G8,
  R_G9,
  R_G10
};

struct VM {
  // Each bit different kinds of compare as well as sign and carry
  uint8_t _statusRegister;
  // Points 2 byte lower than the last element pushed to the stack
  uint8_t _stackPointer;
  // Points to next instruction to be executed
  uint16_t _programCounter;

  // General purpose registers
  uint8_t _GP[11];

  // Memory space of the CPU. Technically not true memory because it is lumped
  // together inside of the CPU making it an MCU but I don't care.

  // 0x0000 -> 0x7FFF intended for ROM instructions, begins at address 0x0000.
  // 0x8000 -> 0xFFFF intended for manipulating memory inside the CPU, including
  // 0xF001 -> 0xF0FF reserved for the stack.
  uint8_t _memory[MEMORY_SIZE];
};

typedef struct VM VM;

// Initializes CPU, instructions array must be of length MEMORY_SIZE
void initCpu(VM *vm, uint8_t *instructions);

// Runs the CPU with its instructions loaded into memory
void run(VM *vm);
