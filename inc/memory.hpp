#pragma once

class IMEM {
    // it is high abstraction interface, does not take into account memory model details
    // contain raw bytes of data
    // alow to read, write

    // virtual uint8_t read8(uint64_t a) = 0;
    // virtual uint32_t read32(uint64_t a) = 0;
    // virtual void write32(uint64_t a, uint32_t v) = 0;

};

class MEM : public IMEM {
    // basic RISCV memory model  
    // in future will support memory hierarchy 


};