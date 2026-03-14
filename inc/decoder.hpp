#pragma once

#include "instruction.hpp"
#include "cpu.hpp"
#include "memory.hpp"

namespace rv
{

class IllegalInstruction32 : public std::runtime_error {
    uint32_t bits_;
public:
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
            case 0x13: return decode_I_arithmetic(bits);
            case 0x03: return decode_I_load(bits);
            case 0x67: return decode_I_jalr(bits);
            case 0x23: return decode_S(bits);
            case 0x63: return decode_B(bits);
            case 0x37: return decode_U(bits, true);
            case 0x17: return decode_U(bits, false); 
            case 0x6F: return decode_J(bits);
            case 0x73: return decode_System(bits);
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
        uint8_t rd = (b >> 7) & 0x1F, rs1 = (b >> 15) & 0x1F, rs2 = (b >> 20) & 0x1F;
        uint8_t f3 = (b >> 12) & 0x7, f7 = (b >> 25) & 0x7F;

        if (f7 == 0x00) {
            switch (f3) {
                case 0x0: return Instruction(ADD{rd, rs1, rs2});
                case 0x1: return Instruction(SLL{rd, rs1, rs2});
                case 0x2: return Instruction(SLT{rd, rs1, rs2});
                case 0x3: return Instruction(SLTU{rd, rs1, rs2});
                case 0x4: return Instruction(XOR{rd, rs1, rs2});
                case 0x5: return Instruction(SRL{rd, rs1, rs2});
                case 0x6: return Instruction(OR{rd, rs1, rs2});
                case 0x7: return Instruction(AND{rd, rs1, rs2});
            }
        } else if (f7 == 0x20) {
            if (f3 == 0x0) return Instruction(SUB{rd, rs1, rs2});
            if (f3 == 0x5) return Instruction(SRA{rd, rs1, rs2});
        }
        throw IllegalInstruction32(b);
    }

    // I-Type: [imm][rs1][funct3][rd][opcode]
    Instruction decode_I_arithmetic(uint32_t b) const {
        uint8_t rd = (b >> 7) & 0x1F, rs1 = (b >> 15) & 0x1F, f3 = (b >> 12) & 0x7;
        int32_t imm = static_cast<int32_t>(b) >> 20;

        switch (f3) {
            case 0x0: return (rs1 == 0) ? Instruction(LI{rd, imm}) : Instruction(ADDI{rd, rs1, imm});
            case 0x2: return Instruction(SLTI{rd, rs1, imm});
            case 0x3: return Instruction(SLTIU{rd, rs1, imm});
            case 0x4: return Instruction(XORI{rd, rs1, imm});
            case 0x6: return Instruction(ORI{rd, rs1, imm});
            case 0x7: return Instruction(ANDI{rd, rs1, imm});
            case 0x1: return Instruction(SLLI{rd, rs1, static_cast<uint8_t>(imm & 0x1F)});
            case 0x5: 
                if ((b >> 30) == 0x0) return Instruction(SRLI{rd, rs1, static_cast<uint8_t>(imm & 0x1F)});
                if ((b >> 30) == 0x2) return Instruction(SRAI{rd, rs1, static_cast<uint8_t>(imm & 0x1F)});
        }
        throw IllegalInstruction32(b);
    }

    Instruction decode_I_load(uint32_t b) const {
        uint8_t rd = (b >> 7) & 0x1F, rs1 = (b >> 15) & 0x1F, f3 = (b >> 12) & 0x7;
        int32_t imm = static_cast<int32_t>(b) >> 20;
        switch (f3) {
            case 0x0: return Instruction(LB{rd, rs1, imm});
            case 0x1: return Instruction(LH{rd, rs1, imm});
            case 0x2: return Instruction(LW{rd, rs1, imm});
            case 0x4: return Instruction(LBU{rd, rs1, imm});
            case 0x5: return Instruction(LHU{rd, rs1, imm});
        }
        throw IllegalInstruction32(b);
    }

    // S-Type: [imm11:5][rs2][rs1][funct3][imm4:0][opcode]
    Instruction decode_S(uint32_t b) const {
        uint8_t rs1 = (b >> 15) & 0x1F, rs2 = (b >> 20) & 0x1F, f3 = (b >> 12) & 0x7;
        int32_t imm = ((static_cast<int32_t>(b) >> 25) << 5) | ((b >> 7) & 0x1F);
        switch (f3) {
            case 0x0: return Instruction(SB{rs1, rs2, imm});
            case 0x1: return Instruction(SH{rs1, rs2, imm});
            case 0x2: return Instruction(SW{rs1, rs2, imm});
        }
        throw IllegalInstruction32(b);
    }

    // B-Type: [imm12][imm10:5][rs2][rs1][funct3][imm4:1][imm11][opcode]
    Instruction decode_B(uint32_t b) const {
        uint8_t rs1 = (b >> 15) & 0x1F, rs2 = (b >> 20) & 0x1F, f3 = (b >> 12) & 0x7;
        int32_t imm = ((static_cast<int32_t>(b) >> 31) << 12) | (((b >> 7) & 1) << 11) |
                      (((b >> 25) & 0x3F) << 5) | (((b >> 8) & 0xF) << 1);
        switch (f3) {
            case 0x0: return Instruction(BEQ{rs1, rs2, imm});
            case 0x1: return Instruction(BNE{rs1, rs2, imm});
            case 0x4: return Instruction(BLT{rs1, rs2, imm});
            case 0x5: return Instruction(BGE{rs1, rs2, imm});
            case 0x6: return Instruction(BLTU{rs1, rs2, imm});
            case 0x7: return Instruction(BGEU{rs1, rs2, imm});
        }
        throw IllegalInstruction32(b);
    }

    Instruction decode_U(uint32_t b, bool is_lui) const {
        uint8_t rd = (b >> 7) & 0x1F;
        int32_t imm = static_cast<int32_t>(b & 0xFFFFF000);
        return is_lui ? Instruction(LUI{rd, imm}) : Instruction(AUIPC{rd, imm});
    }

    Instruction decode_J(uint32_t b) const {
        uint8_t rd = (b >> 7) & 0x1F;
        int32_t imm = ((static_cast<int32_t>(b) >> 31) << 20) | (((b >> 12) & 0xFF) << 12) |
                      (((b >> 20) & 1) << 11) | (((b >> 21) & 0x3FF) << 1);
        return Instruction(JAL{rd, imm});
    }

    Instruction decode_I_jalr(uint32_t b) const {
        uint8_t rd = (b >> 7) & 0x1F, rs1 = (b >> 15) & 0x1F;
        int32_t imm = static_cast<int32_t>(b) >> 20;
        return Instruction(JALR{rd, rs1, imm});
    }

    Instruction decode_System(uint32_t b) const {
        std::cerr << "system instrctuions are not supported\n";
        throw IllegalInstruction32(b);

        // if ((b >> 20) == 0x000) return Instruction(ECALL{});
        // if ((b >> 20) == 0x001) return Instruction(EBREAK{});
        // throw IllegalInstruction32(b);
    }
};

} // namespace rv

