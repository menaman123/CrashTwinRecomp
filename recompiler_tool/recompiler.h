#ifndef RECOMPILER_H
#define RECOMPILER_H

#include <vector>
#include <string>
#include <fstream>
#include <set>
#include <map>
#include <capstone/capstone.h>
#include "cpu_state.h"

// Struct for representing a basic block of instructions
struct basic_block {
    uint64_t start_address;
    uint64_t end_address;
    std::vector<cs_insn*> instructions;
};

// Function Declarations

bool is_control_flow_instruction(const cs_insn& insn);
bool is_return(cs_insn& insn);
bool is_function_call(cs_insn& insn);
bool is_direct_function_call(cs_insn& insn);
bool is_direct_branch(cs_insn& insn);
bool is_branch_likely(cs_insn& insn);
uint32_t calculate_target(cs_insn& insn);

std::set<uint64_t> collect_function_entries(cs_insn* insns, size_t count);

std::vector<basic_block> collect_basic_blocks(cs_insn* insns, size_t count);

void generate_functions_from_block(const std::vector<basic_block>& blocks, std::ofstream& out_file);
void translate_instruction_block(std::ofstream& out_file, cs_insn* insn);
void translate_likely_instructions(std::ofstream& out_file, cs_insn* branch_insn, cs_insn* delay_slot_insn);

int get_gpr_index(mips_reg capstone_reg);

#endif // RECOMPILER_H
