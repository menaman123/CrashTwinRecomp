#include <Function.h>
#include <rabbitizer.h>
#include "instructions/RabbitizerInstructionR5900.h"

void Function::find_basic_blocks(const uint8_t* code, uint32_t code_size){
    RabbitizerInstruction insn;
    uint32_t offset = 0;

    while(code + offset < code_size){
        
        char buffer[256];
        



        // Check if the instruction is valid
        if (RabbitizerInstruction_isValid(&insn)) {

            Block current_block;
            current_block.start_address = code + offset;

            while(code + offset < code_size){
                const uint8_t* curr_address = code + offset;
                uint32_t raw_data = *(reinterpret_cast<const uint32_t*>(curr_address));

                RabbitizerInstructionR5900_init(&insn, raw_data, curr_address);
                RabbitizerInstructionR5900_processUniqueId(&insn);
                RabbitizerInstruction_disassemble(&insn, buffer, NULL, 0, 0);

                std::cout << "0x" << std::hex << current_address
                        << ":\t" << buffer << std::dec << std::endl;
                // Basic block construction start and iterate
                
                current_block.instructions.push_back(buffer);

                if(RabbitizerInstruction_hasDelaySlot(&insn)){
                    // Add the delay slot into the block instructions
                    offset += 4;
                    RabbitizerInstruction_disassemble(&insn, buffer, NULL, 0, 0);
                    current_block.instructions.push_back(buffer);
                    RabbitizerInstruction_destroy(&insn);
                    break;
                }
                RabbitizerInstruction_destroy(&insn);
                offset += 4;
            }
            // Block finished being created now add to the function
            // Check to see if end of function

            current_block.end_address = code + offset;
            self.blocks.push_back(current_block);


            RabbitizerInstruction cf_insn;
            // Check to see what type branch/jump ended the block to see how to handle, curr_address - 4 to account for the delay slot
            uint32_t cf_insn_raw_data = *(reinterpret_cast<const uint32_t*>(curr_address - 4));
            RabbitizerInstructionR5900_init(&cf_insn, cf_insn_raw_data, curr_address - 4);
            RabbitizerInstructionR5900_processUniqueId(&cf_insn);
            RabbitizerInstruction_disassemble(&cf_insn, buffer, NULL, 0, 0);

            // If jr instruction with known target (non $ra register) this will be end of the function, done collecting blocks
            if(RabbitizerInstruction_isJumptableJump(&cf_insn)){
                break;
            }
        } else {

            // Failure - print the raw data and a detailed breakdown
            std::cout << "0x" << std::hex << current_address
                      << ":\t.word   0x" << std::setw(8) << std::setfill('0') << raw_data
                      << "  // <invalid instruction> ID: " << std::dec << insn.uniqueId
                      << " (" << RabbitizerInstrId_getOpcodeName(insn.uniqueId) << ")"
                      << " | opcode: 0x" << std::hex <<RAB_INSTR_GET_opcode(&insn)
                      << " | rs: " << std::dec <<RAB_INSTR_GET_rs(&insn)
                      << " | rt: " << std::dec <<RAB_INSTR_GET_rt(&insn)
                      << " | rd: " << std::dec <<RAB_INSTR_GET_rd(&insn)
                      << " | sa: " << std::dec <<RAB_INSTR_GET_sa(&insn)
                      << " | function: 0x" << std::hex <<RAB_INSTR_GET_function(&insn)
                      << std::dec << std::endl;
        }
        off_set += 4;
    }
    
}