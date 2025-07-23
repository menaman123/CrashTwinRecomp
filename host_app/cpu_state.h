#pragma once

#include <cstdint>

// Basic type definitions from PCSX2's Pcsx2Defs.h
using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using s8 = int8_t;
using s16 = int16_t;
using s32 = int32_t;
using s64 = int64_t;

// Basic vector types that GPRs can be viewed as.
union u128 {
    u64 UD[2];
    u32 UL[4];
    u16 US[8];
    u8 UC[16];
};

union s128 {
    s64 SD[2];
    s32 SL[4];
    s16 SS[8];
    s8 SC[16];
};


// --- Structs from R5900.h, simplified for recompilation ---

// Represents a single 128-bit General Purpose Register (GPR).
union GPR_reg {
    u128 UQ;
    s128 SQ;
    u64 UD[2];
    s64 SD[2];
    u32 UL[4];
    s32 SL[4];
    u16 US[8];
    s16 SS[8];
    u8  UC[16];
    s8  SC[16];
};

// Represents all 32 GPRs.
union GPRregs {
    struct {
        GPR_reg r0, at, v0, v1, a0, a1, a2, a3,
                t0, t1, t2, t3, t4, t5, t6, t7,
                s0, s1, s2, s3, s4, s5, s6, s7,
                t8, t9, k0, k1, gp, sp, s8, ra;
    } n;
    GPR_reg r[32];
};

// Represents the 32 registers of Coprocessor 0 (the System Control Coprocessor).
// These handle exceptions, memory management, and other system-level tasks.
union CP0regs {
    struct {
        u32   Index,    Random,    EntryLo0,  EntryLo1,
              Context,  PageMask,  Wired,     Reserved0,
              BadVAddr, Count,     EntryHi,   Compare,
              Status,   Cause,     EPC,       PRid,
              Config,   LLAddr,    WatchLO,   WatchHI,
              XContext, Reserved1, Reserved2, Debug,
              DEPC,     PerfCnt,   ErrCtl,    CacheErr,
              TagLo,    TagHi,     ErrorEPC,  DESAVE;
    } n;
    u32 r[32];
};

// A container for the core CPU registers.
struct cpuRegisters {
    GPRregs GPR;
    GPR_reg HI; // Special register for multiplication/division results
    GPR_reg LO; // Special register for multiplication/division results
    CP0regs CP0;
    u32 sa;     // Shift Amount register
    u32 pc;     // The Program Counter
    // Emulator-specific fields like IsDelaySlot, code, etc., are omitted.
};

// Represents a single 32-bit Floating Point Register (FPR).
union FPRreg {
    float f;
    u32 UL;
    s32 SL;
};

// Represents the state of the Floating Point Unit (FPU).
struct fpuRegisters {
    FPRreg fpr[32];
    u32 fprc[32]; // FPU control registers
    FPRreg ACC;   // Accumulator register
};

// This is the main, complete state of the Emotion Engine that our
// recompiled functions will operate on. It contains the core CPU
// registers and the FPU registers.
struct EmotionEngineState {
    cpuRegisters cpuRegs;
    fpuRegisters fpuRegs;
};