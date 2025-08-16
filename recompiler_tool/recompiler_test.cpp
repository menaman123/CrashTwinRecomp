
#include "gtest/gtest.h"
#include "recompiler.h"

// Test suite for the analysis helper functions
TEST(AnalysisHelpers, IsBranchLikely) {
    // Create a fake instruction that IS a likely branch
    cs_insn beql_instruction;
    beql_instruction.id = MIPS_INS_BEQL;

    // Create another fake instruction that IS a likely branch
    cs_insn bgezal_instruction;
    bgezal_instruction.id = MIPS_INS_BGEZALL;

    // Create a fake instruction that IS NOT a likely branch
    cs_insn addiu_instruction;
    addiu_instruction.id = MIPS_INS_ADDIU;

    // Use assertions to check the function's behavior
    EXPECT_TRUE(is_branch_likely(beql_instruction));
    EXPECT_TRUE(is_branch_likely(bgezal_instruction));
    EXPECT_FALSE(is_branch_likely(addiu_instruction));
}
