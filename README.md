# GISC-CPU (Goofy Instruction Set Computer)
A simulated 8 bit CPU written in the lord's language, C, that uses the custom instruction set GISC-1.


## Opcodes

| Description                                        | Arguments                                | Assembly Name | OpCode |
| :------------------------------------------------- | :--------------------------------------- | :-----------: | :----: |
| Add To Register                                    | Register, Value                          |      add      |  0x01  |
| Subtract From Register                             | Register, Value                          |      sub      |  0x02  |
| Load to reigster                                   | Register, Address                        |      ld       |  0x03  |
| Move Between Registers                             | Register (src), Register (dest)          |      mv       |  0x04  |
| Jump to Address                                    | Address                                  |      jmp      |  0x05  |
| Add Registers                                      | Register (dest), Register                |     addr      |  0x06  |
| Subtract Registers                                 | Register (dest), Register                |     subr      |  0x07  |
| XOR Registers                                      | Reigster (dest), Register                |      xor      |  0x08  |
| AND Registers                                      | Register (dest), Register                |      and      |  0x09  |
| OR Registers                                       | Register (dest), Register                |      or       |  0x0A  |
| NAND Registers                                     | Register (dest), Register                |     nand      |  0x0B  |
| NOT Registers                                      | Register (dest), Register                |      not      |  0x0C  |
| Shift Value in register                            | Register (dest), Register (shift amount) |     shft      |  0x0D  |
| Store in Memory                                    | Register, Address                        |      st       |  0x0E  |
| Return to Previous Address                         | None                                     |      ret      |  0x0F  |
| Compare Values and store result in status register | Register, Register                       |      cmp      |  0x11  |
| Jump if status register is equal                   | Address                                  |      je       |  0x12  |
| Jump if status register is not equal               | Address                                  |      jne      |  0x13  |
| Jump if status register is greater                 | Address                                  |      jg       |  0x14  |
| Jump if status register is less                    | Address                                  |      jl       |  0x15  |
| Push to stack                                      | Register                                 |     push      |  0x16  |
| Pop from stack                                     | Register                                 |      pop      |  0x17  |
| Halt the program                                   | None                                     |     halt      |  0x18  |


## Registers

| Description                 | Assembly Name | Operand Name |
| --------------------------- | :-----------: | :----------: |
| Status register             |      SR       |     0x01     |
| Stack Pointer               |      SP       |     0x02     |
| Program Counter             |      PC       |     0x03     |
| General Purpose Register 0  |      G0       |     0x04     |
| General Purpose Register 1  |      G1       |     0x05     |
| General Purpose Register 2  |      G2       |     0x06     |
| General Purpose Register 3  |      G3       |     0x07     |
| General Purpose Register 4  |      G4       |     0x08     |
| General Purpose Register 5  |      G5       |     0x09     |
| General Purpose Register 6  |      G6       |     0x0A     |
| General Purpose Register 7  |      G7       |     0x0B     |
| General Purpose Register 8  |      G8       |     0x0C     |
| General Purpose Register 9  |      G9       |     0x0D     |
| General Purpose Register 10 |      G10      |     0x0E     |