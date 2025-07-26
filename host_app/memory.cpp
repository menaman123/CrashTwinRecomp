#include "memory.h"

// Defines and allocates the main memory for the emulated PS2.
// 32MB = 32 * 1024 * 1024 bytes.
std::vector<uint8_t> main_memory(32 * 1024 * 1024);

u32 ReadMemory32(u32 address) {
    // TODO: Implement the logic to read a 32-bit value.
    // 1. Check if the address is within the bounds of main_memory.
    //    (e.g., if (address > main_memory.size() - 4) { /* handle error */ })
    // 2. The PS2 is little-endian. You need to read 4 bytes and combine them.
    //    u32 value = 0;
    //    value |= (u32)main_memory[address];
    //    value |= (u32)main_memory[address + 1] << 8;
    //    value |= (u32)main_memory[address + 2] << 16;
    //    value |= (u32)main_memory[address + 3] << 24;
    //    return value;

    return 0; // Placeholder
}

void WriteMemory32(u32 address, u32 value) {
    // TODO: Implement the logic to write a 32-bit value.
    // 1. Check if the address is within the bounds of main_memory.
    // 2. The PS2 is little-endian. You need to break the 32-bit value into 4 bytes.
    //    main_memory[address]     = (value & 0x000000FF);
    //    main_memory[address + 1] = (value & 0x0000FF00) >> 8;
    //    main_memory[address + 2] = (value & 0x00FF0000) >> 16;
    //    main_memory[address + 3] = (value & 0xFF000000) >> 24;
}
