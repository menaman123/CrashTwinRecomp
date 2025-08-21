#include <capstone/capstone.h>
#include <elfio/elfio.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <iomanip>

// This function takes a blob of bytes and prints the disassembled MIPS instructions.
static void disassemble_and_print(const uint8_t* code, size_t size, uint64_t base_address, csh handle) {
    cs_insn* insn = cs_malloc(handle);
    if (!insn) {
        std::cerr << "[-] ERROR: Capstone failed to allocate instruction memory.\\n";
        return;
    }

    // Manually loop through the entire section, 4 bytes at a time.
    for (size_t offset = 0; offset < size; offset += 4) {
        const uint8_t* current_code_ptr = code + offset;
        size_t remaining_size = 4; // We only want to process one instruction at a time.
        uint64_t current_address = base_address + offset;

        // Try to disassemble just this single 4-byte instruction
        if (cs_disasm_iter(handle, &current_code_ptr, &remaining_size, &current_address, insn)) {
            // Success
            std::cout << "0x" << std::hex << insn->address
                      << ":\\t" << std::setw(8) << std::left << insn->mnemonic
                      << insn->op_str << std::dec << std::endl;
        } else {
            // Failure - print the raw data so we can see what it is
            uint32_t raw_data = *(reinterpret_cast<const uint32_t*>(code + offset));
            std::cout << "0x" << std::hex << (base_address + offset)
                      << ":\\t.word   0x" << std::setw(8) << std::setfill('0') << raw_data
                      << std::dec << std::setfill(' ') << "  // <invalid instruction>" << std::endl;
        }
    }

    cs_free(insn, 1); // Free the single instruction structure.
}


int main(int argc, char** argv) {
    // Use the first command-line argument as the ELF file path.
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <path_to_elf_file>\n";
        return 1;
    }
    std::string filePath = argv[1];

    // Load the ELF file using the ELFIO library.
    ELFIO::elfio reader;
    if (!reader.load(filePath)) {
        std::cerr << "[-] Could not load ELF file: " << filePath << "\n";
        return 1;
    }

    // Initialize the Capstone disassembler for MIPS (32-bit, little-endian).
    csh handle;
    if (cs_open(CS_ARCH_MIPS, (cs_mode)(CS_MODE_MIPS64 | CS_MODE_LITTLE_ENDIAN), &handle) != CS_ERR_OK) {
        std::cerr << "[-] Failed to initialize Capstone\n";
        return 1;
    }
    
    // We don't need detailed instruction info for this simple disassembly.
    cs_option(handle, CS_OPT_DETAIL, CS_OPT_OFF);

    bool found_code = false;

    // Iterate through all sections in the ELF file.
    for (ELFIO::Elf_Half i = 0; i < reader.sections.size(); ++i) {
        const ELFIO::section* sec = reader.sections[i];

        // Check if the section contains executable instructions.

        const char* data = sec->get_data();

        const uint8_t* code = reinterpret_cast<const uint8_t*>(data);
        ELFIO::Elf_Xword size = sec->get_size();
        ELFIO::Elf64_Addr addr = sec->get_address();

        std::cout << "\n// Disassembling section: '" << sec->get_name()
                  << "' at 0x" << std::hex << addr
                  << " (Size: " << std::dec << size << " bytes)\n";
        
        disassemble_and_print(code, static_cast<size_t>(size), static_cast<uint64_t>(addr), handle);
        found_code = true;
    }

    if (!found_code) {
        std::cerr << "[-] No executable sections (.text) found in the ELF file.\n";
    }

    // Clean up Capstone.
    cs_close(&handle);
    return found_code ? 0 : 2;
}
