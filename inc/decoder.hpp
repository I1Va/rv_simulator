#pragma once

#include "instruction.hpp"
#include "cpu.hpp"
#include "memory.hpp"

namespace rv
{

class IllegalInstruction32 : public std::runtime_error {
    uint32_t bits_;
public:
    // We take the raw instruction bits and the address where it occurred
    IllegalInstruction32(uint32_t bits)
        : std::runtime_error(std::format(
            "Illegal Instruction: 0x{:08x}", 
            bits)), 
          bits_(bits) {}

    uint32_t bits() const { return bits_; }
};


class IllegalInstruction32PC : public std::runtime_error {
    uint32_t bits_;
    uint64_t pc_;

public:
    IllegalInstruction32PC(uint32_t bits, uint64_t pc)
        : std::runtime_error(std::format(
            "Illegal Instruction: 0x{:08x} at PC 0x{:016x}", 
            bits, pc)), 
          bits_(bits), pc_(pc) {}

    uint32_t bits() const { return bits_; }
    uint64_t pc() const { return pc_; }
};




struct IDecoder {
    virtual ~IDecoder() = default;
    virtual Instruction fetch_and_decode(uint64_t addr, IMEM &mem) const = 0;
};

class Decoder_RV32I : public IDecoder {
public:
    Decoder_RV32I() = default;

    Instruction decode(uint32_t bits) const {
        uint8_t opcode = bits & 0x7F;

        switch (opcode) {
            case 0x33: return decode_R(bits);
            // case 0x13: return decode_I_arithmetic(bits);
            // case 0x03: return decode_I_load(bits);
            // case 0x67: return decode_I_jalr(bits);
            // case 0x23: return decode_S(bits);
            // case 0x63: return decode_B(bits);
            // case 0x37: return decode_U(bits, true);  // LUI
            // case 0x17: return decode_U(bits, false); // AUIPC
            // case 0x6F: return decode_J(bits);
            // case 0x73: return decode_System(bits);
            default: throw IllegalInstruction32(bits);
        }
    }

    Instruction fetch_and_decode(uint64_t addr, IMEM &mem) const override {
        if (addr % 4 != 0) throw InstructionAddressMisaligned(addr);
        uint32_t instr_bits = 0;
        
        try {
            instr_bits = mem.read_instr32(addr); // fetch
        } catch (const PageFault& e) {
            throw; 
        }  

        try {
            return decode(instr_bits);
        } catch (const IllegalInstruction32& e) {
            throw IllegalInstruction32PC(e.bits(), addr); 
        }   
    }

private:
    // R-Type: [funct7][rs2][rs1][funct3][rd][opcode]
    Instruction decode_R(uint32_t b) const {
        uint8_t rd = (b >> 7) & 0x1F;
        uint8_t rs1 = (b >> 15) & 0x1F;
        uint8_t rs2 = (b >> 20) & 0x1F;
        uint8_t f3 = (b >> 12) & 0x7;
        uint8_t f7 = (b >> 25) & 0x7F;

        if (f3 == 0x0 && f7 == 0x00) return Instruction(ADD{rd, rs1, rs2});
        // if (f3 == 0x0 && f7 == 0x20) return Instruction(SUB{rd, rs1, rs2});
        // Add SLL, SLT, SLTU, XOR, SRL, SRA, OR, AND here
        throw IllegalInstruction32(b);
    }

    // // I-Type: [imm][rs1][funct3][rd][opcode]
    // Instruction decode_I_arithmetic(uint32_t b) const {
    //     uint8_t rd = (b >> 7) & 0x1F;
    //     uint8_t rs1 = (b >> 15) & 0x1F;
    //     uint8_t f3 = (b >> 12) & 0x7;
    //     int32_t imm = static_cast<int32_t>(b) >> 20; // Sign-extend 12 bits

    //     if (f3 == 0x0) return Instruction(Addi{rd, rs1, imm});
    //     // Add SLTI, XORI, ORI, ANDI, SLLI, SRLI, SRAI
    //     throw IllegalInstruction32(b);
    // }

    // // S-Type: [imm11:5][rs2][rs1][funct3][imm4:0][opcode]
    // Instruction decode_S(uint32_t b) const {
    //     uint8_t rs1 = (b >> 15) & 0x1F;
    //     uint8_t rs2 = (b >> 20) & 0x1F;
    //     uint8_t f3 = (b >> 12) & 0x7;
    //     int32_t imm = ((static_cast<int32_t>(b) >> 25) << 5) | ((b >> 7) & 0x1F);

    //     if (f3 == 0x2) return Instruction(Sw{rs1, rs2, imm});
    //     // Add SB, SH
    //     throw IllegalInstruction32(b);
    // }

    // // B-Type: [imm12][imm10:5][rs2][rs1][funct3][imm4:1][imm11][opcode]
    // Instruction decode_B(uint32_t b) const {
    //     uint8_t rs1 = (b >> 15) & 0x1F;
    //     uint8_t rs2 = (b >> 20) & 0x1F;
    //     uint8_t f3 = (b >> 12) & 0x7;
        
    //     int32_t imm = ((b >> 31) << 12) |           // bit 12
    //                   (((b >> 7) & 0x1) << 11) |    // bit 11
    //                   (((b >> 25) & 0x3F) << 5) |   // bits 10:5
    //                   (((b >> 8) & 0xF) << 1);      // bits 4:1
    //     // Note: bit 0 is always 0 in RISC-V branches

    //     if (f3 == 0x0) return Instruction(Beq{rs1, rs2, imm});
    //     // Add BNE, BLT, BGE, BLTU, BGEU
    //     throw IllegalInstruction32(b);
    // }
};

} // namespace rv

