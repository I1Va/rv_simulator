#include "instruction.hpp"
#include <iostream>
namespace rv
{
    
// --- Arithmetic Implementation ---
void execute(ADD  i, ICPU &c, IMEM &) { c.write_reg(i.rd, c.read_reg(i.rs1) + c.read_reg(i.rs2)); c.set_pc(c.pc()+4); }
void execute(SUB  i, ICPU &c, IMEM &) { c.write_reg(i.rd, c.read_reg(i.rs1) - c.read_reg(i.rs2)); c.set_pc(c.pc()+4); }
void execute(SLL  i, ICPU &c, IMEM &) { c.write_reg(i.rd, c.read_reg(i.rs1) << (c.read_reg(i.rs2) & 0x1F)); c.set_pc(c.pc()+4); }
void execute(SLT  i, ICPU &c, IMEM &) { c.write_reg(i.rd, (int32_t)c.read_reg(i.rs1) < (int32_t)c.read_reg(i.rs2) ? 1 : 0); c.set_pc(c.pc()+4); }
void execute(SLTU i, ICPU &c, IMEM &) { c.write_reg(i.rd, c.read_reg(i.rs1) < c.read_reg(i.rs2) ? 1 : 0); c.set_pc(c.pc()+4); }
void execute(XOR  i, ICPU &c, IMEM &) { c.write_reg(i.rd, c.read_reg(i.rs1) ^ c.read_reg(i.rs2)); c.set_pc(c.pc()+4); }
void execute(SRL  i, ICPU &c, IMEM &) { c.write_reg(i.rd, c.read_reg(i.rs1) >> (c.read_reg(i.rs2) & 0x1F)); c.set_pc(c.pc()+4); }
void execute(SRA  i, ICPU &c, IMEM &) { c.write_reg(i.rd, (uint32_t)((int32_t)c.read_reg(i.rs1) >> (c.read_reg(i.rs2) & 0x1F))); c.set_pc(c.pc()+4); }
void execute(OR   i, ICPU &c, IMEM &) { c.write_reg(i.rd, c.read_reg(i.rs1) | c.read_reg(i.rs2)); c.set_pc(c.pc()+4); }
void execute(AND  i, ICPU &c, IMEM &) { c.write_reg(i.rd, c.read_reg(i.rs1) & c.read_reg(i.rs2)); c.set_pc(c.pc()+4); }

// --- Immediate Arithmetic ---
void execute(ADDI i, ICPU &c, IMEM &) { c.write_reg(i.rd, c.read_reg(i.rs1) + i.imm); c.set_pc(c.pc()+4); }
void execute(SLTI i, ICPU &c, IMEM &) { c.write_reg(i.rd, (int32_t)c.read_reg(i.rs1) < i.imm ? 1 : 0); c.set_pc(c.pc()+4); }
void execute(SRAI i, ICPU &c, IMEM &) { c.write_reg(i.rd, (uint32_t)((int32_t)c.read_reg(i.rs1) >> i.shamt)); c.set_pc(c.pc()+4); }
void execute(LUI  i, ICPU &c, IMEM &) { c.write_reg(i.rd, i.imm); c.set_pc(c.pc()+4); }
void execute(AUIPC i, ICPU &c, IMEM &) { c.write_reg(i.rd, (uint32_t)c.pc() + i.imm); c.set_pc(c.pc()+4); }

// --- Memory Operations ---
void execute(LW  i, ICPU &c, IMEM &m) { c.write_reg(i.rd, m.read32(c.read_reg(i.rs1) + i.imm)); c.set_pc(c.pc()+4); }
void execute(LBU i, ICPU &c, IMEM &m) { c.write_reg(i.rd, m.read8(c.read_reg(i.rs1) + i.imm)); c.set_pc(c.pc()+4); }
void execute(SW  i, ICPU &c, IMEM &m) { m.write32(c.read_reg(i.rs1) + i.imm, c.read_reg(i.rs2)); c.set_pc(c.pc()+4); }
void execute(SB  i, ICPU &c, IMEM &m) { m.write8(c.read_reg(i.rs1) + i.imm, (uint8_t)c.read_reg(i.rs2)); c.set_pc(c.pc()+4); }

// --- Branching Logic ---
void execute(BEQ i, ICPU &c, IMEM &) {
    if (c.read_reg(i.rs1) == c.read_reg(i.rs2)) c.set_pc(c.pc() + i.imm);
    else c.set_pc(c.pc() + 4);
}
void execute(BNE i, ICPU &c, IMEM &) {
    if (c.read_reg(i.rs1) != c.read_reg(i.rs2)) c.set_pc(c.pc() + i.imm);
    else c.set_pc(c.pc() + 4);
}
void execute(BLT i, ICPU &c, IMEM &) {
    if ((int32_t)c.read_reg(i.rs1) < (int32_t)c.read_reg(i.rs2)) c.set_pc(c.pc() + i.imm);
    else c.set_pc(c.pc() + 4);
}

// --- Jumps ---
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
void execute(LI i, ICPU &c, IMEM &) {
    std::cout << "execute LI!\n";
    c.write_reg(i.rd, static_cast<uint32_t>(i.imm));
    c.set_pc(c.pc() + 4);
}

} // namespace rv
