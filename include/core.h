#ifndef LIBGIGATRON_H
#define LIBGIGATRON_H

#include <stdint.h>

extern bool turbo;
extern uint8_t ROM[1 << 16][2], RAM[1 << 15], IN_PORT; // 128kb rom, 32kb ram, input port

typedef struct {
    uint8_t IR, D, AC, X, Y, OUT_PORT, undef;
    uint16_t PC;
} CpuState;

CpuState cycle(CpuState s);

#endif
