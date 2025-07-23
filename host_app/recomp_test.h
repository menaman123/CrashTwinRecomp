
#pragma once

#include "cpu_state.h"

// This is the declaration for our manually recompiled function.
// It takes a reference to the CPU state and will modify it directly,
// just like a real MIPS function would.
void GetPlayerMaxHealth_recomp(EmotionEngineState& context);
