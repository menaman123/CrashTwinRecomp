

#include <iostream>
#include <fstream>
#include <vector>
#include <capstone/capstone.h>
#include "cpu_state.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <path_to_game_binary>" << std::endl;
        return 1;
    }

    const std::string file_path = argv[1];
    std::ifstream game_file(file_path, std::ios::binary);
    if (!game_file) {
        std::cerr << "Error: Could not open file " << file_path << std::endl;
        return 1;
    }

    game_file.seekg(0, std::ios::end);
    size_t file_size = game_file.tellg();
    game_file.seekg(0, std::ios::beg);

    std::vector<uint8_t> file_buffer(file_size);
    game_file.read(reinterpret_cast<char*>(file_buffer.data()), file_size);

    // --- MY PART: CAPSTONE SETUP ---
    csh handle;
    cs_insn *insn;
    size_t count;

    if (cs_open(CS_ARCH_MIPS, CS_MODE_MIPS64, &handle) != CS_ERR_OK) {
        std::cerr << "ERROR: Failed to initialize Capstone" << std::endl;
        return -1;
    }

    // Tell Capstone to give us details about the instructions (operands, etc.)
    cs_option(handle, CS_OPT_DETAIL, CS_OPT_ON);

    count = cs_disasm(handle, file_buffer.data(), file_buffer.size(), 0x00100000, 0, &insn);

    /*
    ┌──────────────────────┬────────────────────────────────────────────────┬─────────────────┐
    │ What You Want        │ How to Get It                                  │ Example Value   │
    ├──────────────────────┼────────────────────────────────────────────────┼─────────────────┤
    │ Instruction Type     │ current_insn.id                                │ MIPS_INS_ADDIU  │
    │ Destination Register │ current_insn.detail->mips.operands[0].reg      │ MIPS_REG_V0     │
    │ Source Register 1    │ current_insn.detail->mips.operands[1].reg      │ MIPS_REG_SP     │
    │ Source Register 2    │ current_insn.detail->mips.operands[2].reg      │ (N/A for addiu) │
    │ Immediate Value      │ current_insn.detail->mips.operands[2].imm      │ 16              │
    │ Memory Base Register │ current_insn.detail->mips.operands[1].mem.base │ MIPS_REG_SP     │
    │ Memory Offset        │ current_insn.detail->mips.operands[1].mem.disp │ 16              │
    └──────────────────────┴────────────────────────────────────────────────┴─────────────────┘  
    */

    if (count > 0) {
        std::cout << "// Successfully disassembled " << count << " instructions." << std::endl;
        std::cout << "// Starting C++ code generation..." << std::endl << std::endl;

        for (size_t i = 0; i < count; i++) {
            // --- YOUR PART: TRANSLATION LOGIC ---
            // ==================================================
            // TODO: Add your switch statement here to check insn[i].id
            // and generate the appropriate C++ code.
            // A variable to hold the current instruction for easier access
            cs_insn& current_insn = insn[i];

            switch(current_insn.id) {
                // --- INSTRUCTIONS YOU HAVE COMPLETED ---
                case MIPS_INS_ADDIU: {
                    const auto& dest = current_insn.detail->mips.operands[0].reg;
                    const auto& source = current_insn.detail->mips.operands[1].reg;
                    const auto& imm = current_insn.detail->mips.operands[2].imm;
                    std::cout << "context.cpuRegs.GPR.r[" << dest << "].SD[0] = (s64)(s32)(context.cpuRegs.GPR.r[" << source << "].SD[0] + " << imm << ");" << std::endl;
                    break;
            }
                case MIPS_INS_LW: {
                    const auto& dest = current_insn.detail->mips.operands[0].reg;
                    const auto& base = current_insn.detail->mips.operands[1].mem.base;
                    const auto& offset = current_insn.detail->mips.operands[1].mem.disp;
                    std::cout << "{" << std::endl;
                    std::cout << "    u32 address = context.cpuRegs.GPR.r[" << base << "].UD[0] + " << offset << ";" << std::endl;
                    std::cout << "    context.cpuRegs.GPR.r[" << dest << "].SD[0] = (s64)(s32)ReadMemory32(address);" << std::endl;
                    std::cout << "}" << std::endl;
                    break;
                }
                case MIPS_INS_SW: {
                    const auto& source = current_insn.detail->mips.operands[0].reg;
                    const auto& base = current_insn.detail->mips.operands[1].mem.base;
                    const auto& offset = current_insn.detail->mips.operands[1].mem.disp;
                    std::cout << "{" << std::endl;
                    std::cout << "    u32 address = context.cpuRegs.GPR.r[" << base << "].UD[0] + " << offset << ";" << std::endl;
                    std::cout << "    WriteMemory32(address, (u32)context.cpuRegs.GPR.r[" << source << "].UD[0]);" << std::endl;
                    std::cout << "}" << std::endl;
                    break;
                }
                case MIPS_INS_JR: {
                    const auto& target_reg = current_insn.detail->mips.operands[0].reg;
                    if (i + 1 < count) {
                        cs_insn& delay_slot_insn = insn[i + 1];
                        handle_delay_slot_instruction(delay_slot_insn);
                        std::cout << "context.cpuRegs.pc = context.cpuRegs.GPR.r[" << target_reg << "].UD[0];" << std::endl;
                        i++;
                    } else {
                        std::cout << "context.cpuRegs.pc = context.cpuRegs.GPR.r[" << target_reg << "].UD[0];" << std::endl;
                    }
                    break;
                }

                // --- LOGICAL OPERATIONS ---
                case MIPS_INS_OR: {
                    // TODO: Implement OR instruction.
                    // MIPS: or rd, rs, rt
                    // C++: rd = rs | rt
                    // Operation is on 64-bit registers.
                    // LOGICAL OPERATIONS ARE DONE ON 64 BITS, ARITHMETIC ARE DONE ON 32 BITS

                    const auto& dest = current_insn.detail->mips.operands[0].reg;
                    const auto& r1 = current_insn.detail->mips.operands[1].reg;
                    const auto& r2 = current_insn.detail->mips.operands[2].reg;

                    std::cout << "context.cpuRegs.GPR.r[" << dest << "].UD[0] = (context.cpuRegs.GPR.r[" << r1 << "].UD[0] | context.cpuRegs.GPR.r[" << r2 << "].UD[0]);" << std::endl;
                    break;
                }
                case MIPS_INS_AND: {
                    // TODO: Implement AND instruction.
                    // MIPS: and rd, rs, rt
                    // C++: rd = rs & rt
                    // Operation is on 64-bit registers.
                    const auto& dest = current_insn.detail->mips.operands[0].reg;
                    const auto& r1 = current_insn.detail->mips.operands[1].reg;
                    const auto& r2 = current_insn.detail->mips.operands[2].reg;

                    std::cout << "context.cpuRegs.GPR.r[" << dest << "].UD[0] = (context.cpuRegs.GPR.r[" << r1 << "].UD[0] & context.cpuRegs.GPR.r[" << r2 << "].UD[0]);" << std::endl;
                    break;
                }
                case MIPS_INS_LUI: {
                    // TODO: Implement LUI (Load Upper Immediate).
                    // MIPS: lui rt, immediate
                    // C++: rt = (immediate << 16)
                    // This is a 32-bit operation, the result must be sign-extended to 64 bits.
                    // const auto& rt = current_insn.detail->mips.operands[0].reg;
                    // const auto& imm = current_insn.detail->mips.operands[1].imm;


                    const auto& r1 = current_insn.detail->mips.operands[0].reg;
                    const auto& imm = current_insn.detail->mips.operands[1].imm;

                    //  r1 = s64(imm << 16)

                    std::cout << "context.cpuRegs.GPR.r[" << r1 << "].SD[0] = (s64)(s32)(imm << 16);" << std::endl;
                    break;
                }

                // --- SHIFT OPERATIONS ---
                case MIPS_INS_SLL: {
                    // TODO: Implement SLL (Shift Left Logical).
                    // MIPS: sll rd, rt, sa
                    // C++: rd = rt << sa
                    // This is a 32-bit operation. The lower 32 bits of rt are shifted.
                    // The result in rd is sign-extended.
                    const auto& rd = current_insn.detail->mips.operands[0].reg;
                    const auto& rt = current_insn.detail->mips.operands[1].reg;
                    const auto& sa = current_insn.detail->mips.operands[2].imm;
                    
                    std::cout << "context.cpuRegs.GPR.r[" << rd << "].SD[0] = (s64)(s32)((u32)context.cpuRegs.GPR.r[" << rt << "].UD[0] << " << sa << ");"
                    break;
                }

                // --- BRANCH INSTRUCTIONS ---
                case MIPS_INS_BEQ: {
                    // TODO: Implement BEQ (Branch on Equal).
                    // MIPS: beq rs, rt, offset
                    // C++: if (rs == rt) { pc = pc + offset; }
                    // Remember the branch delay slot!
                    // 1. Get the operands rs, rt, and the immediate offset.
                    // 2. Generate C++ for the comparison: if (context.cpuRegs.GPR.r[rs] == context.cpuRegs.GPR.r[rt]) { ... }
                    // 3. Inside the if, you need to set the PC to the branch target.
                    //    The target is calculated as: (current_instruction_address + 4) + (offset << 2)
                    // 4. After the if statement, you must handle the delay slot by calling handle_delay_slot_instruction().

                    // 5. Remember to advance the loop counter `i++`.
                    const auto& rs = current_insn.detail->mips.operands[0].reg;
                    const auto& rt = current_insn.detail->mips.operands[1].reg;
                    const auto& off = current_insn.detail->mips.operands[2].imm;

                    if (i + 1 < count){
                        handle_delay_slot_instruction(insn[i+1]);
                    }
                    std::cout << "if (context.cpuRegs.GPR.r[" << rs << "].UD[0] == context.cpuRegs.GPR.r["  << rt  << "].UD[0]) {"<< std::endl;
                    std::cout << "context.cpuRegs.pc = " << current_insn.address << " +  4 + (" << off << " << 2);" << std::endl;
                    std::cout << "} else {" << std:: endl;
                    std::cout << "context.cpuRegs.pc = " << current_insn.address << " +  8;" << std::endl;
                    std::cout << "}" << std::endl;
                    ++i;
                    break;
                }
                case MIPS_INS_BNE: {
                    // TODO: Implement BNE (Branch on Not Equal).
                    // MIPS: bne rs, rt, offset
                    // C++: if (rs != rt) { pc = pc + offset; }
                    // Follow the same logic as BEQ.
                    const auto& rs = current_insn.detail->mips.operands[0].reg;
                    const auto& rt = current_insn.detail->mips.operands[1].reg;
                    const auto& off = current_insn.detail->mips.operands[2].imm;

                    if (i + 1 < count){
                        handle_delay_slot_instruction(insn[i+1]);
                    }
                    std::cout << "if (context.cpuRegs.GPR.r[" << rs << "].UD[0] != context.cpuRegs.GPR.r["  << rt  << "].UD[0]) {"<< std::endl;
                    std::cout << "context.cpuRegs.pc = " << current_insn.address << " +  4 + (" << off << " << 2);" << std::endl;
                    std::cout << "} else {" << std:: endl;
                    std::cout << "context.cpuRegs.pc = " << current_insn.address << " +  8;" << std::endl;
                    std::cout << "}" << std::endl;
                    ++i;
                    break;
                }

                default:
                    // This is useful for seeing what you still need to implement.
                    std::cout << "// Unhandled instruction: " << current_insn.mnemonic << std::endl;
                    break;
            }
        }

        cs_free(insn, count);
    }
 } else {
        std::cerr << "ERROR: Failed to disassemble any code!" << std::endl;
        return -1;
    }

    cs_close(&handle);

    return 0;
}

