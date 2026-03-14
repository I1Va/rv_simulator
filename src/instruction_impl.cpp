#include "instruction.hpp"
#include <iostream>
namespace rv
{
    
// --- Arithmetic Implementation ---
void execute(ADD i, ICPU &c, IMEM &) {
    uint32_t result = c.read_reg(i.rs1) + c.read_reg(i.rs2);
    c.write_reg(i.rd, result);
    c.set_pc(c.pc() + 4);
}
void execute(SUB i, ICPU &c, IMEM &) {
    uint32_t result = c.read_reg(i.rs1) - c.read_reg(i.rs2);
    c.write_reg(i.rd, result);
    c.set_pc(c.pc() + 4);
}
void execute(SLL i, ICPU &c, IMEM &) {
    uint32_t shift = c.read_reg(i.rs2) & 0x1F;
    c.write_reg(i.rd, c.read_reg(i.rs1) << shift);
    c.set_pc(c.pc() + 4);
}
void execute(SLT i, ICPU &c, IMEM &) {
    bool less = (int32_t)c.read_reg(i.rs1) < (int32_t)c.read_reg(i.rs2);
    c.write_reg(i.rd, less ? 1 : 0);
    c.set_pc(c.pc() + 4);
}
void execute(SLTU i, ICPU &c, IMEM &) {
    bool less = c.read_reg(i.rs1) < c.read_reg(i.rs2);
    c.write_reg(i.rd, less ? 1 : 0);
    c.set_pc(c.pc() + 4);
}
void execute(XOR i, ICPU &c, IMEM &) {
    c.write_reg(i.rd, c.read_reg(i.rs1) ^ c.read_reg(i.rs2));
    c.set_pc(c.pc() + 4);
}
void execute(SRL i, ICPU &c, IMEM &) {
    uint32_t shift = c.read_reg(i.rs2) & 0x1F;
    c.write_reg(i.rd, c.read_reg(i.rs1) >> shift);
    c.set_pc(c.pc() + 4);
}
void execute(SRA i, ICPU &c, IMEM &) {
    uint32_t shift = c.read_reg(i.rs2) & 0x1F;
    int32_t val = (int32_t)c.read_reg(i.rs1);
    c.write_reg(i.rd, (uint32_t)(val >> shift));
    c.set_pc(c.pc() + 4);
}
void execute(OR i, ICPU &c, IMEM &) {
    c.write_reg(i.rd, c.read_reg(i.rs1) | c.read_reg(i.rs2));
    c.set_pc(c.pc() + 4);
}
void execute(AND i, ICPU &c, IMEM &) {
    c.write_reg(i.rd, c.read_reg(i.rs1) & c.read_reg(i.rs2));
    c.set_pc(c.pc() + 4);
}

// --- Immediate Arithmetic ---
void execute(ADDI i, ICPU &c, IMEM &) {
    c.write_reg(i.rd, c.read_reg(i.rs1) + i.imm);
    c.set_pc(c.pc() + 4);
}
void execute(SLTI i, ICPU &c, IMEM &) {
    bool less = (int32_t)c.read_reg(i.rs1) < i.imm;
    c.write_reg(i.rd, less ? 1 : 0);
    c.set_pc(c.pc() + 4);
}
void execute(SLTIU i, ICPU &c, IMEM &) {
    bool less = c.read_reg(i.rs1) < (uint32_t)i.imm;
    c.write_reg(i.rd, less ? 1 : 0);
    c.set_pc(c.pc() + 4);
}
void execute(XORI i, ICPU &c, IMEM &) {
    c.write_reg(i.rd, c.read_reg(i.rs1) ^ (uint32_t)i.imm);
    c.set_pc(c.pc() + 4);
}
void execute(ORI i, ICPU &c, IMEM &) {
    c.write_reg(i.rd, c.read_reg(i.rs1) | (uint32_t)i.imm);
    c.set_pc(c.pc() + 4);
}
void execute(ANDI i, ICPU &c, IMEM &) {
    c.write_reg(i.rd, c.read_reg(i.rs1) & (uint32_t)i.imm);
    c.set_pc(c.pc() + 4);
}
void execute(SLLI i, ICPU &c, IMEM &) {
    uint32_t shift = i.shamt & 0x1F;
    c.write_reg(i.rd, c.read_reg(i.rs1) << shift);
    c.set_pc(c.pc() + 4);
}
void execute(SRLI i, ICPU &c, IMEM &) {
    uint32_t shift = i.shamt & 0x1F;
    c.write_reg(i.rd, c.read_reg(i.rs1) >> shift);
    c.set_pc(c.pc() + 4);
}
void execute(SRAI i, ICPU &c, IMEM &) {
    uint32_t shift = i.shamt & 0x1F;
    int32_t val = (int32_t)c.read_reg(i.rs1);
    c.write_reg(i.rd, (uint32_t)(val >> shift));
    c.set_pc(c.pc() + 4);
}

// --- Memory Operations ---
void execute(LB i, ICPU &c, IMEM &m) {
    uint32_t addr = c.read_reg(i.rs1) + i.imm;
    int32_t val = (int32_t)(int8_t)m.read8(addr);
    c.write_reg(i.rd, val);
    c.set_pc(c.pc() + 4);
}
void execute(LH i, ICPU &c, IMEM &m) {
    uint32_t addr = c.read_reg(i.rs1) + i.imm;
    int32_t val = (int32_t)(int16_t)m.read16(addr);
    c.write_reg(i.rd, val);
    c.set_pc(c.pc() + 4);
}
void execute(LW i, ICPU &c, IMEM &m) {
    uint32_t addr = c.read_reg(i.rs1) + i.imm;
    c.write_reg(i.rd, m.read32(addr));
    c.set_pc(c.pc() + 4);
}
void execute(LBU i, ICPU &c, IMEM &m) {
    uint32_t addr = c.read_reg(i.rs1) + i.imm;
    c.write_reg(i.rd, (uint32_t)m.read8(addr));
    c.set_pc(c.pc() + 4);
}
void execute(LHU i, ICPU &c, IMEM &m) {
    uint32_t addr = c.read_reg(i.rs1) + i.imm;
    c.write_reg(i.rd, (uint32_t)m.read16(addr));
    c.set_pc(c.pc() + 4);
}
void execute(SB i, ICPU &c, IMEM &m) {
    uint32_t addr = c.read_reg(i.rs1) + i.imm;
    m.write8(addr, (uint8_t)c.read_reg(i.rs2));
    c.set_pc(c.pc() + 4);
}
void execute(SH i, ICPU &c, IMEM &m) {
    uint32_t addr = c.read_reg(i.rs1) + i.imm;
    m.write16(addr, (uint16_t)c.read_reg(i.rs2));
    c.set_pc(c.pc() + 4);
}
void execute(SW i, ICPU &c, IMEM &m) {
    uint32_t addr = c.read_reg(i.rs1) + i.imm;
    m.write32(addr, c.read_reg(i.rs2));
    c.set_pc(c.pc() + 4);
}

// --- Control flow ---
void execute(BEQ i, ICPU &c, IMEM &) {
    if (c.read_reg(i.rs1) == c.read_reg(i.rs2)) {
        c.set_pc(c.pc() + i.imm);
    } else {
        c.set_pc(c.pc() + 4);
    }
}
void execute(BNE i, ICPU &c, IMEM &) {
    if (c.read_reg(i.rs1) != c.read_reg(i.rs2)) {
        c.set_pc(c.pc() + i.imm);
    } else {
        c.set_pc(c.pc() + 4);
    }
}
void execute(BLT i, ICPU &c, IMEM &) {
    if ((int32_t)c.read_reg(i.rs1) < (int32_t)c.read_reg(i.rs2)) {
        c.set_pc(c.pc() + i.imm);
    } else {
        c.set_pc(c.pc() + 4);
    }
}
void execute(BGE i, ICPU &c, IMEM &) {
    if ((int32_t)c.read_reg(i.rs1) >= (int32_t)c.read_reg(i.rs2)) {
        c.set_pc(c.pc() + i.imm);
    } else {
        c.set_pc(c.pc() + 4);
    }
}
void execute(BLTU i, ICPU &c, IMEM &) {
    if (c.read_reg(i.rs1) < c.read_reg(i.rs2)) {
        c.set_pc(c.pc() + i.imm);
    } else {
        c.set_pc(c.pc() + 4);
    }
}
void execute(BGEU i, ICPU &c, IMEM &) {
    if (c.read_reg(i.rs1) >= c.read_reg(i.rs2)) {
        c.set_pc(c.pc() + i.imm);
    } else {
        c.set_pc(c.pc() + 4);
    }
}
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
void execute(LUI i, ICPU &c, IMEM &) {
    c.write_reg(i.rd, (uint32_t)i.imm << 12); 
    c.set_pc(c.pc() + 4);
}
void execute(AUIPC i, ICPU &c, IMEM &) {
    c.write_reg(i.rd, (uint32_t)c.pc() + ((uint32_t)i.imm << 12));
    c.set_pc(c.pc() + 4);
}
void execute(LI i, ICPU &c, IMEM &) {
    c.write_reg(i.rd, (uint32_t)i.imm);
    c.set_pc(c.pc() + 4);
}


static const std::string reg_abi[] = {
    "zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
    "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
    "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
    "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};



#define DEFFUNC(Type, NameStr, OpsExpr) \
    std::string name(const Type&) { return NameStr; } \
    std::string operands([[maybe_unused]] const Type& i) { return OpsExpr; }

#define R_OPS reg_abi[i.rd] + "," + reg_abi[i.rs1] + "," + reg_abi[i.rs2]
#define I_OPS reg_abi[i.rd] + "," + reg_abi[i.rs1] + "," + std::to_string(i.imm)
#define S_OPS reg_abi[i.rs2] + "," + std::to_string(i.imm) + "(" + reg_abi[i.rs1] + ")"
#define L_OPS reg_abi[i.rd] + "," + std::to_string(i.imm) + "(" + reg_abi[i.rs1] + ")"
#define B_OPS reg_abi[i.rs1] + "," + reg_abi[i.rs2] + "," + std::to_string(i.imm)

// --- R-Type ---
DEFFUNC(ADD,  "add",  R_OPS)
DEFFUNC(SUB,  "sub",  R_OPS)
DEFFUNC(SLL,  "sll",  R_OPS)
DEFFUNC(SLT,  "slt",  R_OPS)
DEFFUNC(SLTU, "sltu", R_OPS)
DEFFUNC(XOR,  "xor",  R_OPS)
DEFFUNC(SRL,  "srl",  R_OPS)
DEFFUNC(SRA,  "sra",  R_OPS)
DEFFUNC(OR,   "or",   R_OPS)
DEFFUNC(AND,  "and",  R_OPS)

// --- I-Type ---
DEFFUNC(ADDI,  "addi",  I_OPS)
DEFFUNC(SLTI,  "slti",  I_OPS)
DEFFUNC(SLTIU, "sltiu", I_OPS)
DEFFUNC(XORI,  "xori",  I_OPS)
DEFFUNC(ORI,   "ori",   I_OPS)
DEFFUNC(ANDI,  "andi",  I_OPS)
DEFFUNC(SLLI,  "slli",  reg_abi[i.rd] + "," + reg_abi[i.rs1] + "," + std::to_string(i.shamt))
DEFFUNC(SRLI,  "srli",  reg_abi[i.rd] + "," + reg_abi[i.rs1] + "," + std::to_string(i.shamt))
DEFFUNC(SRAI,  "srai",  reg_abi[i.rd] + "," + reg_abi[i.rs1] + "," + std::to_string(i.shamt))

// --- Loads & Stores ---
DEFFUNC(LB,  "lb",  L_OPS)
DEFFUNC(LH,  "lh",  L_OPS)
DEFFUNC(LW,  "lw",  L_OPS)
DEFFUNC(LBU, "lbu", L_OPS)
DEFFUNC(LHU, "lhu", L_OPS)
DEFFUNC(SB,  "sb",  S_OPS)
DEFFUNC(SH,  "sh",  S_OPS)
DEFFUNC(SW,  "sw",  S_OPS)

// --- Branches ---
DEFFUNC(BEQ,  "beq",  B_OPS)
DEFFUNC(BNE,  "bne",  B_OPS)
DEFFUNC(BLT,  "blt",  B_OPS)
DEFFUNC(BGE,  "bge",  B_OPS)
DEFFUNC(BLTU, "bltu", B_OPS)
DEFFUNC(BGEU, "bgeu", B_OPS)

// --- Jumps & System ---
DEFFUNC(JAL,   "jal",   reg_abi[i.rd] + "," + std::to_string(i.imm))
DEFFUNC(JALR,  "jalr",  reg_abi[i.rd] + "," + std::to_string(i.imm) + "(" + reg_abi[i.rs1] + ")")
DEFFUNC(LUI,   "lui",   reg_abi[i.rd] + "," + std::to_string(i.imm >> 12))
DEFFUNC(AUIPC, "auipc", reg_abi[i.rd] + "," + std::to_string(i.imm >> 12))
DEFFUNC(LI,    "li",    reg_abi[i.rd] + "," + std::to_string(i.imm))
DEFFUNC(ECALL, "ecall", "")



} // namespace rv
