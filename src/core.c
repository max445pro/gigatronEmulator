#include "../include/core.h"
#include <stdlib.h>
#include <stdio.h>

// Initialization

bool turbo = 0;
uint8_t ROM[1 << 16][2], RAM[1 << 15], IN_PORT=0b11111111;  // 128kb rom, 32kb ram, input port
                                                            // Buttons (MSB tp LSB): A, ?, ?, B?, ?, DOWN, ?, ?

// Execution
CpuState cycle(CpuState S) {
    CpuState T = S;

    T.IR = ROM[S.PC][0];  // Fetch opcode
    T.D  = ROM[S.PC][1];  // Fetch operand

    uint8_t ins = S.IR >> 5;
    uint8_t mod = (S.IR >> 2) & 7;
    uint8_t bus = S.IR & 3;
    int W = (ins == 6);
    int J = (ins == 7);

    uint8_t lo = S.D, hi = 0, *dest = NULL;
    int incX = 0;
    if (!J) {
        switch (mod) {
            #define E(p) (W?0:p)
            case 0: dest = E(&T.AC);                                    break; 
            case 1: dest = E(&T.AC); lo = S.X;                          break;
            case 2: dest = E(&T.AC);           hi = S.Y;                break;
            case 3: dest = E(&T.AC); lo = S.X; hi = S.Y;                break;
            case 4: dest =  &T.X;                                       break; 
            case 5: dest =  &T.Y;                                       break;
            case 6: dest = E(&T.OUT_PORT);                              break;
            case 7: dest = E(&T.OUT_PORT);lo = S.X; hi = S.Y; incX = 1; break;
        }
    }
    uint16_t addr = (hi << 8) | lo;

    uint8_t B = S.undef;    // Data bus
    switch (bus) {
        case 0: B = S.D;                        break;
        case 1: if (!W) B = RAM[addr & 0x7fff]; break;
        case 2: B = S.AC;                       break;
        case 3: B = IN_PORT;                    break;
    }

    if (W) RAM[addr & 0x7fff] = B;  // Writes to RAM

    uint8_t ALU;    // Aritchmetic and Logic Unit
    switch (ins) {
        case 0: ALU =        B;  break;
        case 1: ALU = S.AC & B;  break;
        case 2: ALU = S.AC | B;  break;
        case 3: ALU = S.AC ^ B;  break;
        case 4: ALU = S.AC + B;  break;
        case 5: ALU = S.AC - B;  break;
        case 6: ALU = S.AC;      break;
        case 7: ALU = -S.AC;     break;
    }

    if (dest) *dest = ALU;  // Load value into register
    if (incX) T.X = S.X + 1;

    T.PC = S.PC + 1;
    if (J) {
        if (mod != 0) {     // Branch
            int cond = (S.AC >> 7) + 2*(S.AC == 0);
            if (mod & (1 << cond)) {    // Boolean logic madness
                T.PC = (S.PC & 0xff00) | B;
            }
        } else {
            T.PC = (S.Y << 8) | B;
        }
    }
    return T;
}
