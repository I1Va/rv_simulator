#include "instruction.hpp"
#include <iostream>
namespace rv
{
    
// --- Arithmetic Implementation ---
void execute(ADD  i, ICPU &c, IMEM &) { c.write_reg(i.rd, c.read_reg(i.rs1) + c.read_reg(i.rs2)); c.set_pc(c.pc() + 4); }
void execute(SUB  i, ICPU &c, IMEM &) { c.write_reg(i.rd, c.read_reg(i.rs1) - c.read_reg(i.rs2)); c.set_pc(c.pc() + 4); }
void execute(SLL  i, ICPU &c, IMEM &) { c.write_reg(i.rd, c.read_reg(i.rs1) << (c.read_reg(i.rs2) & 0x1F)); c.set_pc(c.pc() + 4); }
void execute(SLT  i, ICPU &c, IMEM &) { c.write_reg(i.rd, (int32_t)c.read_reg(i.rs1) < (int32_t)c.read_reg(i.rs2) ? 1 : 0); c.set_pc(c.pc() + 4); }
void execute(SLTU i, ICPU &c, IMEM &) { c.write_reg(i.rd, c.read_reg(i.rs1) < c.read_reg(i.rs2) ? 1 : 0); c.set_pc(c.pc() + 4); }
void execute(XOR  i, ICPU &c, IMEM &) { c.write_reg(i.rd, c.read_reg(i.rs1) ^ c.read_reg(i.rs2)); c.set_pc(c.pc() + 4); }
void execute(SRL  i, ICPU &c, IMEM &) { c.write_reg(i.rd, c.read_reg(i.rs1) >> (c.read_reg(i.rs2) & 0x1F)); c.set_pc(c.pc() + 4); }
void execute(SRA  i, ICPU &c, IMEM &) { c.write_reg(i.rd, (uint32_t)((int32_t)c.read_reg(i.rs1) >> (c.read_reg(i.rs2) & 0x1F))); c.set_pc(c.pc() + 4); }
void execute(OR   i, ICPU &c, IMEM &) { c.write_reg(i.rd, c.read_reg(i.rs1) | c.read_reg(i.rs2)); c.set_pc(c.pc() + 4); }
void execute(AND  i, ICPU &c, IMEM &) { c.write_reg(i.rd, c.read_reg(i.rs1) & c.read_reg(i.rs2)); c.set_pc(c.pc() + 4); }
// --- Immediate Arithmetic ---
void execute(ADDI  i, ICPU &c, IMEM &) { c.write_reg(i.rd, c.read_reg(i.rs1) + i.imm); c.set_pc(c.pc() + 4); }
void execute(SLTI  i, ICPU &c, IMEM &) { c.write_reg(i.rd, (int32_t)c.read_reg(i.rs1) < i.imm ? 1 : 0); c.set_pc(c.pc() + 4); }
void execute(SLTIU i, ICPU &c, IMEM &) { c.write_reg(i.rd, c.read_reg(i.rs1) < (uint32_t)i.imm ? 1 : 0); c.set_pc(c.pc() + 4); }
void execute(XORI  i, ICPU &c, IMEM &) { c.write_reg(i.rd, c.read_reg(i.rs1) ^ (uint32_t)i.imm); c.set_pc(c.pc() + 4); }
void execute(ORI   i, ICPU &c, IMEM &) { c.write_reg(i.rd, c.read_reg(i.rs1) | (uint32_t)i.imm); c.set_pc(c.pc() + 4); }
void execute(ANDI  i, ICPU &c, IMEM &) { c.write_reg(i.rd, c.read_reg(i.rs1) & (uint32_t)i.imm); c.set_pc(c.pc() + 4); }
void execute(SLLI  i, ICPU &c, IMEM &) { c.write_reg(i.rd, c.read_reg(i.rs1) << (i.shamt & 0x1F)); c.set_pc(c.pc() + 4); }
void execute(SRLI  i, ICPU &c, IMEM &) { c.write_reg(i.rd, c.read_reg(i.rs1) >> (i.shamt & 0x1F)); c.set_pc(c.pc() + 4); }
void execute(SRAI  i, ICPU &c, IMEM &) { c.write_reg(i.rd, (uint32_t)((int32_t)c.read_reg(i.rs1) >> (i.shamt & 0x1F))); c.set_pc(c.pc() + 4); }
// --- Memory Operations ---
// Loads (Sign extended for LB/LH)
void execute(LB  i, ICPU &c, IMEM &m) { c.write_reg(i.rd, (int32_t)(int8_t)m.read8(c.read_reg(i.rs1) + i.imm)); c.set_pc(c.pc() + 4); }
void execute(LH  i, ICPU &c, IMEM &m) { c.write_reg(i.rd, (int32_t)(int16_t)m.read16(c.read_reg(i.rs1) + i.imm)); c.set_pc(c.pc() + 4); }
void execute(LW  i, ICPU &c, IMEM &m) { c.write_reg(i.rd, m.read32(c.read_reg(i.rs1) + i.imm)); c.set_pc(c.pc() + 4); }
void execute(LBU i, ICPU &c, IMEM &m) { c.write_reg(i.rd, (uint32_t)m.read8(c.read_reg(i.rs1) + i.imm)); c.set_pc(c.pc() + 4); }
void execute(LHU i, ICPU &c, IMEM &m) { c.write_reg(i.rd, (uint32_t)m.read16(c.read_reg(i.rs1) + i.imm)); c.set_pc(c.pc() + 4); }

// Stores
void execute(SB  i, ICPU &c, IMEM &m) { m.write8(c.read_reg(i.rs1) + i.imm, (uint8_t)c.read_reg(i.rs2)); c.set_pc(c.pc() + 4); }
void execute(SH  i, ICPU &c, IMEM &m) { m.write16(c.read_reg(i.rs1) + i.imm, (uint16_t)c.read_reg(i.rs2)); c.set_pc(c.pc() + 4); }
void execute(SW  i, ICPU &c, IMEM &m) { m.write32(c.read_reg(i.rs1) + i.imm, c.read_reg(i.rs2)); c.set_pc(c.pc() + 4); }

// --- Control flow ---
void execute(BEQ  i, ICPU &c, IMEM &) { if (c.read_reg(i.rs1) == c.read_reg(i.rs2)) c.set_pc(c.pc() + i.imm); else c.set_pc(c.pc() + 4); }
void execute(BNE  i, ICPU &c, IMEM &) { if (c.read_reg(i.rs1) != c.read_reg(i.rs2)) c.set_pc(c.pc() + i.imm); else c.set_pc(c.pc() + 4); }
void execute(BLT  i, ICPU &c, IMEM &) { if ((int32_t)c.read_reg(i.rs1) < (int32_t)c.read_reg(i.rs2)) c.set_pc(c.pc() + i.imm); else c.set_pc(c.pc() + 4); }
void execute(BGE  i, ICPU &c, IMEM &) { if ((int32_t)c.read_reg(i.rs1) >= (int32_t)c.read_reg(i.rs2)) c.set_pc(c.pc() + i.imm); else c.set_pc(c.pc() + 4); }
void execute(BLTU i, ICPU &c, IMEM &) { if (c.read_reg(i.rs1) < c.read_reg(i.rs2)) c.set_pc(c.pc() + i.imm); else c.set_pc(c.pc() + 4); }
void execute(BGEU i, ICPU &c, IMEM &) { if (c.read_reg(i.rs1) >= c.read_reg(i.rs2)) c.set_pc(c.pc() + i.imm); else c.set_pc(c.pc() + 4); }

void execute(JAL i, ICPU &c, IMEM &) { 
    c.write_reg(i.rd, c.pc() + 4); 
    c.set_pc(c.pc() + i.imm); 
}
void execute(JALR i, ICPU &c, IMEM &) { 
    uint32_t target = (c.read_reg(i.rs1) + i.imm) & ~1U; 
    c.write_reg(i.rd, c.pc() + 4); 
    c.set_pc(target); 
}
// --- Pseudo Instructions ---
void execute(LUI   i, ICPU &c, IMEM &) { c.write_reg(i.rd, (uint32_t)i.imm); c.set_pc(c.pc() + 4); }
void execute(AUIPC i, ICPU &c, IMEM &) { c.write_reg(i.rd, (uint32_t)c.pc() + i.imm); c.set_pc(c.pc() + 4); }

// Pseudo LI (Functionally same as LUI or ADDI depending on decoder)
void execute(LI i, ICPU &c, IMEM &) { c.write_reg(i.rd, (uint32_t)i.imm); c.set_pc(c.pc() + 4); }

} // namespace rv
