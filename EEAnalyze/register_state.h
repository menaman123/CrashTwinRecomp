#include <cstdint>
#include "generated/Registers_enums.h"

// The different kinds of states a register can be in
enum class StateType {
    UNKNOWN,          // Default state
    CONSTANT,         // A known 32-bit or 64-bit integer value
    SYMBOLIC,         // A copy of another register's value (e.g., from a 'move' instruction)
    STACK_POINTER_RELATIVE // An offset from the initial stack pointer
};

// A class to hold the details of the state
class RegisterState {
public:
    StateType type = StateType::UNKNOWN;

    // Use a union to save space, since a state can only be one type at a time
    union {
        uint64_t constantValue;
        RabbitizerRegister_GprO32 sourceRegister;
        int32_t stackOffset;
    } value;

    // Helper constructors to make creating states easier
    RegisterState() : type(StateType::UNKNOWN) {}

    static RegisterState asConstant(uint64_t val) {
        RegisterState state;
        state.type = StateType::CONSTANT;
        state.value.constantValue = val;
        return state;
    }

    static RegisterState asSymbolic(RabbitizerRegister_GprO32 reg) {
        RegisterState state;
        state.type = StateType::SYMBOLIC;
        state.value.sourceRegister = reg;
        return state;
    }

    static RegisterState asStackRelative(int32_t offset) {
        RegisterState state;
        state.type = StateType::STACK_POINTER_RELATIVE;
        state.value.stackOffset = offset;
        return state;
    }
};