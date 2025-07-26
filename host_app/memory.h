#pragma once

#include "cpu_state.h"
#include <vector>

// This declares a global variable for the PS2's main memory (32MB).
// It will be defined in memory.cpp.
extern std::vector<uint8_t> main_memory;

/**
 * @brief Reads a 32-bit (4-byte) value from the emulated memory.
 * @param address The 32-bit memory address to read from.
 * @return The 32-bit value at that address.
 */
u32 ReadMemory32(u32 address);

/**
 * @brief Writes a 32-bit (4-byte) value to the emulated memory.
 * @param address The 32-bit memory address to write to.
 * @param value The 32-bit value to write.
 */
void WriteMemory32(u32 address, u32 value);

// TODO: Add declarations for ReadMemory16, WriteMemory16, ReadMemory8, WriteMemory8
// as you find you need them for instructions like LB, SB, LH, SH etc.
