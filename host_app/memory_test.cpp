
#include "gtest/gtest.h"
#include "memory.h" // Include the header for the code we are testing

// This is a test fixture, a basic unit test in Google Test.
// It's named in a "TestSuitName_TestName" format.
TEST(MemoryTest, ReadWrite32) {
    // 1. Arrange: Define a test value and a test address.
    // Using a value with different bytes helps ensure endianness is correct.
    const u32 test_value = 0xABCD1234;
    const u32 test_address = 0x100; // An arbitrary address in memory

    // 2. Act: Write the value to memory, then immediately read it back.
    WriteMemory32(test_address, test_value);
    u32 read_back_value = ReadMemory32(test_address);

    // 3. Assert: Check if the value we got back is the same as the one we wrote.
    // EXPECT_EQ is a Google Test macro that checks for equality.
    // If they are not equal, the test will fail and print a helpful message.
    EXPECT_EQ(read_back_value, test_value);
}

// TODO: Add another test for a different address and value to be thorough.
// TEST(MemoryTest, ReadWrite32_EdgeCase) { ... }

TEST(MemoryDeathTest, WriteOutOfBounds){
    const u32 out_of_bounds_memory = main_memory.size() - 3;
    const u32 test_value = 0xABCD1234;

    ASSERT_DEATH({
        WriteMemory32(out_of_bounds_memory, test_value);
    }, "Out-of-bounds memory write");
}

TEST(MemoryDeathTest, ReadOutOfBounds){
    const u32 out_of_bounds_memory = main_memory.size() - 3;

    ASSERT_DEATH({
        ReadMemory32(out_of_bounds_memory);
    }, "Out-of-bounds memory write");
}

// TODO: Once you implement WriteMemory16 and ReadMemory16, add a test for them.
// TEST(MemoryTest, ReadWrite16) { ... }

