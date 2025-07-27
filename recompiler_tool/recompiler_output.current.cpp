// Successfully disassembled 72 instructions.
// Starting C++ code generation...

context.cpuRegs.GPR.r[8].SD[0] = (s64)(s32)(4660 << 16);
context.cpuRegs.GPR.r[8].UD[0] = context.cpuRegs.GPR.r[8].UD[0] | (u32)(22136);
context.cpuRegs.GPR.r[9].SD[0] = (s64)(s32)(0 << 16);
context.cpuRegs.GPR.r[9].UD[0] = context.cpuRegs.GPR.r[9].UD[0] | (u32)(1);
context.cpuRegs.GPR.r[10].SD[0] = (s64)(s32)(65535 << 16);
context.cpuRegs.GPR.r[10].UD[0] = context.cpuRegs.GPR.r[10].UD[0] | (u32)(65535);
context.cpuRegs.GPR.r[29].SD[0] = (s64)(s32)(context.cpuRegs.GPR.r[29].SD[0] + -32);
context.cpuRegs.GPR.r[16].SD[0] = (s64)(s32)(context.cpuRegs.GPR.r[8].SD[0] + context.cpuRegs.GPR.r[9].SD[0]);
context.cpuRegs.GPR.r[17].SD[0] = (s64)(s32)(context.cpuRegs.GPR.r[8].SD[0] - context.cpuRegs.GPR.r[9].SD[0]);
context.cpuRegs.GPR.r[11].SD[0] = (s64)((s32)context.cpuRegs.GPR.r[9].SD[0] < (s32)context.cpuRegs.GPR.r[8].SD[0] ? 1 : 0);
context.cpuRegs.GPR.r[12].SD[0] = (s64)((s32)context.cpuRegs.GPR.r[8].SD[0] < (s32)context.cpuRegs.GPR.r[9].SD[0] ? 1 : 0);
context.cpuRegs.GPR.r[13].SD[0] = (s64)((s32)context.cpuRegs.GPR.r[10].SD[0] < (s32)context.cpuRegs.GPR.r[9].SD[0] ? 1 : 0);
{
   s32 op1 = context.cpuRegs.GPR.r[ 8].SD[0];
   s32 op2 = context.cpuRegs.GPR.r[ 9 ].SD[0];
   s64 product = op1 * op2;
   context.cpuRegs.LO.SD[0] = (s64)(s32)product;
   context.cpuRegs.HI.SD[0] = (s64)(s32)(product >> 32);
}
// NOP
// Unhandled instruction: bnez
{
   s32 num = (s32)context.cpuRegs.GPR.r[ 8].SD[0];
   s32 den = (s32)context.cpuRegs.GPR.r[ 9 ].SD[0];
   if (den != 0){
       s32 HI_ans = num % den;
       s32 LO_ans = num / den;
       context.cpuRegs.LO.SD[0] = (s64)(s32)LO_ans;
       context.cpuRegs.HI.SD[0] = (s64)(s32)(HI_ans);
   }
}
// Unhandled instruction: break
context.cpuRegs.GPR.r[1].SD[0] = (s64)(s32)(context.cpuRegs.GPR.r[0].SD[0] + -1);
context.cpuRegs.GPR.r[1].SD[0] = (s64)(s32)(32768 << 16);
if (context.cpuRegs.GPR.r[9].UD[0] != context.cpuRegs.GPR.r[1].UD[0]) {
    context.cpuRegs.pc = 1048648 + 4 + (1048668 << 2);
} else {
    context.cpuRegs.pc = 1048648 + 8;
}
// NOP
if (context.cpuRegs.GPR.r[8].UD[0] != context.cpuRegs.GPR.r[1].UD[0]) {
    context.cpuRegs.pc = 1048656 + 4 + (1048668 << 2);
} else {
    context.cpuRegs.pc = 1048656 + 8;
}
// Unhandled instruction: break
// Unhandled instruction: mflo
// NOP
context.cpuRegs.GPR.r[18].UD[0] = context.cpuRegs.GPR.r[8].UD[0] | context.cpuRegs.GPR.r[9].UD[0];
// Unhandled instruction: and
{
   u64 rs_val = context.cpuRegs.GPR.r[8].UD[0];
   u64 rt_val = context.cpuRegs.GPR.r[9].UD[0];
   context.cpuRegs.GPR.r[20].UD[0] = rs_val ^ rt_val;
}
{
   u64 rs_val = context.cpuRegs.GPR.r[8].UD[0];
   u64 rt_val = context.cpuRegs.GPR.r[9].UD[0];
   context.cpuRegs.GPR.r[21].UD[0] = ~(rs_val | rt_val);
}
context.cpuRegs.GPR.r[22].SD[0] = (s64)(s32)((u32)context.cpuRegs.GPR.r[8].UD[0] << 4);
{
   u64 rt_val = context.cpuRegs.GPR.r[8].UD[0];
   context.cpuRegs.GPR.r[23].UD[0] = (u64)((u32)rt_val >>4);
}
{
   s64 rt_val = context.cpuRegs.GPR.r[8].UD[0];
   context.cpuRegs.GPR.r[24].SD[0] = (s64)((s32)(rt_val) >>4);
}
{
   s64 rt_val = context.cpuRegs.GPR.r[10].UD[0];
   context.cpuRegs.GPR.r[25].SD[0] = (s64)((s32)(rt_val) >>4);
}
{
    u32 address = context.cpuRegs.GPR.r[29].UD[0] + 0;
    WriteMemory32(address, (u32)context.cpuRegs.GPR.r[8].UD[0]);
}
// NOP
{
    u32 address = context.cpuRegs.GPR.r[29].UD[0] + 4;
    if (address % 2 != 0) {
        std::cerr << "FATAL ERROR: Unaligned memory access for LH at address: 0x" << std::hex << address << std::endl;
        exit(1);
    }
    WriteMemory16(address, (u16)context.cpuRegs.GPR.r[8].UD[0]);
}
// NOP
{
    u32 address = context.cpuRegs.GPR.r[29].UD[0] + 6;
    WriteMemory8(address, (u8)context.cpuRegs.GPR.r[8].UD[0]);
}
// NOP
{
    u32 address = context.cpuRegs.GPR.r[29].UD[0] + 0;
    context.cpuRegs.GPR.r[16].SD[0] = (s64)(s32)ReadMemory32(address);
}
// NOP
{
    u32 address = context.cpuRegs.GPR.r[29].UD[0] + 4;
    if (address % 2 != 0) {
        std::cerr << "FATAL ERROR: Unaligned memory access for LH at address: 0x" << std::hex << address << std::endl;
        exit(1);
    }
    s16 value = (s16)ReadMemory16(address);
    context.cpuRegs.GPR.r[17].SD[0] = (s64)value;
}
// NOP
{
    u32 address = context.cpuRegs.GPR.r[29].UD[0] + 6;
    context.cpuRegs.GPR.r[18].SD[0] = (s64)(s32)ReadMemory8(address);
}
// NOP
// NOP
if (context.cpuRegs.GPR.r[8].UD[0] == context.cpuRegs.GPR.r[8].UD[0]) {
    context.cpuRegs.pc = 1048756 + 4 + (1048772 << 2);
} else {
    context.cpuRegs.pc = 1048756 + 8;
}
// NOP
context.cpuRegs.pc = (1048764& 0xF0000000) | (4364 << 2);
// NOP
if (context.cpuRegs.GPR.r[8].UD[0] != context.cpuRegs.GPR.r[9].UD[0]) {
    context.cpuRegs.pc = 1048772 + 4 + (1048788 << 2);
} else {
    context.cpuRegs.pc = 1048772 + 8;
}
// NOP
context.cpuRegs.pc = (1048780& 0xF0000000) | (4364 << 2);
// NOP
if ((s64)context.cpuRegs.GPR.r[8].SD[0] > 0) {
    context.cpuRegs.pc = 1048788 + 4 + (1048804 << 2);
} else {
    context.cpuRegs.pc = 1048788 + 8;
}
// NOP
context.cpuRegs.pc = (1048796& 0xF0000000) | (4364 << 2);
// NOP
if ((s64)context.cpuRegs.GPR.r[10].SD[0] <= 0) {
    context.cpuRegs.pc = 1048804 + 4 + (1048820 << 2);
} else {
    context.cpuRegs.pc = 1048804 + 8;
}
// NOP
context.cpuRegs.pc = (1048812& 0xF0000000) | (4364 << 2);
// NOP
context.cpuRegs.GPR.r[31].UD[0] = 1048820+ 8;
context.cpuRegs.pc = (1048820& 0xF0000000) | (4356 << 2);
// NOP
context.cpuRegs.pc = context.cpuRegs.GPR.r[31].UD[0];
// NOP
context.cpuRegs.pc = context.cpuRegs.GPR.r[31].UD[0];
// NOP
// NOP
// NOP
// NOP
// NOP
