#include "vm.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *RegisterNames[] = {
    [R_SR] = "SR", [R_SP] = "SP", [R_PC] = "PC", [R_G0] = "G0",   [R_G1] = "G1",
    [R_G2] = "G2", [R_G3] = "G3", [R_G4] = "G4", [R_G5] = "G5",   [R_G6] = "G6",
    [R_G7] = "G7", [R_G8] = "G8", [R_G9] = "G9", [R_G10] = "G10",
};

void initCpu(VM *vm, uint8_t *instructions) {
  vm->_statusRegister = 0;

  // Stack Pointer must be greater than zero to detect stack underflow
  vm->_stackPointer = 1;

  vm->_programCounter = 0;

  memset(vm->_GP, 0, 11);
  memcpy(vm->_memory, instructions, MEMORY_SIZE * sizeof(uint8_t));
}

static uint8_t cycle(VM *vm) {
  if (vm->_programCounter > 0x7FFE) {
    printf("Maximum Instruction Length Exceeded.\n");
    exit(-1);
  }

  uint8_t a = vm->_memory[vm->_programCounter];

  return vm->_memory[vm->_programCounter++];
}

static uint8_t *getRegister(VM *vm, uint8_t code) {
  switch (code) {
  case 0x01:
    return &(vm->_statusRegister);
  case 0x02:
    return &(vm->_stackPointer);
  case 0x03:
    printf("Cannot edit Program Counter.\n");
    exit(-1);
    return 0;
  default:
    if (code > 0x0E) {
      printf("Unkown Register '%d'.\n", code);
      exit(-1);
    }

    return &(vm->_GP[code - 4]);
  }
}

static void push(VM *vm, uint8_t val) {
  if (vm->_stackPointer == 255) {
    printf("Stack Overflow.\n");
    exit(-1);
  }

  vm->_memory[0xF000 + vm->_stackPointer++] = val;
}

static uint8_t pop(VM *vm) {
  if (vm->_stackPointer == 0) {
    printf("Stack Underflow.\n");
    exit(-1);
    return 0;
  }

  return vm->_memory[0xF000 + --vm->_stackPointer];
}

void run(VM *vm) {
  while (true) {
    uint8_t op = cycle(vm);

    switch (op) {
    case OP_ADD: {
      uint8_t *reg = getRegister(vm, cycle(vm));
      uint8_t operand2 = cycle(vm);

#ifdef DEBUG
      printf("ADD %d TO %s RESULT %d\n", operand2,
             RegisterNames[vm->_memory[vm->_programCounter - 2]],
             *reg + operand2);
#endif

      *reg += operand2;
      break;
    }
    case OP_SUB: {
      uint8_t *reg = getRegister(vm, cycle(vm));
      uint8_t operand2 = cycle(vm);

#ifdef DEBUG
      printf("SUB %d FROM %s RESULT %d\n", operand2,
             RegisterNames[vm->_memory[vm->_programCounter - 2]],
             *reg - operand2);
#endif

      *reg -= operand2;
      break;
    }
    case OP_LD: {
      uint8_t *reg = getRegister(vm, cycle(vm));

      uint16_t address = cycle(vm);
      address += cycle(vm) << 8;

#ifdef DEBUG
      printf("LOAD %d FROM 0x%02X TO %s\n", vm->_memory[address], address,
             RegisterNames[vm->_memory[vm->_programCounter - 3]]);
#endif

      *reg = vm->_memory[address];
      break;
    }
    case OP_MV: {
      uint8_t val = *getRegister(vm, cycle(vm));

      *getRegister(vm, cycle(vm)) = val;

#ifdef DEBUG
      printf("LOAD %d FROM %s TO %s\n", val,
             RegisterNames[vm->_programCounter - 2],
             RegisterNames[vm->_programCounter - 1]);
#endif
      break;
    }
    case OP_JMP: {
      uint16_t address = cycle(vm) + (cycle(vm) << 8);

      push(vm, vm->_programCounter);
      push(vm, (vm->_programCounter) >> 8);

#ifdef DEBUG
      printf("JMP FROM %d TO %d\n", vm->_programCounter, address);
#endif

      vm->_programCounter = address;
      break;
    }
    case OP_ADDR: {
      uint8_t *dest = getRegister(vm, cycle(vm));
      uint8_t *add = getRegister(vm, cycle(vm));

#ifdef DEBUG
      printf("ADD %d FROM %s TO %d FROM %s\n", *add,
             RegisterNames[vm->_memory[vm->_programCounter - 1]], *dest,
             RegisterNames[vm->_memory[vm->_programCounter - 2]]);
#endif

      *dest += *add;
      break;
    }
    case OP_SUBR: {
      uint8_t *dest = getRegister(vm, cycle(vm));
      uint8_t *sub = getRegister(vm, cycle(vm));

#ifdef DEBUG
      printf("SUB %d FROM %s TO %d FROM %s\n", *sub,
             RegisterNames[vm->_memory[vm->_programCounter - 1]], *dest,
             RegisterNames[vm->_memory[vm->_programCounter - 2]]);
#endif

      *dest -= *sub;
      break;
    }
    case OP_XOR: {
      uint8_t *dest = getRegister(vm, cycle(vm));
      uint8_t * xor = getRegister(vm, cycle(vm));

#ifdef DEBUG
      printf("XOR %d FROM %s TO %d FROM %s\n", *xor,
             RegisterNames[vm->_memory[vm->_programCounter - 1]], *dest,
             RegisterNames[vm->_memory[vm->_programCounter - 2]]);
#endif

      *dest ^= *xor;
      break;
    }
    case OP_AND: {
      uint8_t *dest = getRegister(vm, cycle(vm));
      uint8_t *and = getRegister(vm, cycle(vm));

#ifdef DEBUG
      printf("AND %d FROM %s TO %d FROM %s\n", *and,
             RegisterNames[vm->_memory[vm->_programCounter - 1]], *dest,
             RegisterNames[vm->_memory[vm->_programCounter - 2]]);
#endif

      *dest &= *and;
      break;
    }
    case OP_OR: {
      uint8_t *dest = getRegister(vm, cycle(vm));
      uint8_t * or = getRegister(vm, cycle(vm));

#ifdef DEBUG
      printf("OR %d FROM %s TO %d FROM %s\n", * or,
             RegisterNames[vm->_memory[vm->_programCounter - 1]], *dest,
             RegisterNames[vm->_memory[vm->_programCounter - 2]]);
#endif

      *dest |= * or ;
      break;
    }
    case OP_NAND: {
      uint8_t *dest = getRegister(vm, cycle(vm));
      uint8_t *nand = getRegister(vm, cycle(vm));

#ifdef DEBUG
      printf("NAND %d FROM %s TO %d FROM %s\n", *nand,
             RegisterNames[vm->_memory[vm->_programCounter - 1]], *dest,
             RegisterNames[vm->_memory[vm->_programCounter - 2]]);
#endif

      *dest = ~(*dest & *nand);
      break;
    }
    case OP_NOT: {
      uint8_t *dest = getRegister(vm, cycle(vm));

#ifdef DEBUG
      printf("NOT %d FROM %s\n", *dest,
             RegisterNames[vm->_memory[vm->_programCounter - 1]]);
#endif

      *dest = ~*dest;
      break;
    }
    case OP_SHFT: {
      // Heads up, shift amount is offset by 8. so 0 shifts 8 to the right and
      // 16 shifts 8 to the left
      uint8_t *dest = getRegister(vm, cycle(vm));
      uint8_t *amount = getRegister(vm, cycle(vm));

#ifdef DEBUG
      printf("SHIFT %d AT %s %d\n", *dest,
             RegisterNames[vm->_memory[vm->_programCounter]], *amount);
#endif

      *dest <<= (int)*amount - 8;
      break;
    }
    case OP_ST: {
      uint8_t *reg = getRegister(vm, cycle(vm));
      uint16_t address = cycle(vm) + (cycle(vm) << 8);

#ifdef DEBUG
      printf("STORE %d FROM %s TO ADDRESS 0x%04X\n", *reg,
             RegisterNames[vm->_memory[vm->_programCounter - 3]], address);
#endif

      vm->_memory[address] = *reg;
      break;
    }
    case OP_RET: {
      uint16_t address = (pop(vm) << 8) + pop(vm);

#ifdef DEBUG
      printf("MOV TO ADDRESS 0x%04X\n", address);
#endif

      vm->_programCounter = address;
      break;
    }
    case OP_CMP: {
      vm->_statusRegister = 0;

      // Status register is 8 bits. Bit 1 (LSB) Zero Flag, Bit 2 is Negative
      // Flag

      uint8_t operand1 = cycle(vm);
      uint8_t operand2 = cycle(vm);

      if (operand2 > operand1) {
        vm->_statusRegister |= 2;
      } else if (operand2 == operand1) {
        vm->_statusRegister |= 1;
      }

#ifdef DEBUG
      printf("CMP, STATE %d\n", vm->_statusRegister);
#endif

      break;
    }
    case OP_JE: {
      uint16_t address = cycle(vm) + (cycle(vm) << 8);

#ifdef DEBUG
      printf("JUMP IF EQUAL, TO ADDRESS 0x%04X\n", address);
#endif
      // Means bit 1 is 1 so zero flag is set
      if (vm->_statusRegister == 1) {
        push(vm, vm->_programCounter);
        push(vm, (vm->_programCounter) >> 8);
        vm->_programCounter = address;
      }

      break;
    }
    case OP_JNE: {
      uint16_t address = cycle(vm) + (cycle(vm) << 8);

#ifdef DEBUG
      printf("JUMP IF NOT EQUAL, TO ADDRESS 0x%04X\n", address);
#endif
      // Means Zero flag is not set so cannot be equal
      if (vm->_statusRegister != 1) {
        push(vm, vm->_programCounter);
        push(vm, (vm->_programCounter) >> 8);
        vm->_programCounter = address;
      }

      break;
    }
    case OP_JG: {
      uint16_t address = cycle(vm) + (cycle(vm) << 8);

#ifdef DEBUG
      printf("JUMP IF GREATER, TO ADDRESS 0x%04X\n", address);
#endif
      // Neither bit is set which means it isnt zero and isnt less
      if (vm->_statusRegister == 0) {
        push(vm, vm->_programCounter);
        push(vm, (vm->_programCounter) >> 8);
        vm->_programCounter = address;
      }

      break;
    }
    case OP_JL: {
      uint16_t address = cycle(vm) + (cycle(vm) << 8);

#ifdef DEBUG
      printf("JUMP IF LESS, TO ADDRESS 0x%04X\n", address);
#endif
      // Means negative flag is set
      if (vm->_statusRegister == 2) {
        push(vm, vm->_programCounter);
        push(vm, (vm->_programCounter) >> 8);
        vm->_programCounter = address;
      }

      break;
    }
    case OP_PUSH: {
      uint8_t val = cycle(vm);
      push(vm, val);

#ifdef DEBUG
      printf("PUSH %d TO STACK\n", val);
#endif

      break;
    }
    case OP_POP: {
      uint8_t val = cycle(vm);
      push(vm, val);

#ifdef DEBUG
      printf("PUSH %d TO STACK\n", val);
#endif

      break;
    }
    case OP_HALT: {
      printf("Program ran successfully.\n");
      exit(0);
    }
    default:
      printf("Unkown Command '%d'.\n", op);
      exit(-1);
    }
  }
}
