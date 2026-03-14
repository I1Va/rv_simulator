#pragma once

#include <cstdint>
#include <cassert>
#include <iomanip>
#include <string>
#include <vector>
#include <iostream>

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
    virtual void dump() const = 0;
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
};



} // namespace rv
