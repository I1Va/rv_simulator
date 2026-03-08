#pragma once

class ICPU {
    // overall RISCV CPU interface
    // orchestrates instructions execution, memory interaction 
    // contain:
    // registers (PC, x0-x31)
    // 

    
    // virtual uint32_t read_reg(int idx) = 0;
    // virtual void write_reg(int idx, uint32_t val) = 0;
    // virtual void set_pc(uint64_t pc) = 0;
    // virtual uint64_t pc() const = 0;
    // virtual void raise_exception(Exception e) = 0;
};

class CPU : public ICPU {
    // current RV32I CPU implementation
};

