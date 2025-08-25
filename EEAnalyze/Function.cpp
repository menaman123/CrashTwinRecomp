   #include "Function.h"
   #include "instructions/RabbitizerInstructionR5900.h"
   #include <iostream>
   #include <iomanip>
   
   void Function::find_basic_blocks(const uint8_t* code, uint32_t code_size) {
       // This function assumes 'this->base_address' has been set in the constructor.
       uint32_t offset = 0;
   
       // Use the corrected, safe loop condition.
       while (offset + 4 <= code_size) {
   
           // --- Create a new block ---
           Block current_block;
           current_block.start_address = this->base_address + offset;
   
           RabbitizerInstruction last_instr_in_block;
           bool function_ended = false;
   
           // --- Inner loop to populate the block ---
           while (offset + 4 <= code_size) {
               uint32_t current_vram = this->base_address + offset;
               uint32_t instruction_word = *(reinterpret_cast<const uint32_t*>(code + offset));
   
               RabbitizerInstruction current_instr;
               RabbitizerInstructionR5900_init(&current_instr, instruction_word, current_vram);
               RabbitizerInstructionR5900_processUniqueId(&current_instr);
   
               // Add the decoded instruction struct to the vector, not the char buffer.
               current_block.instructions.push_back(current_instr);
               offset += 4;
   
               // Check if this instruction has a delay slot (i.e., it's a branch or jump)
               if (RabbitizerInstruction_hasDelaySlot(&current_instr)) {
                   // The branch/jump instruction is the last one for this block's logic.
                   // The delay slot instruction will be the first instruction of the NEXT block.
                   last_instr_in_block = current_instr;
                   break;
               }
           }
   
           // --- Finalize and store the block ---
           current_block.end_address = this->base_address + offset;
           this->blocks.push_back(current_block);
   
           // --- Check if the function should end ---
           // If the last instruction was a `jr` that is not `jr $ra` (a jumptable jump),
           // we can assume the function analysis is over.
           if (RabbitizerInstruction_isJumptableJump(&last_instr_in_block)) {
               function_ended = true;
           }
   
           RabbitizerInstruction_destroy(&last_instr_in_block);
   
           if (function_ended) {
               break; // Exit the outer while loop
           }
       }
   }
