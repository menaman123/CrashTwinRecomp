

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
            switch(insn[i].id){
                case MIPS_INS_ADDIU: {
                    // destination = source reg + immediate value
                    // Gets that assembly line INSTRUCTION DEST REG1 REG2 
                    const auto& mips_details = insn[i].detail->mips;
                    
                    // Dest reg
                    const auto& dest_operand = mips_insn.operand[0];

                    // REG1
                    const auto& reg1_operand = mips_insn.operand[1];

                    // REG2 but since it is ADDIU we want immediate value instead of reg2
                    const auto& imm_operand = mips_insn.operand[2];

                    int dest_val = dest_operand.reg;
                    int reg1_val = reg1_operand.reg;
                    long long imm_val = imm_operand.imm;

                    // Now we parse this into a string
                    /*
                    context.cpuRegs.GPR.r[ dest_reg ].UD[0] = context.cpuRegs.GPR.r[ reg_1 ].UD[0] + context.cpuRegs.GPR.r[ imm ].UD[0]
                    */

                    std::count << "context.cpuRegs.GPR.r[" << dest_val << "].UD[0] = context.cpuRegs.GPR.r[" << reg1_val << "].UD[0] + " << imm_val << ";" std::endl;

            }

            // Example of how to access instruction details:
            const auto& current_insn = insn[i];

            std::cout << "// Unhandled Instruction at 0x" << std::hex << current_insn.address
                      << ": " << current_insn.mnemonic << " " << current_insn.op_str << std::endl;

            // ==================================================
        }

        cs_free(insn, count);
    } else {
        std::cerr << "ERROR: Failed to disassemble any code!" << std::endl;
        return -1;
    }

    cs_close(&handle);

    return 0;
}

