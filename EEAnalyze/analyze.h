#pragma once

#include <vector>
#include <cstdint>
#include <capstone/capstone.h> // Include Capstone header

// The main structure to hold the results of a function analysis.
// It owns all the basic blocks that belong to it.
struct Function {
    // A basic block is a sequence of instructions that has
    // a single entry point and a single exit point.
    struct Block {
        uint64_t base; // The starting address of the block, relative to the function's base.
        uint64_t size; // The size of the block in bytes.

        // Default constructor
        Block() : base(0), size(0) {}

        // Constructor to initialize with a base and size
        Block(uint64_t base_offset, uint64_t block_size)
            : base(base_offset), size(block_size) {}
    };

    uint64_t base_address; // The absolute starting address of the function in memory.
    uint64_t size;         // The total size of the function, calculated after analysis.
    
    // The vector that holds all the actual Block objects.
    std::vector<Block> blocks;

    // Default constructor
    Function() : base_address(0), size(0) {}

    // Constructor to initialize with a base address
    Function(uint64_t base) : base_address(base), size(0) {}

    // A helper function to find if a block containing a given address already exists.
    // Returns the index of the block, or -1 if not found.
    int search_block(uint64_t absolute_address) const;
};

// The main analysis function declaration.
// It requires a handle to an initialized Capstone instance.
Function analyze_mips_func(csh capstone_handle, const void* code, size_t size, uint64_t base);
