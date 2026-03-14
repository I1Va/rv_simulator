#pragma once

#include <memory>

#include "cpu.hpp"
#include "memory.hpp"

namespace rv
{

// --- Arithmetic R-Type ---
struct ADD  { uint8_t rd, rs1, rs2; };
struct SUB  { uint8_t rd, rs1, rs2; };
struct SLL  { uint8_t rd, rs1, rs2; };
struct SLT  { uint8_t rd, rs1, rs2; };
struct SLTU { uint8_t rd, rs1, rs2; };
struct XOR  { uint8_t rd, rs1, rs2; };
struct SRL  { uint8_t rd, rs1, rs2; };
struct SRA  { uint8_t rd, rs1, rs2; };
struct OR   { uint8_t rd, rs1, rs2; };
struct AND  { uint8_t rd, rs1, rs2; };

// --- Arithmetic I-Type ---
struct ADDI  { uint8_t rd, rs1; int32_t imm; };
struct SLTI  { uint8_t rd, rs1; int32_t imm; };
struct SLTIU { uint8_t rd, rs1; int32_t imm; };
struct XORI  { uint8_t rd, rs1; int32_t imm; };
struct ORI   { uint8_t rd, rs1; int32_t imm; };
struct ANDI  { uint8_t rd, rs1; int32_t imm; };
struct SLLI  { uint8_t rd, rs1; uint8_t shamt; };
struct SRLI  { uint8_t rd, rs1; uint8_t shamt; };
struct SRAI  { uint8_t rd, rs1; uint8_t shamt; };

// --- Loads & Stores ---
struct LB  { uint8_t rd, rs1; int32_t imm; };
struct LH  { uint8_t rd, rs1; int32_t imm; };
struct LW  { uint8_t rd, rs1; int32_t imm; };
struct LBU { uint8_t rd, rs1; int32_t imm; };
struct LHU { uint8_t rd, rs1; int32_t imm; };
struct SB  { uint8_t rs1, rs2; int32_t imm; };
struct SH  { uint8_t rs1, rs2; int32_t imm; };
struct SW  { uint8_t rs1, rs2; int32_t imm; };

// --- Control Flow ---
struct BEQ  { uint8_t rs1, rs2; int32_t imm; };
struct BNE  { uint8_t rs1, rs2; int32_t imm; };
struct BLT  { uint8_t rs1, rs2; int32_t imm; };
struct BGE  { uint8_t rs1, rs2; int32_t imm; };
struct BLTU { uint8_t rs1, rs2; int32_t imm; };
struct BGEU { uint8_t rs1, rs2; int32_t imm; };
struct JAL  { uint8_t rd; int32_t imm; };
struct JALR { uint8_t rd, rs1; int32_t imm; };

// --- Upper Immediates & System ---
struct LUI   { uint8_t rd; int32_t imm; };
struct AUIPC { uint8_t rd; int32_t imm; };
struct ECALL  {};
struct EBREAK {};

// --- Pseudo Instructions ---
struct LI { uint8_t rd; int32_t imm; };


// --- Arithmetic Implementation (R-Type) ---
void execute(ADD  i, ICPU &c, IMEM &);
void execute(SUB  i, ICPU &c, IMEM &);
void execute(SLL  i, ICPU &c, IMEM &);
void execute(SLT  i, ICPU &c, IMEM &);
void execute(SLTU i, ICPU &c, IMEM &);
void execute(XOR  i, ICPU &c, IMEM &);
void execute(SRL  i, ICPU &c, IMEM &);
void execute(SRA  i, ICPU &c, IMEM &);
void execute(OR   i, ICPU &c, IMEM &);
void execute(AND  i, ICPU &c, IMEM &);

// --- Immediate Arithmetic (I-Type) ---
void execute(ADDI  i, ICPU &c, IMEM &);
void execute(SLTI  i, ICPU &c, IMEM &);
void execute(SLTIU i, ICPU &c, IMEM &);
void execute(XORI  i, ICPU &c, IMEM &);
void execute(ORI   i, ICPU &c, IMEM &);
void execute(ANDI  i, ICPU &c, IMEM &);
void execute(SLLI  i, ICPU &c, IMEM &);
void execute(SRLI  i, ICPU &c, IMEM &);
void execute(SRAI  i, ICPU &c, IMEM &);

// --- Memory Operations (Loads & Stores) ---
void execute(LB   i, ICPU &c, IMEM &m);
void execute(LH   i, ICPU &c, IMEM &m);
void execute(LW   i, ICPU &c, IMEM &m);
void execute(LBU  i, ICPU &c, IMEM &m);
void execute(LHU  i, ICPU &c, IMEM &m);
void execute(SB   i, ICPU &c, IMEM &m);
void execute(SH   i, ICPU &c, IMEM &m);
void execute(SW   i, ICPU &c, IMEM &m);

// --- Branching Logic (B-Type) ---
void execute(BEQ  i, ICPU &c, IMEM &);
void execute(BNE  i, ICPU &c, IMEM &);
void execute(BLT  i, ICPU &c, IMEM &);
void execute(BGE  i, ICPU &c, IMEM &);
void execute(BLTU i, ICPU &c, IMEM &);
void execute(BGEU i, ICPU &c, IMEM &);

// --- Jumps & Upper Immediates ---
void execute(JAL   i, ICPU &c, IMEM &);
void execute(JALR  i, ICPU &c, IMEM &);
void execute(LUI   i, ICPU &c, IMEM &);
void execute(AUIPC i, ICPU &c, IMEM &);

// --- System & Pseudo Instructions ---
void execute(ECALL  i, ICPU &c, IMEM &);
void execute(EBREAK i, ICPU &c, IMEM &);
void execute(LI     i, ICPU &c, IMEM &);


class Instruction {
    struct I_Instruction {
        virtual ~I_Instruction() = default;
        virtual std::unique_ptr<I_Instruction> copy_() const = 0;
        virtual void execute_(ICPU &, IMEM &) const = 0;
    };

    template <typename T>
    struct InstructionObject final : I_Instruction {
        T data_;
        InstructionObject(T x) : data_(std::move(x)) {}
        std::unique_ptr<I_Instruction> copy_() const override {
            return std::make_unique<InstructionObject>(*this);
        }

        void execute_(ICPU &cpu, IMEM &mem) const override {
            rv::execute(data_, cpu, mem);
        }
    };

    std::unique_ptr<I_Instruction> self_;

public:
    template <typename T>
    Instruction(T &x) : self_(std::make_unique<InstructionObject<T>>(x)) {}

    template <typename T>
    Instruction(T &&x) : self_(std::make_unique<InstructionObject<T>>(std::move(x))) {}

    template <typename T>
    Instruction operator=(T x) { 
        Instruction tmp{std::move(x)};
        std::swap(this->self_, tmp.self_); 
        return *this;
    }
    
    template <typename T>
    Instruction operator=(T &&x) { 
        self_ = std::make_unique<InstructionObject<T>>(std::move(x)); 
        return *this;
    }

public:
    friend void execute(const Instruction &x, ICPU &cpu, IMEM &mem) {
        x.self_->execute_(cpu, mem);
    }
};

} // namspace rv
