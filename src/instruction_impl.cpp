#include <iostream>
#include <array>
#include <format>
#include <iostream>

#include "instruction.hpp"


namespace rv
{
  
static const char* reg_names[] = {
    "zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
    "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
    "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
    "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};

std::string get_instr_name(Instruction &instr) {
    switch (instr.type) {
        // --- Arithmetic R-Type ---
        case InstructionType::ADD:   return "add";
        case InstructionType::SUB:   return "sub";
        case InstructionType::SLL:   return "sll";
        case InstructionType::SLT:   return "slt";
        case InstructionType::SLTU:  return "sltu";
        case InstructionType::XOR:   return "xor";
        case InstructionType::SRL:   return "srl";
        case InstructionType::SRA:   return "sra";
        case InstructionType::OR:    return "or";
        case InstructionType::AND:   return "and";

        // --- Arithmetic I-Type ---
        case InstructionType::ADDI:  return "addi";
        case InstructionType::SLTI:  return "slti";
        case InstructionType::SLTIU: return "sltiu";
        case InstructionType::XORI:  return "xori";
        case InstructionType::ORI:   return "ori";
        case InstructionType::ANDI:  return "andi";
        case InstructionType::SLLI:  return "slli";
        case InstructionType::SRLI:  return "srli";
        case InstructionType::SRAI:  return "srai";

        // --- Loads & Stores ---
        case InstructionType::LB:    return "lb";
        case InstructionType::LH:    return "lh";
        case InstructionType::LW:    return "lw";
        case InstructionType::LBU:   return "lbu";
        case InstructionType::LHU:   return "lhu";
        case InstructionType::SB:    return "sb";
        case InstructionType::SH:    return "sh";
        case InstructionType::SW:    return "sw";

        // --- Control Flow ---
        case InstructionType::BEQ:   return "beq";
        case InstructionType::BNE:   return "bne";
        case InstructionType::BLT:   return "blt";
        case InstructionType::BGE:   return "bge";
        case InstructionType::BLTU:  return "bltu";
        case InstructionType::BGEU:  return "bgeu";
        case InstructionType::JAL:   return "jal";
        case InstructionType::JALR:  return "jalr";

        // --- Upper Immediates & System ---
        case InstructionType::LUI:   return "lui";
        case InstructionType::AUIPC: return "auipc";
        case InstructionType::ECALL: return "ecall";
        case InstructionType::EBREAK:return "ebreak";

        // --- Pseudo Instructions ---
        case InstructionType::LI:    return "li";

        default: return "unknown";
    }
}

std::string get_instr_operands(Instruction &instr) {
    using IT = InstructionType;
    
    switch (instr.type) {
        // R-Type: rd, rs1, rs2
        case IT::ADD:  case IT::SUB:  case IT::SLL:  case IT::SLT:
        case IT::SLTU: case IT::XOR:  case IT::SRL:  case IT::SRA:
        case IT::OR:   case IT::AND:
            return std::string(reg_names[instr.rd]) + ", " + reg_names[instr.rs1] + ", " + reg_names[instr.rs2];

        // I-Type: rd, rs1, imm
        case IT::ADDI:  case IT::SLTI: case IT::SLTIU: 
        case IT::XORI:  case IT::ORI:  case IT::ANDI: case IT::LI:
            return std::string(reg_names[instr.rd]) + ", " + reg_names[instr.rs1] + ", " + std::to_string(instr.imm);

        // Shift I-Type: rd, rs1, shamt
        case IT::SLLI:  case IT::SRLI: case IT::SRAI:
            return std::string(reg_names[instr.rd]) + ", " + reg_names[instr.rs1] + ", " + std::to_string(instr.shamt);

        // Load-Type: rd, imm(rs1)
        case IT::LB:  case IT::LH:  case IT::LW:  case IT::LBU: case IT::LHU:
            return std::string(reg_names[instr.rd]) + ", " + std::to_string(instr.imm) + "(" + reg_names[instr.rs1] + ")";

        // Store-Type: rs2, imm(rs1)
        case IT::SB:  case IT::SH:  case IT::SW:
            return std::string(reg_names[instr.rs2]) + ", " + std::to_string(instr.imm) + "(" + reg_names[instr.rs1] + ")";

        // Branch-Type: rs1, rs2, imm
        case IT::BEQ:  case IT::BNE:  case IT::BLT: 
        case IT::BGE:  case IT::BLTU: case IT::BGEU:
            return std::string(reg_names[instr.rs1]) + ", " + reg_names[instr.rs2] + ", " + std::to_string(instr.imm);

        // Jump/Upper-Type: rd, imm
        case IT::JAL:   case IT::LUI:  case IT::AUIPC:
            return std::string(reg_names[instr.rd]) + ", " + std::to_string(instr.imm);

        // JALR: rd, imm(rs1)
        case IT::JALR:
            return std::string(reg_names[instr.rd]) + ", " + std::to_string(instr.imm) + "(" + reg_names[instr.rs1] + ")";

        case IT::ECALL: case IT::EBREAK:
            return "";

        default:
            return "unknown";
    }
}


} // namespace rv
