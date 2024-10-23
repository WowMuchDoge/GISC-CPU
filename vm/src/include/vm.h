#ifndef VM_H_
#define VM_H_

#include <stdbool.h>
#include <stdint.h>

#define DEBUG

#define MEMORY_SIZE 65536
#define BUFFER_MAX 4096
#define INPUT_BUFFER 0x9001
#define PRINT_BUFFER 0xA001
#define FILENAME_BUFFER 0xB001

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
  OP_CALL,
  OP_HALT
};

enum {
  R_SR = 0x01,
  R_SP,
  R_PC,
  R_SC,
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

enum SysCalls {
  CALL_PRINT = 0x01,
  CALL_PCLEAR,
  CALL_FREAD,
  CALL_FWRITE,
  CALL_CREAD,
  CALL_ICLEAR
};

struct VM {
  // Each bit different kinds of compare as well as sign and carry
  uint8_t _statusRegister;
  // Points 2 byte lower than the last element pushed to the stack
  uint8_t _stackPointer;
  // Holds the syscall to the executed
  uint8_t _syscall;
  // Points to next instruction to be executed
  uint16_t _programCounter;

  // General purpose registers
  uint8_t _GP[11];

  // Memory space of the CPU. Technically not true memory because it is lumped
  // together inside of the CPU making it an MCU but I don't care.

  // 0x0000 -> 0x7FFF intended for ROM instructions, begins at address 0x0000.
  // 0x8000 -> 0xFFFF intended for manipulating memory inside the CPU, including
  // 0xF001 -> 0xF0FF reserved for the stack,
  // 0x9000 -> 0xA000 reserved for input buffering and
  // 0xA001 -> 0xB000 reserved for the print buffer
  // 0xB001 -> 0xB0FF reserved for the filename buffer
  uint8_t _memory[MEMORY_SIZE];
};

typedef struct VM VM;

// Initializes CPU, instructions array must be of length MEMORY_SIZE
void initCpu(VM *vm, uint8_t *instructions);

// Runs the CPU with its instructions loaded into memory
void run(VM *vm);

#endif
