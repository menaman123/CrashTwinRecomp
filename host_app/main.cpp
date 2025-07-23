

#include <iostream>
#include "cpu_state.h"
#include "recomp_test.h"

int main() {
    // 1. Create an instance of the Emotion Engine CPU state.
    EmotionEngineState cpu_context = {}; // Zero-initialize all registers.

    std::cout << "--- Before ---" << std::endl;
    // The register r2 ($v0) is used for function return values.
    // We print its value before calling the function.
    std::cout << "Return value register ($v0 / r2): " << cpu_context.cpuRegs.GPR.r[2].UD[0] << std::endl;

    // 2. Call our manually recompiled test function.
    // This function will simulate the behavior of a simple PS2 function.
    GetPlayerMaxHealth_recomp(cpu_context);

    std::cout << "\n--- After ---" << std::endl;
    // 3. Print the value of the register again to verify it was changed.
    std::cout << "Return value register ($v0 / r2): " << cpu_context.cpuRegs.GPR.r[2].UD[0] << std::endl;

    // 4. Check the result.
    if (cpu_context.cpuRegs.GPR.r[2].UD[0] == 100) {
        std::cout << "\nSuccess! The recompiled function correctly modified the CPU state." << std::endl;
    } else {
        std::cout << "\nFailure. The recompiled function did not work as expected." << std::endl;
    }

    return 0;
}

