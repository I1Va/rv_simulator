#pragma once
#include <cstdint>
#include <string>
namespace rv
{

enum class InstructionType {
    // --- Arithmetic R-Type ---
    ADD,
    SUB,
    SLL,
    SLT,
    SLTU,
    XOR,
    SRL,
    SRA,
    OR,
    AND,

    // --- Arithmetic I-Type ---
    ADDI,
    SLTI,
    SLTIU,
    XORI,
    ORI,
    ANDI,
    SLLI,
    SRLI,
    SRAI,

    // --- Loads & Stores ---
    LB,
    LH,
    LW,
    LBU,
    LHU,
    SB,
    SH,
    SW,

    // --- Control Flow ---
    BEQ,
    BNE,
    BLT,
    BGE,
    BLTU,
    BGEU,
    JAL,
    JALR,

    // --- Upper Immediates & System ---
    LUI,
    AUIPC,
    ECALL,
    EBREAK,

    // --- Pseudo Instructions ---
    LI,

    // --- Synchronization ---
    FENCE,
    FENCE_I
};

struct Instruction {
    InstructionType type;
    uint8_t rd, rs1, rs2;
    uint8_t shamt;
    int32_t imm;
};

#define R_TYPE_CONSTRUCTOR(Name) \
inline Instruction Name(uint8_t rd, uint8_t rs1, uint8_t rs2) { \
    Instruction result = {}; \
    result.type = InstructionType::Name; \
    result.rd = rd; result.rs1 = rs1; result.rs2 = rs2; \
    return result; \
}

R_TYPE_CONSTRUCTOR(ADD)  R_TYPE_CONSTRUCTOR(SUB)  R_TYPE_CONSTRUCTOR(SLL)
R_TYPE_CONSTRUCTOR(SLT)  R_TYPE_CONSTRUCTOR(SLTU) R_TYPE_CONSTRUCTOR(XOR)
R_TYPE_CONSTRUCTOR(SRL)  R_TYPE_CONSTRUCTOR(SRA)  R_TYPE_CONSTRUCTOR(OR)
R_TYPE_CONSTRUCTOR(AND)

#define I_TYPE_CONSTRUCTOR(Name) \
inline Instruction Name(uint8_t rd, uint8_t rs1, int32_t imm) { \
    Instruction result = {}; \
    result.type = InstructionType::Name; \
    result.rd = rd; result.rs1 = rs1; result.imm = imm; \
    return result; \
}

I_TYPE_CONSTRUCTOR(ADDI)  I_TYPE_CONSTRUCTOR(SLTI)  I_TYPE_CONSTRUCTOR(SLTIU)
I_TYPE_CONSTRUCTOR(XORI)  I_TYPE_CONSTRUCTOR(ORI)   I_TYPE_CONSTRUCTOR(ANDI)
I_TYPE_CONSTRUCTOR(LB)    I_TYPE_CONSTRUCTOR(LH)    I_TYPE_CONSTRUCTOR(LW)
I_TYPE_CONSTRUCTOR(LBU)   I_TYPE_CONSTRUCTOR(LHU)   I_TYPE_CONSTRUCTOR(JALR)

inline Instruction LI(uint8_t rd, int32_t imm) {
    Instruction result = {};
    result.type = InstructionType::LI; 
    result.rd = rd;
    result.imm = imm;
    return result; 
}

#define SHIFT_TYPE_CONSTRUCTOR(Name) \
inline Instruction Name(uint8_t rd, uint8_t rs1, uint8_t shamt) { \
    Instruction result = {}; \
    result.type = InstructionType::Name; \
    result.rd = rd; result.rs1 = rs1; result.shamt = shamt; \
    return result; \
}

SHIFT_TYPE_CONSTRUCTOR(SLLI) SHIFT_TYPE_CONSTRUCTOR(SRLI) SHIFT_TYPE_CONSTRUCTOR(SRAI)

#define S_TYPE_CONSTRUCTOR(Name) \
inline Instruction Name(uint8_t rs1, uint8_t rs2, int32_t imm) { \
    Instruction result = {}; \
    result.type = InstructionType::Name; \
    result.rs1 = rs1; result.rs2 = rs2; result.imm = imm; \
    return result; \
}

S_TYPE_CONSTRUCTOR(SB) S_TYPE_CONSTRUCTOR(SH) S_TYPE_CONSTRUCTOR(SW)

#define B_TYPE_CONSTRUCTOR(Name) \
inline Instruction Name(uint8_t rs1, uint8_t rs2, int32_t imm) { \
    Instruction result = {}; \
    result.type = InstructionType::Name; \
    result.rs1 = rs1; result.rs2 = rs2; result.imm = imm; \
    return result; \
}

B_TYPE_CONSTRUCTOR(BEQ)  B_TYPE_CONSTRUCTOR(BNE)  B_TYPE_CONSTRUCTOR(BLT)
B_TYPE_CONSTRUCTOR(BGE)  B_TYPE_CONSTRUCTOR(BLTU) B_TYPE_CONSTRUCTOR(BGEU)

#define UJ_TYPE_CONSTRUCTOR(Name) \
inline Instruction Name(uint8_t rd, int32_t imm) { \
    Instruction result = {}; \
    result.type = InstructionType::Name; \
    result.rd = rd; result.imm = imm; \
    return result; \
}

UJ_TYPE_CONSTRUCTOR(LUI) UJ_TYPE_CONSTRUCTOR(AUIPC) UJ_TYPE_CONSTRUCTOR(JAL)

inline Instruction ECALL() {
    Instruction result = {};
    result.type = InstructionType::ECALL;
    return result;
}

inline Instruction EBREAK() {
    Instruction result = {};
    result.type = InstructionType::EBREAK;
    return result;
}

inline Instruction FENCE_I() {
    Instruction result = {};
    result.type = InstructionType::FENCE_I;
    return result;
}

inline Instruction FENCE() {
    Instruction result = {};
    result.type = InstructionType::FENCE;
    return result;
}


std::string get_instr_name(Instruction &instr);
std::string get_instr_operands(Instruction &instr);

} // namspace rv
