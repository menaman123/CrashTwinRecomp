
#!/bin/bash

# This script automates the functional testing of the recompiler tool.

# Exit immediately if a command exits with a non-zero status.
set -e

# --- Configuration ---
RECOMPILER_EXE="./build/recompiler_tool"
ASSEMBLER="mips-linux-gnu-as"
LINKER="mips-linux-gnu-ld"
OBJCOPY="mips-linux-gnu-objcopy"

# --- Step 1: Assemble the MIPS test file with little-endian ---
echo "[TEST] Assembling MIPS test binary (little-endian)..."
$ASSEMBLER -EL -o test.o test.asm        # Forces little-endian assembly
$LINKER -EL -Ttext=0x1000 -o test.elf test.o  # Forces little-endian linking
$OBJCOPY -O binary -j .text test.elf test.bin
echo "[TEST] Assembly complete."

# --- Step 2: Generate the Golden File (if it doesn't exist) ---
if [ ! -f recompiler_output.golden.cpp ]; then
    echo "[TEST] Golden file not found. Creating one from current recompiler output..."
    $RECOMPILER_EXE test.bin > recompiler_output.golden.cpp
    echo "[TEST] Golden file created. Please inspect it manually to ensure it is correct."
    exit 0
fi

# --- Step 3: Run the recompiler to get the current output ---
echo "[TEST] Running recompiler to get current output..."
$RECOMPILER_EXE test.bin > recompiler_output.current.cpp

# --- Step 4: Compare the current output with the golden file ---
echo "[TEST] Comparing current output with golden file..."
diff recompiler_output.golden.cpp recompiler_output.current.cpp

# --- Step 5: Report result ---
# The `set -e` at the top means the script will exit automatically if `diff` finds a difference.
# If we reach this line, it means `diff` found no differences.
echo "✅ Recompiler test PASSED"

# Clean up intermediate files
rm test.o test.elf test.bin recompiler_output.current.cpp

