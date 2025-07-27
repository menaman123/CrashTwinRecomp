
#include <iostream>
#include <fstream>
#include <vector>
#include <capstone/capstone.h>
#include "cpu_state.h"

// Helper function to map Capstone's register enum to the correct 0-31 GPR index.
// This function should be placed in main.cpp, typically above the main() function.
int get_gpr_index(mips_reg capstone_reg) {
    switch (capstone_reg) {
        // General Purpose Registers (GPRs)
        case MIPS_REG_ZERO: return 0; // $zero
        case MIPS_REG_AT:   return 1; // $at
        case MIPS_REG_V0:   return 2; // $v0
        case MIPS_REG_V1:   return 3; // $v1
        case MIPS_REG_A0:   return 4; // $a0
        case MIPS_REG_A1:   return 5; // $a1
        case MIPS_REG_A2:   return 6; // $a2
        case MIPS_REG_A3:   return 7; // $a3
        case MIPS_REG_T0:   return 8; // $t0
        case MIPS_REG_T1:   return 9; // $t1
        case MIPS_REG_T2:   return 10; // $t2
        case MIPS_REG_T3:   return 11; // $t3
        case MIPS_REG_T4:   return 12; // $t4
        case MIPS_REG_T5:   return 13; // $t5
        case MIPS_REG_T6:   return 14; // $t6
        case MIPS_REG_T7:   return 15; // $t7
        case MIPS_REG_S0:   return 16; // $s0
        case MIPS_REG_S1:   return 17; // $s1
        case MIPS_REG_S2:   return 18; // $s2
        case MIPS_REG_S3:   return 19; // $s3
        case MIPS_REG_S4:   return 20; // $s4
        case MIPS_REG_S5:   return 21; // $s5
        case MIPS_REG_S6:   return 22; // $s6
        case MIPS_REG_S7:   return 23; // $s7
        case MIPS_REG_T8:   return 24; // $t8
        case MIPS_REG_T9:   return 25; // $t9
        case MIPS_REG_K0:   return 26; // $k0
        case MIPS_REG_K1:   return 27; // $k1
        case MIPS_REG_GP:   return 28; // $gp
        case MIPS_REG_SP:   return 29; // $sp
        case MIPS_REG_S8:   return 30; // $fp (or $s8)
        case MIPS_REG_RA:   return 31; // $ra

        // If you encounter other types of registers (e.g., floating point, COP0),
        // you'll need to add cases for them and handle them appropriately.
        // For now, return -1 for unhandled or invalid registers to indicate an error.
        default:
            std::cerr << "ERROR: Unhandled Capstone register enum: " << capstone_reg << std::endl;
            return -1; // Or throw an exception
    }
}

// This function is now the heart of the recompiler.
// It translates an instruction and returns the number of instructions consumed (1 for normal, 2 for branch-with-delay-slot).
int translate_instruction_block(cs_insn* insn, size_t i, size_t total_count);

int main(int argc, char* argv[]) {
    // --- File loading and Capstone setup ---
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

    csh handle;
    cs_insn *insn;
    size_t count;
    if (cs_open(CS_ARCH_MIPS, (cs_mode)(CS_MODE_MIPS64 | CS_MODE_LITTLE_ENDIAN), &handle) != CS_ERR_OK) { 
        std::cerr << "ERROR: Failed to initialize Capstone" << std::endl;
        return -1; 
    }
    cs_option(handle, CS_OPT_DETAIL, CS_OPT_ON);
    count = cs_disasm(handle, file_buffer.data(), file_buffer.size(), 0x00100000, 0, &insn);

    if (count > 0) {
        std::cout << "// Successfully disassembled " << count << " instructions." << std::endl;
        std::cout << "// Starting C++ code generation..." << std::endl << std::endl;

        // --- Main Recompiler Loop ---
        size_t i = 0;
        while (i < count) {
            std::cerr << "DEBUG: Processing instruction " << i << " of " << count << std::endl;
            int instructions_consumed = translate_instruction_block(insn, i, count);
            i += instructions_consumed;
        }

        cs_free(insn, count);
    } else {
        std::cerr << "ERROR: Failed to disassemble any code!" << std::endl;
        return -1;
    }

    cs_close(&handle);
    return 0;
}

// This is the single source of truth for translating any MIPS instruction.
int translate_instruction_block(cs_insn* insn, size_t i, size_t total_count) {
    cs_insn& current_insn = insn[i];
    cs_mips& mips_details = current_insn.detail->mips;

    std::cerr << "DEBUG [" << current_insn.mnemonic << "]: " << mips_details.op_count << " operands." << std::endl;

    switch (current_insn.id) {
        // --- JUMP & BRANCH INSTRUCTIONS (Special Handling) ---
        case MIPS_INS_JR: {
            std::cerr << "  Operand 0 [target]: type=" << mips_details.operands[0].type << ", reg=" << mips_details.operands[0].reg << std::endl;
            const auto& target_reg_capstone = mips_details.operands[0].reg;
            int target_reg_index = get_gpr_index(target_reg_capstone);

            if (i + 1 < total_count) {
                translate_instruction_block(insn, i + 1, total_count);
            }
            std::cout << "context.cpuRegs.pc = context.cpuRegs.GPR.r[" << target_reg_index << "].UD[0];" << std::endl;
            return 2;
        }
        case MIPS_INS_BEQ: {
            std::cerr << "  Operand 0 [rs]: type=" << mips_details.operands[0].type << ", reg=" << mips_details.operands[0].reg << std::endl;
            std::cerr << "  Operand 1 [rt]: type=" << mips_details.operands[1].type << ", reg=" << mips_details.operands[1].reg << std::endl;
            std::cerr << "  Operand 2 [offset]: type=" << mips_details.operands[2].type << ", imm=" << mips_details.operands[2].imm << std::endl;
            const auto& rs_capstone = mips_details.operands[0].reg;
            const auto& rt_capstone = mips_details.operands[1].reg;
            const auto& off = mips_details.operands[2].imm;
            int rs_index = get_gpr_index(rs_capstone);
            int rt_index = get_gpr_index(rt_capstone);

            if (i + 1 < total_count) {
                translate_instruction_block(insn, i + 1, total_count);
            }

            std::cout << "if (context.cpuRegs.GPR.r[" << rs_index << "].UD[0] == context.cpuRegs.GPR.r[" << rt_index << "].UD[0]) {" << std::endl;
            std::cout << "    context.cpuRegs.pc = " << current_insn.address << " + 4 + (" << off << " << 2);" << std::endl;
            std::cout << "} else {" << std::endl;
            std::cout << "    context.cpuRegs.pc = " << current_insn.address << " + 8;" << std::endl;
            std::cout << "}" << std::endl;
            return 2;
        }
        case MIPS_INS_BNE: {
            std::cerr << "  Operand 0 [rs]: type=" << mips_details.operands[0].type << ", reg=" << mips_details.operands[0].reg << std::endl;
            std::cerr << "  Operand 1 [rt]: type=" << mips_details.operands[1].type << ", reg=" << mips_details.operands[1].reg << std::endl;
            std::cerr << "  Operand 2 [offset]: type=" << mips_details.operands[2].type << ", imm=" << mips_details.operands[2].imm << std::endl;
            const auto& rs_capstone = mips_details.operands[0].reg;
            const auto& rt_capstone = mips_details.operands[1].reg;
            const auto& off = mips_details.operands[2].imm;
            int rs_index = get_gpr_index(rs_capstone);
            int rt_index = get_gpr_index(rt_capstone);

            if (i + 1 < total_count) {
                translate_instruction_block(insn, i + 1, total_count);
            }

            std::cout << "if (context.cpuRegs.GPR.r[" << rs_index << "].UD[0] != context.cpuRegs.GPR.r[" << rt_index << "].UD[0]) {" << std::endl;
            std::cout << "    context.cpuRegs.pc = " << current_insn.address << " + 4 + (" << off << " << 2);" << std::endl;
            std::cout << "} else {" << std::endl;
            std::cout << "    context.cpuRegs.pc = " << current_insn.address << " + 8;" << std::endl;
            std::cout << "}" << std::endl;
            return 2;
        }

        // --- STANDARD INSTRUCTIONS (Default Handling) ---
        case MIPS_INS_ADDIU: {
            std::cerr << "  Operand 0 [dest]: type=" << mips_details.operands[0].type << ", reg=" << mips_details.operands[0].reg << std::endl;
            std::cerr << "  Operand 1 [source]: type=" << mips_details.operands[1].type << ", reg=" << mips_details.operands[1].reg << std::endl;
            std::cerr << "  Operand 2 [imm]: type=" << mips_details.operands[2].type << ", imm=" << mips_details.operands[2].imm << std::endl;
            const auto& dest_capstone = mips_details.operands[0].reg;
            const auto& source_capstone = mips_details.operands[1].reg;
            const auto& imm = mips_details.operands[2].imm;
            int dest_index = get_gpr_index(dest_capstone);
            int source_index = get_gpr_index(source_capstone);

            std::cout << "context.cpuRegs.GPR.r[" << dest_index << "].SD[0] = (s64)(s32)(context.cpuRegs.GPR.r[" << source_index << "].SD[0] + " << imm << ");" << std::endl;
            break;
        }
        case MIPS_INS_LW: {
            std::cerr << "  Operand 0 [dest]: type=" << mips_details.operands[0].type << ", reg=" << mips_details.operands[0].reg << std::endl;
            std::cerr << "  Operand 1 [mem]: type=" << mips_details.operands[1].type << ", base=" << mips_details.operands[1].mem.base << ", disp=" << mips_details.operands[1].mem.disp << std::endl;
            const auto& dest_capstone = mips_details.operands[0].reg;
            const auto& base_capstone = mips_details.operands[1].mem.base;
            const auto& offset = mips_details.operands[1].mem.disp;
            int dest_index = get_gpr_index(dest_capstone);
            int base_index = get_gpr_index(base_capstone);

            std::cout << "{" << std::endl;
            std::cout << "    u32 address = context.cpuRegs.GPR.r[" << base_index << "].UD[0] + " << offset << ";" << std::endl;
            std::cout << "    context.cpuRegs.GPR.r[" << dest_index << "].SD[0] = (s64)(s32)ReadMemory32(address);" << std::endl;
            std::cout << "}" << std::endl;
            break;
        }
        case MIPS_INS_SW: {
            std::cerr << "  Operand 0 [source]: type=" << mips_details.operands[0].type << ", reg=" << mips_details.operands[0].reg << std::endl;
            std::cerr << "  Operand 1 [mem]: type=" << mips_details.operands[1].type << ", base=" << mips_details.operands[1].mem.base << ", disp=" << mips_details.operands[1].mem.disp << std::endl;
            const auto& source_capstone = mips_details.operands[0].reg;
            const auto& base_capstone = mips_details.operands[1].mem.base;
            const auto& offset = mips_details.operands[1].mem.disp;
            int source_index = get_gpr_index(source_capstone);
            int base_index = get_gpr_index(base_capstone);

            std::cout << "{" << std::endl;
            std::cout << "    u32 address = context.cpuRegs.GPR.r[" << base_index << "].UD[0] + " << offset << ";" << std::endl;
            std::cout << "    WriteMemory32(address, (u32)context.cpuRegs.GPR.r[" << source_index << "].UD[0]);" << std::endl;
            std::cout << "}" << std::endl;
            break;
        }
        case MIPS_INS_OR: {
            std::cerr << "  Operand 0 [dest]: type=" << mips_details.operands[0].type << ", reg=" << mips_details.operands[0].reg << std::endl;
            std::cerr << "  Operand 1 [r1]: type=" << mips_details.operands[1].type << ", reg=" << mips_details.operands[1].reg << std::endl;
            std::cerr << "  Operand 2 [r2]: type=" << mips_details.operands[2].type << ", reg=" << mips_details.operands[2].reg << std::endl;
            const auto& dest_capstone = mips_details.operands[0].reg;
            const auto& r1_capstone = mips_details.operands[1].reg;
            const auto& r2_capstone = mips_details.operands[2].reg;
            int dest_index = get_gpr_index(dest_capstone);
            int r1_index = get_gpr_index(r1_capstone);
            int r2_index = get_gpr_index(r2_capstone);

            std::cout << "context.cpuRegs.GPR.r[" << dest_index << "].UD[0] = context.cpuRegs.GPR.r[" << r1_index << "].UD[0] | context.cpuRegs.GPR.r[" << r2_index << "].UD[0];" << std::endl;
            break;
        }
        case MIPS_INS_LUI: {
            std::cerr << "  Operand 0 [rt]: type=" << mips_details.operands[0].type << ", reg=" << mips_details.operands[0].reg << std::endl;
            std::cerr << "  Operand 1 [imm]: type=" << mips_details.operands[1].type << ", imm=" << mips_details.operands[1].imm << std::endl;
            const auto& rt_capstone = mips_details.operands[0].reg;
            const auto& imm = mips_details.operands[1].imm;
            int rt_index = get_gpr_index(rt_capstone);

            std::cout << "context.cpuRegs.GPR.r[" << rt_index << "].SD[0] = (s64)(s32)(imm << 16);" << std::endl;
            break;
        }
        case MIPS_INS_SLL: {
            std::cerr << "  Operand 0 [rd]: type=" << mips_details.operands[0].type << ", reg=" << mips_details.operands[0].reg << std::endl;
            std::cerr << "  Operand 1 [rt]: type=" << mips_details.operands[1].type << ", reg=" << mips_details.operands[1].reg << std::endl;
            std::cerr << "  Operand 2 [sa]: type=" << mips_details.operands[2].type << ", imm=" << mips_details.operands[2].imm << std::endl;
            const auto& rd_capstone = mips_details.operands[0].reg;
            const auto& rt_capstone = mips_details.operands[1].reg;
            const auto& sa = mips_details.operands[2].imm;
            int rd_index = get_gpr_index(rd_capstone);
            int rt_index = get_gpr_index(rt_capstone);

            std::cout << "context.cpuRegs.GPR.r[" << rd_index << "].SD[0] = (s64)(s32)((u32)context.cpuRegs.GPR.r[" << rt_index << "].UD[0] << " << sa << ");" << std::endl;
            break;
        }
        case MIPS_INS_NOP: {
            std::cout << "// NOP" << std::endl;
            break;
        }
        case MIPS_INS_ORI: {
            const auto& dest_reg = mips_details.operands[0].reg;
            const auto& source_reg = mips_details.operands[1].reg;
            const auto& imm = mips_details.operands[2].imm;

            int dest_index = get_gpr_index(dest_reg);
            int source_index = get_gpr_index(source_reg);

            std::cout << "context.cpuRegs.GPR.r[" << dest_index << "].UD[0] = context.cpuRegs.GPR.r["<< source_index << "].UD[0] | (u32)("<< imm << ");"<< std::endl;
            break;
        }
        case MIPS_INS_ADDU: {
            const auto& dest_reg = mips_details.operands[0].reg;
            const auto& reg1 = mips_details.operands[1].reg;
            const auto& reg2 = mips_details.operands[2].reg;

            int dest_index = get_gpr_index(dest_reg);
            int reg1_index = get_gpr_index(reg1);
            int reg2_index = get_gpr_index(reg2);

            std::cout << "context.cpuRegs.GPR.r[" << dest_index << "].SD[0] = (s64)(s32)(context.cpuRegs.GPR.r[" << reg1_index << "].SD[0] + context.cpuRegs.GPR.r["<< reg2_index << "].SD[0]);"<< std::endl;
            break;

        }
        case MIPS_INS_SUBU: {
            const auto& dest_reg = mips_details.operands[0].reg;
            const auto& reg1 = mips_details.operands[1].reg;
            const auto& reg2 = mips_details.operands[2].reg;

            int dest_index = get_gpr_index(dest_reg);
            int reg1_index = get_gpr_index(reg1);
            int reg2_index = get_gpr_index(reg2);

            std::cout << "context.cpuRegs.GPR.r[" << dest_index << "].SD[0] = (s64)(s32)(context.cpuRegs.GPR.r[" << reg1_index << "].SD[0] - context.cpuRegs.GPR.r["<< reg2_index << "].SD[0]);"<< std::endl;
            break;

        }
        case MIPS_INS_SLT: {
            const auto& dest_reg = mips_details.operands[0].reg;
            const auto& reg1 = mips_details.operands[1].reg;
            const auto& reg2 = mips_details.operands[2].reg;

            int dest_index = get_gpr_index(dest_reg);
            int reg1_index = get_gpr_index(reg1);
            int reg2_index = get_gpr_index(reg2);

            std::cout << "context.cpuRegs.GPR.r[" << dest_index << "].SD[0] = (s64)((s32)context.cpuRegs.GPR.r[" << reg1_index << "].SD[0] < (s32)context.cpuRegs.GPR.r["<< reg2_index << "].SD[0] ? 1 : 0);"<< std::endl;
            break;
        }
        case MIPS_INS_SLTI: {
            const auto& dest_reg = mips_details.operands[0].reg;
            const auto& source_reg = mips_details.operands[1].reg;
            const auto& imm = mips_details.operands[2].imm;

            int dest_index = get_gpr_index(dest_reg);
            int source_index = get_gpr_index(source_reg);

            std::cout << "context.cpuRegs.GPR.r[" << dest_index << "].SD[0] = (s64)((s32)context.cpuRegs.GPR.r[" << source_index << "].SD[0] < (s32)" << imm << " ? 1 : 0);"<< std::endl;
            break;
        }
        case MIPS_INS_MULT : {
            const auto& dest_reg = mips_details.operands[0].reg;
            const auto& source_reg = mips_details.operands[1].reg;

            int dest_index = get_gpr_index(dest_reg);
            int source_index = get_gpr_index(source_reg);

            /*
            {
                s32 op1 = context.cpuRegs.GPR.r[dest_index].SD[0];
                s32 op2 = context.cpuRegs.GPR.r[source_index].SD[0];
                s64 product = op1 * op2;
                context.cpuRegs.GPR.r[dest_index].LO.SD[0] = (s64)(s32)product;
                context.cpuRegs.GPR.r[dest_index].HI.SD[0] = (s64)(s32)(product >> 32);
            
            }
            */
           std::cout << "{" << std::endl;
           std::cout << "   s32 op1 = context.cpuRegs.GPR.r[ " << dest_index << "].SD[0];" << std::endl;
           std::cout << "   s32 op2 = context.cpuRegs.GPR.r[ " << source_index <<" ].SD[0];" << std::endl;
           std::cout << "   s64 product = op1 * op2;" << std::endl;
           std::cout << "   context.cpuRegs.LO.SD[0] = (s64)(s32)product;" << std::endl;
           std::cout << "   context.cpuRegs.HI.SD[0] = (s64)(s32)(product >> 32);" << std::endl;
           std::cout << "}" << std::endl;
           break;
        }
        case MIPS_INS_DIV : {
            const auto& dest_reg = mips_details.operands[0].reg;
            const auto& source_reg = mips_details.operands[1].reg;

            int dest_index = get_gpr_index(dest_reg);
            int source_index = get_gpr_index(source_reg);

            /*
            {
                s32 num = context.cpuRegs.GPR.r[dest_index].SD[0];
                s32 den = context.cpuRegs.GPR.r[source_index].SD[0];

                if (den != 0){
                    s32 HI_ans = num % den;
                    s32 LO_ans = num / den;
                    context.cpuRegs.LO.SD[0] = (s64)(s32)LO_ans;
                    context.cpuRegs.HI.SD[0] = (s64)(s32)(HI_ans);
                }
            
            }
            */
           std::cout << "{" << std::endl;
           std::cout << "   s64 num = context.cpuRegs.GPR.r[ " << dest_index << "].SD[0];" << std::endl;
           std::cout << "   s64 den = context.cpuRegs.GPR.r[ " << source_index <<" ].SD[0];" << std::endl;
           std::cout << "   if (den != 0){" << std::endl;
           std::cout << "       s32 HI_ans = num " << "%" << "den;"  <<std::endl;
           std::cout << "       s32 LO_ans = num / den;" << std::endl;
           std::cout << "       context.cpuRegs.LO.SD[0] = (s64)(s32)LO_ans;" << std::endl;
           std::cout << "       context.cpuRegs.HI.SD[0] = (s64)(s32)(HI_ans);" << std::endl;
           std::cout << "   }" << std::endl;
           std::cout << "}" << std::endl;
           break;
        }
        case MIPS_INS_XOR : {
            const auto& rd_reg = mips_details.operands[0].reg;
            const auto& rs_reg = mips_details.operands[1].reg;
            const auto& rt_reg = mips_details.operands[2].reg;

            int rd_index = get_gpr_index(rd_reg);
            int rs_index = get_gpr_index(rs_reg);
            int rt_index = get_gpr_index(rt_reg);

            /*
            {
                u64 rs = context.cpuRegs.GPR.r[rs_index].UD[0];
                u64 rt = context.cpuRegs.GPR.r[rt_index].UD[0];
                context.cpuRegs.GPT.r[rd_index].UD[0] = rs ^ rt;
            
            }
            */
           std::cout << "{" << std::endl;
           std::cout << "   u64 rs_val = context.cpuRegs.GPR.r[rs_index].UD[0];" << std::endl;
           std::cout << "   u64 rt_val = context.cpuRegs.GPR.r[rt_index].UD[0];" << std::endl;
           std::cout << "   context.cpuRegs.GPR.r[" << rd_index << "].UD[0] = rs_val ^ rt_val;" << std::endl;
           std::cout << "}" << std::endl;
           break;
        }
        case MIPS_INS_NOR : {
            const auto& rd_reg = mips_details.operands[0].reg;
            const auto& rs_reg = mips_details.operands[1].reg;
            const auto& rt_reg = mips_details.operands[2].reg;

            int rd_index = get_gpr_index(rd_reg);
            int rs_index = get_gpr_index(rs_reg);
            int rt_index = get_gpr_index(rt_reg);

            /*
            {
                u64 rs = context.cpuRegs.GPR.r[rs_index].UD[0];
                u64 rt = context.cpuRegs.GPR.r[rt_index].UD[0];
                context.cpuRegs.GPT.r[rd_index].UD[0] = ~(rs | rt);
            
            }
            */
           std::cout << "{" << std::endl;
           std::cout << "   u64 rs_val = context.cpuRegs.GPR.r[rs_index].UD[0];" << std::endl;
           std::cout << "   u64 rt_val = context.cpuRegs.GPR.r[rt_index].UD[0];" << std::endl;
           std::cout << "   context.cpuRegs.GPR.r[" << rd_index << "].UD[0] = ~(rs_val | rt_val);" << std::endl;
           std::cout << "}" << std::endl;
           break;
        }
        case MIPS_INS_SRL : {
            const auto& rd_reg = mips_details.operands[0].reg;
            const auto& rt_reg = mips_details.operands[1].reg;
            const auto& imm = mips_details.operands[2].imm;

            int rd_index = get_gpr_index(rd_reg);
            int rt_index = get_gpr_index(rt_reg);

            /*
            {

                u64 rt_val = context.cpuRegs.GPR.r[rt_index].UD[0];
                context.cpuRegs.GPT.r[rd_index].UD[0] = u64(rt_val >> imm);
            
            }
            */
           std::cout << "{" << std::endl;
           std::cout << "   u64 rt_val = context.cpuRegs.GPR.r[rt_index].UD[0];" << std::endl;
           std::cout << "   context.cpuRegs.GPR.r[" << rd_index << "].UD[0] = (u64)((u32)rt_val >>" << imm << ");" << std::endl;
           std::cout << "}" << std::endl;
           break;
        }
        case MIPS_INS_SRA : {
            const auto& rd_reg = mips_details.operands[0].reg;
            const auto& rt_reg = mips_details.operands[1].reg;
            const auto& imm = mips_details.operands[2].imm;

            int rd_index = get_gpr_index(rd_reg);
            int rt_index = get_gpr_index(rt_reg);

            /*
            {

                u64 rt_val = context.cpuRegs.GPR.r[rt_index].UD[0];
                context.cpuRegs.GPT.r[rd_index].SD[0] = (s64)(s32)((u32)(rt_val) >> imm);
            
            }
            */
           std::cout << "{" << std::endl;
           std::cout << "   s64 rt_val = context.cpuRegs.GPR.r[rt_index].UD[0];" << std::endl;
           std::cout << "   context.cpuRegs.GPR.r[" << rd_index << "].SD[0] = (s64)((s32)(rt_val) >>" << imm << ");" << std::endl;
           std::cout << "}" << std::endl;
           break;
        }
        case MIPS_INS_LB : {
            std::cerr << "  Operand 0 [dest]: type=" << mips_details.operands[0].type << ", reg=" << mips_details.operands[0].reg << std::endl;
            std::cerr << "  Operand 1 [mem]: type=" << mips_details.operands[1].type << ", base=" << mips_details.operands[1].mem.base << ", disp=" << mips_details.operands[1].mem.disp << std::endl;
            const auto& dest_capstone = mips_details.operands[0].reg;
            const auto& base_capstone = mips_details.operands[1].mem.base;
            const auto& offset = mips_details.operands[1].mem.disp;
            int dest_index = get_gpr_index(dest_capstone);
            int base_index = get_gpr_index(base_capstone);

            std::cout << "{" << std::endl;
            std::cout << "    u32 address = context.cpuRegs.GPR.r[" << base_index << "].UD[0] + " << offset << ";" << std::endl;
            std::cout << "    context.cpuRegs.GPR.r[" << dest_index << "].SD[0] = (s64)(s32)ReadMemory8(address);" << std::endl;
            std::cout << "}" << std::endl;
            break;
        }
        case MIPS_INS_LBU : {
            std::cerr << "  Operand 0 [dest]: type=" << mips_details.operands[0].type << ", reg=" << mips_details.operands[0].reg << std::endl;
            std::cerr << "  Operand 1 [mem]: type=" << mips_details.operands[1].type << ", base=" << mips_details.operands[1].mem.base << ", disp=" << mips_details.operands[1].mem.disp << std::endl;
            const auto& dest_capstone = mips_details.operands[0].reg;
            const auto& base_capstone = mips_details.operands[1].mem.base;
            const auto& offset = mips_details.operands[1].mem.disp;
            int dest_index = get_gpr_index(dest_capstone);
            int base_index = get_gpr_index(base_capstone);

            std::cout << "{" << std::endl;
            std::cout << "    u32 address = context.cpuRegs.GPR.r[" << base_index << "].UD[0] + " << offset << ";" << std::endl;
            std::cout << "    context.cpuRegs.GPR.r[" << dest_index << "].UD[0] = (u64)(u32)ReadMemory8(address);" << std::endl;
            std::cout << "}" << std::endl;
            break;
        }
        case MIPS_INS_LH: {
            const auto& rt_capstone = current_insn.detail->mips.operands[0].reg;
            const auto& base_capstone = current_insn.detail->mips.operands[1].mem.base;
            const auto& offset = current_insn.detail->mips.operands[1].mem.disp; // Capstone gives disp for offset(base)
        
            int rt_index = get_gpr_index(rt_capstone);
            int base_index = get_gpr_index(base_capstone);
        
            // Debug prints (optional)
            std::cerr << "  Operand 0 [rt]: type=" << mips_details.operands[0].type << ", reg=" << mips_details.operands[0].reg << std::endl;
            std::cerr << "  Operand 1 [mem]: type=" << mips_details.operands[1].type << ", base=" << mips_details.operands[1].mem.base << ", disp=" << mips_details.operands[1].mem.disp << std::endl;
        
            std::cout << "{" << std::endl;
            std::cout << "    u32 address = context.cpuRegs.GPR.r[" << base_index << "].UD[0] + " << offset << ";" << std::endl;
        
            // Alignment Check: Address must be 2-byte aligned (address % 2 == 0)
            std::cout << "    if (address % 2 != 0) {" << std::endl;
            std::cout << "        std::cerr << \"FATAL ERROR: Unaligned memory access for LH at address: 0x\" << std::hex << address << std::endl;" << std::endl;
            std::cout << "        exit(1);" << std::endl;
            std::cout << "    }" << std::endl;
        
            // Read the 16-bit value and cast it to a signed 16-bit integer (s16)
            std::cout << "    s16 value = (s16)ReadMemory16(address);" << std::endl;
            // Assign the signed 16-bit value to the signed 64-bit register. C++ handles the sign extension.
            std::cout << "    context.cpuRegs.GPR.r[" << rt_index << "].SD[0] = (s64)value;" << std::endl;
            std::cout << "}" << std::endl;
            break;
        }
        case MIPS_INS_LHU : {
            const auto& rt_capstone = current_insn.detail->mips.operands[0].reg;
            const auto& base_capstone = current_insn.detail->mips.operands[1].mem.base;
            const auto& offset = current_insn.detail->mips.operands[1].mem.disp; // Capstone gives disp for offset(base)
        
            int rt_index = get_gpr_index(rt_capstone);
            int base_index = get_gpr_index(base_capstone);
        
            // Debug prints (optional)
            std::cerr << "  Operand 0 [rt]: type=" << mips_details.operands[0].type << ", reg=" << mips_details.operands[0].reg << std::endl;
            std::cerr << "  Operand 1 [mem]: type=" << mips_details.operands[1].type << ", base=" << mips_details.operands[1].mem.base << ", disp=" << mips_details.operands[1].mem.disp << std::endl;
        
            std::cout << "{" << std::endl;
            std::cout << "    u32 address = context.cpuRegs.GPR.r[" << base_index << "].UD[0] + " << offset << ";" << std::endl;
        
            // Alignment Check: Address must be 2-byte aligned (address % 2 == 0)
            std::cout << "    if (address % 2 != 0) {" << std::endl;
            std::cout << "        std::cerr << \"FATAL ERROR: Unaligned memory access for LH at address: 0x\" << std::hex << address << std::endl;" << std::endl;
            std::cout << "        exit(1);" << std::endl;
            std::cout << "    }" << std::endl;
        
            // Read the 16-bit value and cast it to a signed 16-bit integer (s16)
            std::cout << "    u16 value = ReadMemory16(address);" << std::endl;
            // Assign the signed 16-bit value to the signed 64-bit register. C++ handles the sign extension.
            std::cout << "    context.cpuRegs.GPR.r[" << rt_index << "].UD[0] = (u64)value;" << std::endl;
            std::cout << "}" << std::endl;
            break;
        }
        case MIPS_INS_SB : {
            std::cerr << "  Operand 0 [source]: type=" << mips_details.operands[0].type << ", reg=" << mips_details.operands[0].reg << std::endl;
            std::cerr << "  Operand 1 [mem]: type=" << mips_details.operands[1].type << ", base=" << mips_details.operands[1].mem.base << ", disp=" << mips_details.operands[1].mem.disp << std::endl;
            const auto& source_capstone = mips_details.operands[0].reg;
            const auto& base_capstone = mips_details.operands[1].mem.base;
            const auto& offset = mips_details.operands[1].mem.disp;
            int source_index = get_gpr_index(source_capstone);
            int base_index = get_gpr_index(base_capstone);

            std::cout << "{" << std::endl;
            std::cout << "    u32 address = context.cpuRegs.GPR.r[" << base_index << "].UD[0] + " << offset << ";" << std::endl;
            std::cout << "    WriteMemory8(address, (u8)context.cpuRegs.GPR.r[" << source_index << "].UD[0]);" << std::endl;
            std::cout << "}" << std::endl;
            break;
        }
        case MIPS_INS_SH : {
            std::cerr << "  Operand 0 [source]: type=" << mips_details.operands[0].type << ", reg=" << mips_details.operands[0].reg << std::endl;
            std::cerr << "  Operand 1 [mem]: type=" << mips_details.operands[1].type << ", base=" << mips_details.operands[1].mem.base << ", disp=" << mips_details.operands[1].mem.disp << std::endl;
            const auto& source_capstone = mips_details.operands[0].reg;
            const auto& base_capstone = mips_details.operands[1].mem.base;
            const auto& offset = mips_details.operands[1].mem.disp;
            int source_index = get_gpr_index(source_capstone);
            int base_index = get_gpr_index(base_capstone);

            std::cout << "{" << std::endl;
            std::cout << "    u32 address = context.cpuRegs.GPR.r[" << base_index << "].UD[0] + " << offset << ";" << std::endl;
        
            // Alignment Check: Address must be 2-byte aligned (address % 2 == 0)
            std::cout << "    if (address % 2 != 0) {" << std::endl;
            std::cout << "        std::cerr << \"FATAL ERROR: Unaligned memory access for LH at address: 0x\" << std::hex << address << std::endl;" << std::endl;
            std::cout << "        exit(1);" << std::endl;
            std::cout << "    }" << std::endl;
            std::cout << "    WriteMemory16(address, (u16)context.cpuRegs.GPR.r[" << source_index << "].UD[0]);" << std::endl;
            std::cout << "}" << std::endl;
            break;
        }
        case MIPS_INS_BGTZ : {
            std::cerr << "  Operand 0 [rs]: type=" << mips_details.operands[0].type << ", reg=" << mips_details.operands[0].reg << std::endl;
            std::cerr << "  Operand 2 [offset]: type=" << mips_details.operands[1].type << ", imm=" << mips_details.operands[2].imm << std::endl;
            const auto& rs_capstone = mips_details.operands[0].reg;
            const auto& off = mips_details.operands[1].imm;
            int rs_index = get_gpr_index(rs_capstone);

            if (i + 1 < total_count) {
                translate_instruction_block(insn, i + 1, total_count);
            }

            std::cout << "if ((s64)context.cpuRegs.GPR.r[" << rs_index << "].SD[0] > 0) {" << std::endl;
            std::cout << "    context.cpuRegs.pc = " << current_insn.address << " + 4 + (" << off << " << 2);" << std::endl;
            std::cout << "} else {" << std::endl;
            std::cout << "    context.cpuRegs.pc = " << current_insn.address << " + 8;" << std::endl;
            std::cout << "}" << std::endl;
            return 2;
        }
        case MIPS_INS_BLEZ : {
            std::cerr << "  Operand 0 [rs]: type=" << mips_details.operands[0].type << ", reg=" << mips_details.operands[0].reg << std::endl;
            std::cerr << "  Operand 2 [offset]: type=" << mips_details.operands[1].type << ", imm=" << mips_details.operands[2].imm << std::endl;
            const auto& rs_capstone = mips_details.operands[0].reg;
            const auto& off = mips_details.operands[1].imm;
            int rs_index = get_gpr_index(rs_capstone);

            if (i + 1 < total_count) {
                translate_instruction_block(insn, i + 1, total_count);
            }

            std::cout << "if ((s64)context.cpuRegs.GPR.r[" << rs_index << "].SD[0] <= 0) {" << std::endl;
            std::cout << "    context.cpuRegs.pc = " << current_insn.address << " + 4 + (" << off << " << 2);" << std::endl;
            std::cout << "} else {" << std::endl;
            std::cout << "    context.cpuRegs.pc = " << current_insn.address << " + 8;" << std::endl;
            std::cout << "}" << std::endl;
            return 2;
        }
        case MIPS_INS_JAL : {
            std::cerr << "  Operand 0 [target]: type=" << mips_details.operands[0].type << ", reg=" << mips_details.operands[0].reg << std::endl;
            const auto& target_imm = mips_details.operands[0].imm;

            if (i + 1 < total_count) {
                translate_instruction_block(insn, i + 1, total_count);
            }
            std::cout << "context.cpuRegs.GPR.r[31] = " << current_insn.address << "+ 8;" << std::endl;
            /*                      0xFFFFFFFF                              0x02FFFFFF
                                    0xF0000000                              0x0FFFFFF0


            context.cpuRegs.pc = (current_insn.address & 0xF0000000) | (target_reg_index << 2);
            */
            std::cout << "context.cpuRegs.pc = (" << current_insn.address << "& 0xF0000000) | (" << target_imm << " << 2);" << std::endl;
            return 2;
        }
        case MIPS_INS_J : {}
        case MIPS_INS_JALR : {}
        case MIPS_INS_SYSCALL : {}
        case MIPS_INS_MFC0 : {}
        case MIPS_INS_MTC0 : {}
        default:
            std::cout << "// Unhandled instruction: " << current_insn.mnemonic << std::endl;
            break;
    }

    // Default case: we consumed one instruction.
    return 1;
}
