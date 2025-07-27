// Successfully disassembled 8 instructions.
// Starting C++ code generation...

context.cpuRegs.GPR.r[8].SD[0] = (s64)(s32)(imm << 16);
context.cpuRegs.GPR.r[8].UD[0] = context.cpuRegs.GPR.r[8] | (u64)(u32)(u16)(48879);
{
    u32 address = context.cpuRegs.GPR.r[29].UD[0] + 16;
    WriteMemory32(address, (u32)context.cpuRegs.GPR.r[8].UD[0]);
}
{
    u32 address = context.cpuRegs.GPR.r[29].UD[0] + 16;
    context.cpuRegs.GPR.r[9].SD[0] = (s64)(s32)ReadMemory32(address);
}
// NOP
context.cpuRegs.pc = context.cpuRegs.GPR.r[31].UD[0];
// NOP
// NOP
