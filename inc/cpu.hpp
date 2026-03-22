#pragma once

#include <cstdint>
#include <cassert>
#include <iomanip>
#include <string>
#include <vector>
#include <instruction.hpp>
#include <iostream>
#include <memory.hpp>

namespace rv
{

class ICPU {
public:    
    virtual uint64_t read_reg(uint8_t idx) const = 0;
    virtual void write_reg(uint8_t idx, uint64_t val) = 0;

    virtual void set_pc(uint64_t pc) = 0;
    virtual uint64_t pc() const = 0;
    virtual void dump() const = 0;
    virtual void execute(const Instruction &i, IMEM &m) = 0;
    // virtual void raise_exception(Exception e) = 0;
};

class CPU_RV32I : public ICPU {
    uint32_t pc_;
    std::vector<uint32_t> regs_;

    const std::vector<std::string> abi_names_ = {
        "zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
        "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
        "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
        "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
    };

public:
    CPU_RV32I(): pc_(0), regs_(32) {}

    void set_pc(uint64_t pc) override {
        pc_ = static_cast<uint32_t> (pc);
    }

    uint64_t pc() const override {
        return pc_;
    }
    
    uint64_t read_reg(uint8_t idx) const override {
        assert(idx < 32 && "Register index out of range!");
        return regs_[idx];
    }

    void write_reg(uint8_t idx, uint64_t val) override {
        assert(idx < 32 && "Register index out of range!");
        if (idx == 0) return;
        regs_[idx] = static_cast<uint32_t>(val);
    }

    void dump() const {
        std::cout << " pc       " 
                << std::hex << std::setw(8) << std::setfill('0') << pc_ 
                << std::dec << "\n";

        for (int i = 0; i < 32; ++i) {
            std::string abi = "x" + std::to_string(i) + "/" + abi_names_[i];
            
            std::cout << " ";

            std::cout << std::left << std::setw(8) << std::setfill(' ') << abi << " ";
            
            std::cout << std::right << std::hex << std::setw(8) << std::setfill('0') << regs_[i];

            if ((i + 1) % 4 == 0) {
                std::cout << "\n";
            } else {
                std::cout << " ";
            }
        }
        std::cout << std::dec << std::setfill(' ');
    }

    void execute(const Instruction &i, IMEM &m) override {
        switch (i.type) {
            // --- Arithmetic R-Type ---
            case InstructionType::ADD:
                write_reg(i.rd, read_reg(i.rs1) + read_reg(i.rs2));
                set_pc(pc() + 4);
                break;
            case InstructionType::SUB:
                write_reg(i.rd, read_reg(i.rs1) - read_reg(i.rs2));
                set_pc(pc() + 4);
                break;
            case InstructionType::SLL:
                write_reg(i.rd, read_reg(i.rs1) << (read_reg(i.rs2) & 0x1F));
                set_pc(pc() + 4);
                break;
            case InstructionType::SLT:
                write_reg(i.rd, (int32_t)read_reg(i.rs1) < (int32_t)read_reg(i.rs2) ? 1 : 0);
                set_pc(pc() + 4);
                break;
            case InstructionType::SLTU:
                write_reg(i.rd, read_reg(i.rs1) < read_reg(i.rs2) ? 1 : 0);
                set_pc(pc() + 4);
                break;
            case InstructionType::XOR:
                write_reg(i.rd, read_reg(i.rs1) ^ read_reg(i.rs2));
                set_pc(pc() + 4);
                break;
            case InstructionType::SRL:
                write_reg(i.rd, read_reg(i.rs1) >> (read_reg(i.rs2) & 0x1F));
                set_pc(pc() + 4);
                break;
            case InstructionType::SRA:
                write_reg(i.rd, (uint32_t)((int32_t)read_reg(i.rs1) >> (read_reg(i.rs2) & 0x1F)));
                set_pc(pc() + 4);
                break;
            case InstructionType::OR:
                write_reg(i.rd, read_reg(i.rs1) | read_reg(i.rs2));
                set_pc(pc() + 4);
                break;
            case InstructionType::AND:
                write_reg(i.rd, read_reg(i.rs1) & read_reg(i.rs2));
                set_pc(pc() + 4);
                break;

            // --- Arithmetic I-Type ---
            case InstructionType::ADDI:
                write_reg(i.rd, read_reg(i.rs1) + i.imm);
                set_pc(pc() + 4);
                break;
            case InstructionType::SLTI:
                write_reg(i.rd, (int32_t)read_reg(i.rs1) < i.imm ? 1 : 0);
                set_pc(pc() + 4);
                break;
            case InstructionType::SLTIU:
                write_reg(i.rd, read_reg(i.rs1) < (uint32_t)i.imm ? 1 : 0);
                set_pc(pc() + 4);
                break;
            case InstructionType::XORI:
                write_reg(i.rd, read_reg(i.rs1) ^ (uint32_t)i.imm);
                set_pc(pc() + 4);
                break;
            case InstructionType::ORI:
                write_reg(i.rd, read_reg(i.rs1) | (uint32_t)i.imm);
                set_pc(pc() + 4);
                break;
            case InstructionType::ANDI:
                write_reg(i.rd, read_reg(i.rs1) & (uint32_t)i.imm);
                set_pc(pc() + 4);
                break;
            case InstructionType::SLLI:
                write_reg(i.rd, read_reg(i.rs1) << (i.shamt & 0x1F));
                set_pc(pc() + 4);
                break;
            case InstructionType::SRLI:
                write_reg(i.rd, read_reg(i.rs1) >> (i.shamt & 0x1F));
                set_pc(pc() + 4);
                break;
            case InstructionType::SRAI:
                write_reg(i.rd, (uint32_t)((int32_t)read_reg(i.rs1) >> (i.shamt & 0x1F)));
                set_pc(pc() + 4);
                break;

            // --- Memory Operations ---
            case InstructionType::LB:
                write_reg(i.rd, (int32_t)(int8_t)m.read8(read_reg(i.rs1) + i.imm));
                set_pc(pc() + 4);
                break;
            case InstructionType::LH:
                write_reg(i.rd, (int32_t)(int16_t)m.read16(read_reg(i.rs1) + i.imm));
                set_pc(pc() + 4);
                break;
            case InstructionType::LW:
                write_reg(i.rd, m.read32(read_reg(i.rs1) + i.imm));
                set_pc(pc() + 4);
                break;
            case InstructionType::LBU:
                write_reg(i.rd, (uint32_t)m.read8(read_reg(i.rs1) + i.imm));
                set_pc(pc() + 4);
                break;
            case InstructionType::LHU:
                write_reg(i.rd, (uint32_t)m.read16(read_reg(i.rs1) + i.imm));
                set_pc(pc() + 4);
                break;
            case InstructionType::SB:
                m.write8(read_reg(i.rs1) + i.imm, (uint8_t)read_reg(i.rs2));
                set_pc(pc() + 4);
                break;
            case InstructionType::SH:
                m.write16(read_reg(i.rs1) + i.imm, (uint16_t)read_reg(i.rs2));
                set_pc(pc() + 4);
                break;
            case InstructionType::SW:
                m.write32(read_reg(i.rs1) + i.imm, read_reg(i.rs2));
                set_pc(pc() + 4);
                break;

            // --- Control Flow ---
            case InstructionType::BEQ:
                set_pc(pc() + (read_reg(i.rs1) == read_reg(i.rs2) ? i.imm : 4));
                break;
            case InstructionType::BNE:
                set_pc(pc() + (read_reg(i.rs1) != read_reg(i.rs2) ? i.imm : 4));
                break;
            case InstructionType::BLT:
                set_pc(pc() + ((int32_t)read_reg(i.rs1) < (int32_t)read_reg(i.rs2) ? i.imm : 4));
                break;
            case InstructionType::BGE:
                set_pc(pc() + ((int32_t)read_reg(i.rs1) >= (int32_t)read_reg(i.rs2) ? i.imm : 4));
                break;
            case InstructionType::BLTU:
                set_pc(pc() + (read_reg(i.rs1) < read_reg(i.rs2) ? i.imm : 4));
                break;
            case InstructionType::BGEU:
                set_pc(pc() + (read_reg(i.rs1) >= read_reg(i.rs2) ? i.imm : 4));
                break;
            case InstructionType::JAL:
                write_reg(i.rd, pc() + 4);
                set_pc(pc() + i.imm);
                break;
            case InstructionType::JALR: {
                uint32_t target = (read_reg(i.rs1) + i.imm) & ~1U;
                write_reg(i.rd, pc() + 4);
                set_pc(target);
                break;
            }

            // --- Upper Immediates & Pseudo ---
            case InstructionType::LUI:
                write_reg(i.rd, (uint32_t)i.imm << 12);
                set_pc(pc() + 4);
                break;
            case InstructionType::AUIPC:
                write_reg(i.rd, (uint32_t)pc() + ((uint32_t)i.imm << 12));
                set_pc(pc() + 4);
                break;
            case InstructionType::LI:
                write_reg(i.rd, (uint32_t)i.imm);
                set_pc(pc() + 4);
                break;

            case InstructionType::ECALL:
            case InstructionType::EBREAK:
                set_pc(pc() + 4);
                break;

            default:
                throw std::runtime_error(std::format(
                    "CPU_RV32I::execute unknown instruction type {}", 
                    static_cast<int>(i.type)
                ));
        }
    }

};


} // namespace rv
