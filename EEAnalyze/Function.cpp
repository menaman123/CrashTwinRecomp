#include "Function.h"
#include "instructions/RabbitizerInstructionR5900.h"
#include <set>
#include <vector>
#include <algorithm>

void Function::find_basic_blocks(const uint8_t* code, uint32_t code_size) {
    if (code_size == 0) return;

    // --- PASS 1: Find all leader addresses (same as before) ---
    std::set<uint32_t> leader_addresses;
    leader_addresses.insert(this->base_address);

    for (uint32_t offset = 0; offset + 4 <= code_size; offset += 4) {
        uint32_t current_vram = this->base_address + offset;
        uint32_t instruction_word = *(reinterpret_cast<const uint32_t*>(code + offset));

        RabbitizerInstruction instr;
        RabbitizerInstructionR5900_init(&instr, instruction_word, current_vram);
        RabbitizerInstructionR5900_processUniqueId(&instr);

        if (RabbitizerInstrDescriptor_isBranch(&instr) || RabbitizerInstrDescriptor_isJump(&instr)) {
            // The instruction AFTER the delay slot is a leader.
            if (offset + 8 <= code_size) {
                leader_addresses.insert(current_vram + 8);
            }

            // The target of the branch/jump is a leader.
            uint32_t target_vram = 0;
            if (RabbitizerInstrDescriptor_isUnconditionalBranch(&instr)) {
                target_vram = RabbitizerInstruction_getInstrIndexAsVram(&instr);
            } else {
                target_vram = RabbitizerInstruction_getBranchVramGeneric(&instr);
            }
            leader_addresses.insert(target_vram);
        }
        RabbitizerInstruction_destroy(&instr);
    }

    // --- PASS 2: Form blocks based on your model ---
    std::vector<uint32_t> sorted_leaders(leader_addresses.begin(), leader_addresses.end());
    std::sort(sorted_leaders.begin(), sorted_leaders.end());

    for (size_t i = 0; i < sorted_leaders.size(); ++i) {
        uint32_t block_start_addr = sorted_leaders[i];

        // The block ends right before the next leader starts.
        uint32_t block_end_addr_exclusive = (i + 1 < sorted_leaders.size()) ? sorted_leaders[i+1] : (this->base_address + code_size);

        Block current_block;
        current_block.start_address = block_start_addr;
        current_block.end_address = block_end_addr_exclusive;

        // Populate the block with its instructions.
        for (uint32_t addr = block_start_addr; addr < block_end_addr_exclusive; addr += 4) {
            uint32_t instruction_offset = addr - this->base_address;
            uint32_t instruction_word = *(reinterpret_cast<const uint32_t*>(code + instruction_offset));

            RabbitizerInstruction decoded_instr;
            RabbitizerInstructionR5900_init(&decoded_instr, instruction_word, addr);
            RabbitizerInstructionR5900_processUniqueId(&decoded_instr);
            current_block.instructions.push_back(decoded_instr);
        }

        if (!current_block.instructions.empty()) {
            this->blocks.push_back(current_block);
        }
    }
}