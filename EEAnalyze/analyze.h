#pragma once

#include <vector>
#include <set>
#include <cstdint>
#include "Function.h" // We will include the new Function class

/**
 * Scans the entire .text section to find the starting address of every function.
 * It does this by finding all instructions that are targets of a JAL instruction
 * and any functions listed in the ELF's symbol table.
 * @param code A pointer to the raw bytes of the .text section.
 * @param code_size The size of the .text section.
 * @return A set of unique addresses, each being the start of a function.
 */
std::set<uint32_t> find_function_starts(const uint8_t* code, uint32_t code_size);


/**
 * The main entry point for the entire analysis phase.
 * It finds all function starts and then analyzes each one to produce a complete
 * representation of the program.
 * @param code A pointer to the raw bytes of the .text section.
 * @param code_size The size of the .text section.
 * @return A vector of fully analyzed Function objects.
 */
std::vector<Function> build_functions(const uint8_t* code, uint32_t code_size);