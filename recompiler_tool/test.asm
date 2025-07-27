.globl _start
.set noreorder # Crucial for manual delay slot handling

_start:
    # Initialize some registers
    # $t0 (reg 8)
    # $t1 (reg 9)
    # $t2 (reg 10)
    # $s0 (reg 16)
    # $s1 (reg 17)
    # $sp (reg 29)
    # $ra (reg 31)

    # --- Setup Initial Values ---
    lui $t0, 0x1234       # $t0 = 0x12340000
    ori $t0, $t0, 0x5678  # $t0 = 0x12345678 (Test ORI)

    lui $t1, 0x0000       # $t1 = 0x00000000
    ori $t1, $t1, 0x0001  # $t1 = 0x00000001 (for division/comparison)

    lui $t2, 0xFFFF       # $t2 = 0xFFFF0000
    ori $t2, $t2, 0xFFFF  # $t2 = 0xFFFFFFFF (signed -1)

    # Setup stack pointer for memory ops
    addiu $sp, $sp, -32   # Allocate some stack space

    # --- Arithmetic Operations ---
    addu $s0, $t0, $t1    # $s0 = $t0 + $t1 (0x12345678 + 1 = 0x12345679)
    subu $s1, $t0, $t1    # $s1 = $t0 - $t1 (0x12345678 - 1 = 0x12345677)

    # SLT (Set on Less Than)
    slt $t3, $t1, $t0     # $t3 = 1 (1 < 0x12345678)
    slt $t4, $t0, $t1     # $t4 = 0 (0x12345678 < 1 is false)
    slt $t5, $t2, $t1     # $t5 = 1 (-1 < 1)

    # MULT (Multiply)
    # $t0 = 0x12345678, $t1 = 1
    mult $t0, $t1         # HI:LO = $t0 * $t1 (0x12345678)
    nop                   # Delay slot for MULT (assembler might insert, but good to be explicit)

    # DIV (Divide)
    # $t0 = 0x12345678, $t1 = 1
    div $t0, $t1          # LO = $t0 / $t1, HI = $t0 % $t1
    nop                   # Delay slot for DIV

    # --- Logical Operations ---
    # $t0 = 0x12345678, $t1 = 0x00000001
    or $s2, $t0, $t1      # $s2 = $t0 | $t1 (0x12345679)
    and $s3, $t0, $t1     # $s3 = $t0 & $t1 (0x00000000)
    xor $s4, $t0, $t1     # $s4 = $t0 ^ $t1 (0x12345679)
    nor $s5, $t0, $t1     # $s5 = ~($t0 | $t1) (0xEDCBA986)

    # --- Shift Operations ---
    # $t0 = 0x12345678
    sll $s6, $t0, 4       # $s6 = $t0 << 4 (0x23456780)
    srl $s7, $t0, 4       # $s7 = $t0 >> 4 (logical, 0x01234567)
    sra $t8, $t0, 4       # $t8 = $t0 >> 4 (arithmetic, 0x01234567)
    sra $t9, $t2, 4       # $t9 = $t2 >> 4 (arithmetic, 0xFFFFFFFF >> 4 = 0xFFFFFFFF)

    # --- Memory Access Operations ---
    # $t0 = 0x12345678
    sw $t0, 0($sp)        # Store $t0 (0x12345678) at $sp+0
    nop                   # Delay slot for SW

    sh $t0, 4($sp)        # Store lower 16 bits of $t0 (0x5678) at $sp+4
    nop                   # Delay slot for SH

    sb $t0, 6($sp)        # Store lowest 8 bits of $t0 (0x78) at $sp+6
    nop                   # Delay slot for SB

    lw $s0, 0($sp)        # Load word from $sp+0 into $s0 (should be 0x12345678)
    nop                   # Delay slot for LW

    lh $s1, 4($sp)        # Load halfword from $sp+4 into $s1 (should be 0x5678, sign-extended)
    nop                   # Delay slot for LH

    lb $s2, 6($sp)        # Load byte from $sp+6 into $s2 (should be 0x78, sign-extended)
    nop                   # Delay slot for LB

    # --- Branch Operations ---
    # $t0 = 0x12345678, $t1 = 0x00000001, $t2 = 0xFFFFFFFF (-1)

    # BEQ (Branch if Equal)
    beq $t0, $t0, branch_target_1 # Should branch
    nop                           # Delay slot
    j end_of_branches             # Should NOT be reached
    nop

branch_target_1:
    # BNE (Branch if Not Equal)
    bne $t0, $t1, branch_target_2 # Should branch
    nop                           # Delay slot
    j end_of_branches             # Should NOT be reached
    nop

branch_target_2:
    # BGTZ (Branch if Greater Than Zero)
    bgtz $t0, branch_target_3     # Should branch (0x12345678 > 0)
    nop                           # Delay slot
    j end_of_branches             # Should NOT be reached
    nop

branch_target_3:
    # BLEZ (Branch if Less Than or Equal to Zero)
    blez $t2, branch_target_4     # Should branch (-1 <= 0)
    nop                           # Delay slot
    j end_of_branches             # Should NOT be reached
    nop

branch_target_4:
    # --- Jump Operations ---
    # JAL (Jump and Link)
    jal function_call_target      # Should jump to function_call_target
    nop                           # Delay slot
    # $ra should now hold the address of 'after_jal_return'

after_jal_return:
    # JR (Jump Register) - used for function return
    jr $ra                        # Should jump back to 'end_of_test'
    nop                           # Delay slot

function_call_target:
    # This is a dummy function.
    # It will return to $ra (which holds 'after_jal_return')
    jr $ra
    nop

end_of_branches:
    # This label is a fallback for branches that shouldn't be taken.
    # It will be skipped if branches work correctly.
    nop

end_of_test:
    # End of our test program.
    # In a real scenario, this might be a loop or a syscall to exit.
    nop