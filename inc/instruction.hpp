#pragma once

#include "memory.hpp"
#include "cpu.hpp"

namespace rv
{

struct ADD {
    uint8_t rd, rs1, rs2;    
}; 

void execute(ADD instr, ICPU &cpu, IMEM &) {
    uint32_t val = static_cast<uint32_t>(cpu.read_reg(instr.rs1) + cpu.read_reg(instr.rs2));
    cpu.write_reg(instr.rd, val);
    cpu.set_pc(cpu.pc() + 4);
}

struct ADDI {
    uint8_t rd, rs1;
    int32_t imm; 
}; 

void execute(ADDI instr, ICPU &cpu, IMEM &) {
    uint32_t val = static_cast<uint32_t>(cpu.read_reg(instr.rs1) + instr.imm);
    cpu.write_reg(instr.rd, val);
    cpu.set_pc(cpu.pc() + 4);
}

struct SW {
    uint8_t rs1, rs2;
    int32_t imm;
}; 

void execute(SW instr, ICPU &cpu, IMEM &mem) {
    uint32_t addr = cpu.read_reg(instr.rs1) + instr.imm;
    mem.write32(addr, cpu.read_reg(instr.rs2));
    cpu.set_pc(cpu.pc() + 4);
}

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
