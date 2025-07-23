
# Crash TwinSanity Recompilation Project

This repository is an attempt to statically recompile the PlayStation 2 game *Crash Twinsanity* to run natively on PC. The project is in its very early stages and is following a phased development roadmap.

---

## Phase 1: The "Hello, World" of Recompilation (Complete)

This initial phase has been successfully completed. The goal of this phase was to prove that the core concept of recompilation is viable for this project before investing significant time in building complex tools.

### What We Did

We built a minimal C++ application (`host_app`) that successfully simulates the execution of a single, manually recompiled function from a hypothetical PS2 game.

This involved three key steps:

1.  **Defining the CPU State:** We created a C++ header (`host_app/cpu_state.h`) that defines the essential registers of the PlayStation 2's main processor, the **Emotion Engine**.

2.  **Creating a Host Application:** We wrote a `main.cpp` file that initializes a representation of the CPU's state and calls our test function.

3.  **Manual Recompilation:** We wrote a C++ function that directly mimics the behavior of a simple MIPS assembly function. This function modifies the CPU state that was created in the host application.

### Key Concepts & Files

#### `EmotionEngineState`

This is the most important data structure in the project so far. It is a C++ `struct` that represents the entire state of the PS2's main CPU at any given moment.

-   **What it contains:** It holds all of the registers that a program running on the PS2 can access, including:
    -   `GPR` (General-Purpose Registers): The main registers used for integer math, logic, and memory addressing (`$v0`, `$a0`, `$sp`, etc.).
    -   `pc` (Program Counter): The register that holds the memory address of the next instruction to execute.
    -   `FPU Registers`: For floating-point math.
    -   `CP0 Registers`: System control registers for memory management and exceptions.
-   **Where it's defined:** `host_app/cpu_state.h`
-   **Why it's important:** All recompiled game code will be written as C++ functions that take a reference to an `EmotionEngineState` object. Modifying this object in C++ is the direct equivalent of an assembly instruction modifying the physical registers on the PS2.

#### The Recompilation Proof

We demonstrated the concept in `host_app/recomp_test.cpp`.

**The Goal:** To translate this simple MIPS assembly function into C++:

```mips
# MIPS Assembly
li $v0, 100      # Load the number 100 into the return value register ($v0)
jr $ra           # Return from the function
```

**The Result:** This was manually translated into the following C++ function:

```cpp
// C++ Recompilation in recomp_test.cpp
void GetPlayerMaxHealth_recomp(EmotionEngineState& context) {
    // This C++ line is the equivalent of `li $v0, 100`
    context.cpuRegs.GPR.r[2].UD[0] = 100;
}
```

When we ran the `host_app` executable, it successfully showed that the value of the `$v0` register (which is `GPR.r[2]`) was changed from `0` to `100`, proving the concept works.

### Next Steps

The project is now ready to proceed to **Phase 2: Building the Recompiler Tool**. This will involve creating a program that can automatically read PS2 assembly instructions and translate them into the equivalent C++ code, automating the manual process we just completed.
