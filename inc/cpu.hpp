#pragma once

#include <cstdint>
#include <cassert>

namespace rv
{

class ICPU {
public:
    // overall RISCV CPU interface
    // orchestrates instructions execution, memory interaction 
    // contain:
    // registers (PC, x0-x31)
    
    virtual uint64_t read_reg(uint8_t idx) const = 0;
    virtual void write_reg(uint8_t idx, uint64_t val) = 0;

    virtual void set_pc(uint64_t pc) = 0;
    virtual uint64_t pc() const = 0;
    // virtual void raise_exception(Exception e) = 0;
};

class CPU_RV32I : public ICPU {
    uint32_t pc_;
    std::vector<uint32_t> regs_;

public:
    CPU_RV32I() = default;

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

    // current RV32I CPU implementation
};



} // namespace rv
